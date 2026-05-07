// main.cpp
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include <fstream>
#include <sstream>
#include "simulation.h"
#include <ccenergy/EnergyTracker.hpp>

// Initial Conditions
int NO_PEOPLE = 100; // Total number of people
int I0 = 10; // Number of initially infected peoples
double timeStep = 0.005; // Set timestep = 0.0001
double maxTime = 5; // Maximum Simulation Time
double beta = 7 / static_cast<double>(NO_PEOPLE); // infection rate
double alpha = 1; // recovery rate
int sampleCounter1 = 1; // Set sample counter = 1
int sampleCounter2 = 1; // Set sample counter = 1
int sampleNumber = 100; // total number of samples

int main(int argc, char* argv[]) {
    /*
    This function:
    1) Creates and starts a program clock
    3) Runs the simulation
    4) Stops and prints program clock
    */

    // Start measuring run time of program
    
    ccenergy::EnergyTracker energy_tracker {{ .label = "OnUpdate",
                                              .measure_cpu = true,
                                              .measure_gpu  = false,
                                              .log_to_stdout = false }};
    energy_tracker.start();
    clock_t tClock; 
    tClock = clock(); 

    // Loop over samples
    std::vector<std::string> fileNames; 
    while (sampleCounter1 <= sampleNumber){
        char filename[50]; 
        sprintf(filename, "SIR_%d.txt", sampleCounter1); // Create filename
        fileNames.push_back(filename); // Update filenames vector 
        std::cout<<"Sample = "<< sampleCounter1 <<std::endl; // Print simulation number
        simulate(argc, argv, beta, alpha, NO_PEOPLE, I0, timeStep, maxTime, filename); // Run Simulation
        sampleCounter1 += 1; // increase sample counter
    }
    
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;

    // stop energy tracking
    auto r = energy_tracker.stop();

    // Reporting the information from the energy tracker
    std::cout << energy_tracker.mkReport() << std::endl;
}
