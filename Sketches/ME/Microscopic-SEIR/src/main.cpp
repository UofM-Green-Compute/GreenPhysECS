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
int I0 = 1; // Number of initially infected peoples
double timeStep = 0.005; // Set timestep = 0.0001
double maxTime = 10; // Maximum Simulation Time
double beta = 7 / static_cast<double>(NO_PEOPLE); // infection rate in infections per infectious person per day
double a = 1; // latency rate 
double alpha = 1; // recovery rate in recoviers per day
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
    
    clock_t tClock; 
    tClock = clock(); 

    // Loop over samples
    std::vector<std::string> fileNames; 
    while (sampleCounter1 <= sampleNumber){
        char filename[50]; 
        sprintf(filename, "SEIR_%d.txt", sampleCounter1); // Create filename
        fileNames.push_back(filename); // Update filenames vector 
        std::cout<<"Sample = "<< sampleCounter1 <<std::endl; // Print simulation number
        simulate(argc, argv, beta, a, alpha, NO_PEOPLE, I0, timeStep, maxTime, filename); // Run Simulation
        sampleCounter1 += 1; // increase sample counter
    }
    
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
