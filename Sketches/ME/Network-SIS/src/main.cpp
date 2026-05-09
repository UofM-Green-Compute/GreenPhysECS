/*
Oluwole Delano
Created 10/3/26

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
int TIME_STEPS = 30000; // Number of elapsed time steps
double deltaT = 0.01;

// Network constants
int NO_PEOPLE = 100; // Total number of people
std::vector<int> population_sizes = {0, 0}; // {no. Susceptible, no. Infected}
double c = 0.02; // Probability of forming a connection

// Transition constants
double beta = 7 / static_cast<double>(NO_PEOPLE);
double alpha = 0.005; 

// Network components
struct peopleIndex { int i; }; // Label for each individual in the population
struct infectedConnections { int no; }; // No. of infected people an individual is connected to
// Infection components
struct State { int s; }; // State that an individual is in (S or I)
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2

void createNodes(flecs::world world, std::vector<flecs::entity> &p, int nI) {
    
    population_sizes = {NO_PEOPLE - nI, nI};

    // Create 'Susceptible' nodes
    for(int i = 0; i < population_sizes[0]; i++){
        p.push_back(
            world.entity()
                .set<State>({1})
                .set<peopleIndex>({i})
                .set<infectedConnections>({0})
                .set<TransitionProbabilities>({{0,0}})
        );
    }
    // Create 'Infected' nodes
    for(int j = 0; j < population_sizes[1]; j++){
        p.push_back(
            world.entity()
            .set<State>({2})
            .set<peopleIndex>({population_sizes[0]+j})
            .set<infectedConnections>({0})
            .set<TransitionProbabilities>({{0,0}})
        );
    }

}

void ErdosRenyiGraph(std::vector<flecs::entity> &population, std::ofstream &MyFile, flecs::entity sLink, flecs::entity infcLink){
    
    // Tools for picking random numbers
    std::mt19937 rng( std::random_device{}() ) ;
    std::uniform_real_distribution<double> dist(0,1);

    for(int i = 0; i < NO_PEOPLE; i++){
        for(int k = i+1; k < NO_PEOPLE; k++){
            // Flip a coin
            double rand = dist(rng);
            if ((rand < c) && (i != k) && (population[i].get<State>().s != 2) && (population[k].get<State>().s != 2)) {
                population[i].add(sLink, population[k]);
                population[k].add(sLink, population[i]);
                MyFile << population[i].get<peopleIndex>().i << "," << k << ";" ;
            }
            else if ((rand < c) && (i != k) && ((population[i].get<State>().s) == 2 || (population[k].get<State>().s == 2))) {
                population[i].add(infcLink, population[k]);
                population[k].add(infcLink, population[i]);
                MyFile << population[i].get<peopleIndex>().i << "," << k << ";" ;
            }
        }
    }
    MyFile << std::endl;
}

int main(int argc, char* argv[]) {

    // Create the world
    flecs::world world;

    // Create a vector for the population
    std::vector<flecs::entity> population;
    population.reserve(NO_PEOPLE);

    // Initialise the components
    world.component<peopleIndex>();
    world.component<infectedConnections>();

    // Relationship types
    flecs::entity Link = world.entity();
    flecs::entity infectionLink = world.entity();

    // Tools for picking random numbers
    std::mt19937 rng( std::random_device{}() ) ;
    std::uniform_real_distribution<double> dist(0,1);

    // File handling
    std::ofstream MyFileNetworkSIR;
    MyFileNetworkSIR.open("Network-SIR.txt");
    std::ofstream MyFileEdges;
    MyFileEdges.open("Network-Edges.txt");

    // Initialise the nodes
    createNodes(world, population, 1);

    // Create the network - An Erdos-Renyi graph
    ErdosRenyiGraph(population, MyFileEdges, Link, infectionLink);
    MyFileEdges.close();

    // Find the transition probabilities for each entity
    world.system<peopleIndex, State, infectedConnections, TransitionProbabilities>()
        .each([&](flecs::entity e, peopleIndex& person, State& state, infectedConnections infected,TransitionProbabilities& p_vector){
            
            // Query how many infection links the entity has
            infected.no = 0;
            for(int l = 0; l < population.size(); l++){
                if( e.has(infectionLink,population[l]) || population[l].has(infectionLink,e)) {
                    infected.no += 1;
                }
            }

            // Set the transition coefficients
            if( state.s == 1 ){
                p_vector.qnm = {1 - (beta * infected.no * deltaT),beta * infected.no * deltaT};
            }
            else if ( state.s == 2 ) {
                p_vector.qnm = {alpha * deltaT,1 - (alpha * deltaT)};
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

    // Update the links between entities
    world.system<peopleIndex, State, TransitionProbabilities>()
        .each([&](flecs::entity e, peopleIndex& person, State& state, TransitionProbabilities& p_vector){

            for(int j = 0; j < NO_PEOPLE; j++){
                // Remove infected links that are gone
                if( (person.i != j) && (state.s != 2) && (population[j].get<State>().s != 2) && e.has(infectionLink,population[j]) ) {
                    e.remove(infectionLink,population[j]);
                    e.remove(Link,population[j]);
                    e.add(Link,population[j]);
                }
                else if( (person.i != j) && (state.s != 2) && (population[j].get<State>().s != 2) && population[j].has(infectionLink,e)) {
                    population[j].remove(infectionLink,e);
                    population[j].remove(Link,e);
                    population[j].add(Link,e);
                }
                // Add the new infection links
                if( (person.i != j) && ((state.s == 2) || (population[j].get<State>().s == 2)) && !(e.has(infectionLink,population[j])) && (e.has(Link,population[j]))) {
                    e.remove(Link,population[j]);
                    e.add(infectionLink,population[j]);
                }
                else if( (person.i != j) && ((state.s == 2) || (population[j].get<State>().s == 2)) && !(population[j].has(infectionLink,e)) && (population[j].has(Link,e))) {
                    population[j].remove(Link,e);
                    population[j].add(infectionLink,e);
                }
            }
        });

    // Run the world TIME_STEPS times
    for(int j = 0; j < TIME_STEPS; j++){
        MyFileNetworkSIR << j * deltaT << "," << population_sizes[0] << "," << population_sizes[1] << std::endl;

        world.progress();
    }

    MyFileNetworkSIR.close();

}
