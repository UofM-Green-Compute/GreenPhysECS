/*
Oluwole Delano
12/2/26

*** Transition Matrix *** 
 { q_11 | q_12 }
 { q_21 | q_22 }
   -- where q_nm = q_n<-m = probability to transition from state n to state m
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

int TIME_STEPS = 1000; 

// Counters to keep track of how many people are in each state
int NO_PEOPLE = 100; // Total number of people
std::vector<int> population_sizes = {0, 0}; // {no. Susceptible, no. Infected}

// Transition constants
double beta = 0.1 / static_cast<double>(NO_PEOPLE); 
double alpha = 0.01; 

// Constants for creating the phase portrait
int NO_PHASE_RUNS = 10; 

// Transition probabilities
// -- where q_nm = q_n<-m = probability to transition from state n to state m
double Q21 = beta * population_sizes[1]; 
double Q11 = 1 - Q21;
double Q12 = alpha;
double Q22 = 1 - Q12;

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct PeopleIndex { int p; }; 
struct State { int s; }; // Track current state of entity
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2

// Function to set the initial condition 
void initial_condition(flecs::world world, std::vector<flecs::entity> &p, int nI){

    // -- Set initial numbers of each population -- 
    population_sizes = {NO_PEOPLE - nI, nI};

    // -- Set the initial transition probabilities -- 
    Q21 = beta * population_sizes[1]; 
    Q11 = 1 - Q21;

    // Delete any previous entities (made in previous phase portrait loop)
    for(int l = 0; l < p.size(); l++){
        ecs_delete(world, p[l]); 
    }
    p.clear(); 
    
    // Initialise entities
    for (int i = 0; i < population_sizes[0]; ++i) { 
        std::vector<double> Qnm = {Q11, Q21}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<State>({1})
                .set<TransitionProbabilities>({Qnm})
        ); 
    } 
    for (int i = 0; i < population_sizes[1]; ++i) { 
        std::vector<double> Qnm = {Q12, Q22}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({population_sizes[0] + i})
                .set<State>({2})
                .set<TransitionProbabilities>({Qnm})
        ); 
    }  

}

// Set the transition probability vector based on current state of the system
std::vector<double> calculateProbabilities(std::vector<int> population_vector, int state){

    std::vector<double> probabilities = {0, 0}; 

    if (state == 1){
        double q21 = beta * population_vector[1];
        double q11 = 1 - q21;
        probabilities[0] = q11; 
        probabilities[1] = q21; 
    }
    else if (state == 2){
        double q12 = alpha;
        double q22 = 1 - q12;
        probabilities[0] = q12; 
        probabilities[1] = q22; 
    }

    return probabilities; 
}

int main(int argc, char* argv[]) {

    // Start measuring run time of program
    clock_t t; 
    t = clock(); 

    // ******** Programming setup ******** 

    // File handling
    std::ofstream MyFileSIS;

    // Create file names for phase portrait 
    std::vector<std::string> fileNames; 
    for(int i = 0; i <= NO_PHASE_RUNS; i++){
        char file_name[50]; 
        sprintf(file_name, "SIS_%d.txt", i); 
        fileNames.push_back(file_name); 
    }

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    world.component<PeopleIndex>(); 
    world.component<State>(); 
    world.component<TransitionProbabilities>();

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // ******** Physics ********

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(2*NO_PEOPLE); 

    // Set the initial conditions
    initial_condition(world, people, 1); 

    // Evolution for entities 
    world.system<PeopleIndex, State, TransitionProbabilities>()
        .each([&](flecs::entity e, PeopleIndex& person, State& state, TransitionProbabilities& p_vector){

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

        // System to update transition probabilities
        world.system<PeopleIndex, State, TransitionProbabilities>()
        .each([&](flecs::entity e, PeopleIndex& person, State& state, TransitionProbabilities& p_vector){

            std::vector<double> qnm = calculateProbabilities(population_sizes, state.s); 
            e.set<TransitionProbabilities>({qnm});
            
        }); 

        for(int i = 0; i <= NO_PHASE_RUNS; i++){

            // Set the initial condition for each run
            initial_condition(world, people, i); 

            // Open the corresponding file for each run
            MyFileSIS.open(fileNames[i]);
            if (!MyFileSIS.is_open()){
                std::cout<<"Error in creating file"<<std::endl; 
                return 1; 
            }
            MyFileSIS << "Time,Susceptible,Infected" << std::endl; 

            // Run the world TIME_STEPS times
            for(int j = 0; j < TIME_STEPS; j++){
                MyFileSIS << j << "," << population_sizes[0] << "," << population_sizes[1] << std::endl; 
                world.progress(); 
            }

            // Close the file
            MyFileSIS.close(); 

        }

        MyFileSIS.open("SIS.txt");
        // Check if file is open
        if (!MyFileSIS.is_open()){
            std::cout<<"Error in creating file"<<std::endl; 
            return 1; 
        }
        MyFileSIS << "Time,Susceptible,Infected" << std::endl; 

        initial_condition(world, people, 1); 

        for(int k = 0; k < TIME_STEPS; k++){
            MyFileSIS << k << "," << population_sizes[0] << "," << population_sizes[1] << std::endl; 
            world.progress(); 
        }
        MyFileSIS.close();

    t = clock() - t; 
    double time_taken = ((double)t) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
    
}