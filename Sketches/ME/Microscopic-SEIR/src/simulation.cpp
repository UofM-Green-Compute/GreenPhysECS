//simulation.cpp
#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include "systems.h"

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct PeopleIndex { int p; }; 
struct State { int s; }; // Track current state of entity
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2

// Tools for picking random numbers 
std::mt19937 rng( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> dist(0,1); 

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected){
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<State>({1})
                .set<TransitionProbabilities>({Qnm})
        ); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({totalPopulation-initialInfected + i})
                .set<State>({3})
                .set<TransitionProbabilities>({Qnm})
        ); 
    }
}

void setupComponents(flecs::world world){
    world.component<PeopleIndex>(); 
    world.component<State>(); 
    world.component<TransitionProbabilities>();
}

void setupSystems(flecs::world world, std::vector<int> &population, const double infectionRate,
    const double latencyRate, const double recoveryRate, const double step) {
    updateProbabilities(world, population, infectionRate, latencyRate, recoveryRate, step);
    transition(world, population);
}

int simulate(int argc, char* argv[], const double infectionRate, const double latencyRate,
             const double recoveryRate, const int totalPopulation, const int initialInfected,
             const double step, const double totalTime, std::string filename) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */

    // Population vector will be updated and saved over time
    std::vector<int> population_vector = {totalPopulation-initialInfected, 0, initialInfected, 0};

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    setupComponents(world);

    // Create a people vector and generate entities
    std::vector<flecs::entity> people; 
    people.reserve(totalPopulation); 
    setupEntities(world, people, totalPopulation, initialInfected); 

    // Create the systems
    setupSystems(world, population_vector, infectionRate, latencyRate, recoveryRate, step);

    // Run the simulation
    std::ofstream MyFile;
    MyFile.open(filename);
    MyFile << "Time,Susceptible,Exposed,Infected,Recovered" << std::endl; 
    double time = 0;
    MyFile << time << "," << population_vector[0] << "," << population_vector[1] << ","
           << population_vector[2] << ","<<population_vector[3]<<std::endl;  
    while (time<totalTime) {
        world.progress();
        time += step;
        // update sample variables
        MyFile << time << "," << population_vector[0] << "," << population_vector[1] << ","
               << population_vector[2] << "," << population_vector[3] << std::endl;  
    }
    MyFile.close();
    return 0;
}
