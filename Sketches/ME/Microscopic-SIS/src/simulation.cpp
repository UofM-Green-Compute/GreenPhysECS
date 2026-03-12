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
#include "epidemicFunctions.h"

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct PeopleIndex { int p; }; 
struct State { int s; }; // Track current state of entity
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2

// Function to set the initial condition 
void initialSetup(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected){

    // Initialise entities
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<State>({1})
                .set<TransitionProbabilities>({Qnm})
        ); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({totalPopulation-initialInfected + i})
                .set<State>({2})
                .set<TransitionProbabilities>({Qnm})
        ); 
    }
}

// Set the transition probability vector based on current state of the system
std::vector<double> calculateProbabilities(std::vector<int> population_vector, int state,
    const double infectionRate, const double recoveryRate){
    std::vector<double> probabilities = {0, 0}; 
    if (state == 1){
        double q21 = infectionRate * population_vector[1];
        double q11 = 1 - q21;
        probabilities[0] = q11; 
        probabilities[1] = q21; 
    }
    else if (state == 2){
        double q12 = recoveryRate;
        double q22 = 1 - q12;
        probabilities[0] = q12; 
        probabilities[1] = q22; 
    }
    return probabilities; 
}

int simulate(int argc, char* argv[], const double infectionRate, const double recoveryRate, 
const int totalPopulation, const int initialInfected, const double step, const double totalTime) {

    // Start measuring run time of program
    clock_t t; 
    t = clock(); 

    // ******** Programming setup ******** 

    // File handling
    std::ofstream MyFileSIS;

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
    std::vector<int> population_vector = {totalPopulation-initialInfected, initialInfected, 0}; // Initial Population Vector
    std::vector<flecs::entity> people; 
    people.reserve(totalPopulation); 

    // Evolution for entities 
    world.system<State, TransitionProbabilities>()
        .each([&](flecs::entity e, State& state, TransitionProbabilities& p_vector){
            transition(e, state.s, p_vector.qnm, population_vector);
        }); 

    // System to update transition probabilities
    world.system<State>()
    .each([&](flecs::entity e, State& state){
        std::vector<double> qnm = calculateProbabilities(population_vector, state.s, infectionRate, 
            recoveryRate); 
        e.set<TransitionProbabilities>({qnm});
    }); 

    MyFileSIS.open("SIS.txt");
    // Check if file is open
    if (!MyFileSIS.is_open()){
        std::cout<<"Error in creating file"<<std::endl; 
        return 1; 
    }
    MyFileSIS << "Time,Susceptible,Infected" << std::endl; 

    initialSetup(world, people, totalPopulation, initialInfected); 

    while (t<totalTime) {
        world.progress();
        t += step;
        // update sample variables
        MyFileSIS << t << "," << population_vector[0] << "," << population_vector[1] << std::endl;  
    }
    MyFileSIS.close();

    t = clock() - t; 
    double time_taken = ((double)t) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
    return 0;
}
