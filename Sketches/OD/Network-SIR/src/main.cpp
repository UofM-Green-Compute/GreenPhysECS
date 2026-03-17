/*
Oluwole Delano
Created 10/3/25 

Helpful resource on entity relationships: 
- https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c
- https://www.flecs.dev/flecs/md_docs_2Relationships.html

*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

// Code constants
int TIME_STEPS = 100; 

// Network constants
int NO_PEOPLE = 100; // Total number of people
std::vector<int> population_sizes = {0, 0, 0}; // {no. Susceptible, no. Infected, no. Recovered}
double c = 0.01; // Probability of forming a connection 

// Transition constants
double beta = 0.1 / static_cast<double>(NO_PEOPLE); 
double alpha = 0.01; 

// Network components
struct peopleIndex { int i; }; // Label for each individual in the population
struct susceptibleConnections { int no; }; // No. of susceptible people an individual is connected to
struct infectedConnections { int no; }; // No. of infected people an individual is connected to

// Infection components
struct State { int s; }; // State that an individual is in (S, I, or R)
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2,3

void createNodes(flecs::world world, std::vector<flecs::entity> &p, int nI) {

    population_sizes = {NO_PEOPLE - nI, nI, 0};

    // Create 'Susceptible' nodes
    for(int i = 0; i < population_sizes[0]; i++){
        p.push_back(
            world.entity()
                .set<State>({1})
                .set<peopleIndex>({i})
                .set<susceptibleConnections>({0})
                .set<infectedConnections>({0})
                .set<TransitionProbabilities>({{0,0,0}})
        ); 
    }
    // Create 'Infected' nodes 
    for(int j = 0; j < population_sizes[1]; j++){
        p.push_back(
            world.entity()
                .set<State>({2})
                .set<peopleIndex>({j})
                .set<susceptibleConnections>({0})
                .set<infectedConnections>({0})
                .set<TransitionProbabilities>({{0,0,0}})
        ); 
    }

}

int main(int argc, char* argv[]) {

    // Create the world 
    flecs::world world;
    
    // Create a vector for the population
    std::vector<flecs::entity> population; 
    population.reserve(NO_PEOPLE); 

    // Initialise the components 
    world.component<peopleIndex>(); 
    world.component<susceptibleConnections>(); 
    world.component<infectedConnections>(); 

    // Relationship types 
    flecs::entity Link = world.entity();
    flecs::entity infectionLink = world.entity(); 

    // Initialise the nodes
    createNodes(world, population, 1); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1);
    
    // File handling
    std::ofstream MyFileNetworkSIR;
    MyFileNetworkSIR.open("Network-SIR.txt"); 
    std::ofstream MyFileEdges;
    MyFileEdges.open("Network-Edges.txt"); 

    // System to form the network - An Erdos-Renyi graph
    world.system<peopleIndex, susceptibleConnections, infectedConnections, State>()
        .each([&](flecs::entity e, peopleIndex &person, susceptibleConnections &susceptible, infectedConnections &infected, 
            State &s){

            // Tools for picking random numbers 
            std::mt19937 rng( std::random_device{}() ); 
            std::uniform_real_distribution<double> dist(0,1); 

            for(int k = 0; k < NO_PEOPLE - 1; k++){
                // Flip a coin 
                double rand = dist(rng); 
                if ((rand < c) && (person.i != k) && (s.s == 1)) { 
                    e.add(Link, population[k]); 
                    population[k].add(Link, e); 
                    MyFileEdges << person.i << "," << k << ";" ; 
                }
                else if ((rand < c) && (person.i != k) && (s.s == 2)) { 
                    e.add(infectionLink, population[k]); 
                    population[k].add(infectionLink, e);
                }
            }

        }); 

    // Find the transition probabilities for each entity 
    world.system<peopleIndex, State, infectedConnections, TransitionProbabilities>()
        .each([&](flecs::entity e, peopleIndex& person, State& state, infectedConnections infected,TransitionProbabilities& p_vector){

            // Query how many infection links the entity has
            infected.no = 0; 
            for(int l = 0; l < population.size(); l++){
                if( e.has(infectionLink,population[l]) ) { 
                    infected.no += 1; 
                    if(l != population.size()) { MyFileEdges << person.i << "," << l << ";"; }
                }
            }

            // Set the transition coefficients 
            if( state.s == 1 ){
                p_vector.qnm = {1 - (beta * infected.no),beta * infected.no,0}; 
            }
            else if ( state.s == 2 ) {
                p_vector.qnm = {0,1 - alpha,alpha}; 
            }
            else if ( state.s == 3 ) {
                p_vector.qnm = {0,0,1};
            }
            
        }); 

    // Evolve the state of each entity
    world.system<peopleIndex, State, TransitionProbabilities>()
        .each([&](flecs::entity e, peopleIndex& person, State& state, TransitionProbabilities& p_vector){

            double rand = dist(rng); 
            double probability_sum = 0; 
            bool transition = false; 

            for(int i = 1; i <= p_vector.qnm.size(); i++){
                if (state.s != i) { 
                    probability_sum += p_vector.qnm[i-1]; 
                    if ( (rand < probability_sum) ){
                        population_sizes[state.s-1] -= 1;
                        population_sizes[i-1] += 1; 
                        e.set<State>({i});
                        break; 
                    }
                }
            }
        
        }); 

    // Run the world TIME_STEPS times
    // Order of edges matters
    for(int j = 0; j < TIME_STEPS; j++){
        MyFileEdges << j << ";" ;
        MyFileNetworkSIR << j << "," << population_sizes[0] << "," << population_sizes[1] << "," << population_sizes[2] << std::endl; 
        world.progress(); 
        MyFileEdges << std::endl; 
    } 

    MyFileNetworkSIR.close(); 

    // population[0].each(infectionLink, flecs::Wildcard, [](flecs::id id){
    //     std::cout << "Bob has an infection link to " << id.second().get<peopleIndex>().i 
    //     << ". Bob has " << id.second().get<infectedConnections>().no << " infected connections. " << std::endl; 
    // }); 

    // population[0].each(Link, flecs::Wildcard, [](flecs::id id){
    //     std::cout << "Bob has a link to " << id.second().get<peopleIndex>().i 
    //     << ". Bob has " << id.second().get<susceptibleConnections>().no << " connections. " << std::endl; 
    // }); 

}

