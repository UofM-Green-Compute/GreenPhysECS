#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include <fstream>
#include <sstream>
#include <cmath>
#include <filesystem>
#include "simulation.h"

// Parameters
int MAX_TIME = 3000; // Maximum Simulation Time

// Vectors of form {crops, sentinels}
std::vector<double> BETAS = {5*pow(10,-5),5*pow(10, -5)}; // daily per capita infection rate
std::vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
std::vector<double> GAMMAS = {452, 49};

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {1000, 50}; // total number of plants
std::vector<int> SICK_PLANTS = {3, 0}; //total number of sick plants

// Detection Parameters
int DELTA = 10;
std::vector<int> SAMPLE_SIZES = {0, 25};

// Sample Parameters
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

    // creates directory based on 1) probelm setup and 2) surveillance strategy
    char DIRECTORYNAMESETUP[50]; 
    char DIRECTORYNAMESURVEILLANCE[50]; 
    sprintf(DIRECTORYNAMESETUP, "Pcrops%d_Psentinels%d", 
        TOTAL_NUMBER[0], TOTAL_NUMBER[1]);   
    sprintf(DIRECTORYNAMESURVEILLANCE, "Delta%dNcrops%d_Nsentinels%d", 
        DELTA, SAMPLE_SIZES[0], SAMPLE_SIZES[1]);
    std::filesystem::path setupPath = DIRECTORYNAMESETUP;
    std::filesystem::create_directory(setupPath);
    std::filesystem::path dir_path = setupPath / DIRECTORYNAMESURVEILLANCE;
    std::filesystem::create_directory(dir_path);
    
    // Loop over samples
    while (sampleCounter1 <= sampleNumber){
        char FILENAME1[50]; 
        char FILENAME2[50]; 
        sprintf(FILENAME1, "HUDcrops_%d.txt", sampleCounter1);
        sprintf(FILENAME2, "HUDsentinels_%d.txt", sampleCounter1);
        std::filesystem::path FILEPATH1 = dir_path / FILENAME1;
        std::filesystem::path FILEPATH2 = dir_path / FILENAME2;
        std::cout<<"Sample = "<< sampleCounter1 <<std::endl;
        simulate(argc, argv, BETAS, EPSILONS, GAMMAS, TOTAL_NUMBER, SICK_PLANTS, DELTA, SAMPLE_SIZES, MAX_TIME, 
                FILEPATH1, FILEPATH2);
        sampleCounter1 += 1;
    }
    
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
