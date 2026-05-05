#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include "Simulation.h"

// Parameters
int MAX_TIME = 3000; // Maximum Simulation Time

double RADIUS = 1.5;

// Vectors of form {crops, sentinels}
std::vector<double> BETAS = {5*pow(10,-5),5*pow(10,-5)}; // daily per capita infection rate
std::vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
std::vector<double> GAMMAS = {452, 49};

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {50, 50}; // total number of plants {Crops, Sentinels}
std::vector<int> SICK_PLANTS = {5, 0}; // total number of sick plants {Crops, Sentinels}

// Sample Parameters
int sampleNumber = 2; // total number of samples

int main(int argc, char* argv[]) {

    // Start measuring run time of program 
    clock_t tClock; 
    tClock = clock(); 

    // Create the world 
    flecs::world world;

    for(int sampleCounter = 1; sampleCounter <= sampleNumber; sampleCounter++){
        std::cout<<"Sample "<<sampleCounter<<std::endl; 
        // Set file names
        char FILENAME1[50]; 
        char FILENAME2[50]; 
        sprintf(FILENAME1, "HUDcrops_%d.txt", sampleCounter);
        sprintf(FILENAME2, "HUDsentinels_%d.txt", sampleCounter);
        std::string FILENAME3 = "Lattice-Grid.txt"; 
        // Run Simulation
        simulate(argc, argv, BETAS, EPSILONS, GAMMAS, TOTAL_NUMBER, SICK_PLANTS, MAX_TIME, RADIUS, FILENAME1, FILENAME2, FILENAME3); 
    }

    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}

