#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include <fstream>
#include <sstream>
#include <cmath>
#include "simulation.h"

// Parameters
int MAX_TIME = 3000; // Maximum Simulation Time
double RADIUS = sqrt(2);

// Vectors of form {crops, sentinels}
std::vector<double> BETAS = {5*pow(10,-5),5*pow(10,-5)}; // daily per capita infection rate
std::vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
std::vector<double> GAMMAS = {452, 49};

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {100, 100}; // total number of plants
std::vector<int> SICK_PLANTS = {5, 0}; //total number of sick plants

// Sample Parameters
int sampleCounter1 = 1; // Set sample counter = 1
int sampleCounter2 = 1; // Set sample counter = 1
int sampleNumber = 1; // total number of samples

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
    while (sampleCounter1 <= sampleNumber){
        char FILENAME1[50]; 
        char FILENAME2[50]; 
        sprintf(FILENAME1, "HUDcrops_%d.txt", sampleCounter1);
        sprintf(FILENAME2, "HUDsentinels_%d.txt", sampleCounter1);
        std::cout<<"Sample = "<< sampleCounter1 <<std::endl;
        simulate(argc, argv, BETAS, EPSILONS, GAMMAS, TOTAL_NUMBER, SICK_PLANTS, MAX_TIME, FILENAME1, FILENAME2,
                 RADIUS);
        sampleCounter1 += 1;
    }
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
