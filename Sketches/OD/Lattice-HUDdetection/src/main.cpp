// main.cpp
#include <iostream>
#include <fstream>
#include <time.h> 
#include <sstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <filesystem>
#include "Simulation.h"

// Tools for picking random numbers
std::mt19937 sickRNG(std::random_device{}()) ; 
std::uniform_real_distribution<double> sickDist(0,1); 

// Vectors of form {crops, sentinels}
std::vector<double> BETAS = {5*pow(10,-5),5*pow(10, -5)}; // daily per capita infection rate
std::vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
std::vector<double> GAMMAS = {452, 49};

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {100, 5}; // total number of plants {crops,sentinels}
bool baseline;

// Detection Parameters
int DELTA = 30;
int SAMPLE_SIZE = 5; // surveillance sample not ensemble sample
double epsilon = 1 * pow(10,-8); 
std::vector<double> RADIUS_VECTOR = {0,0.05,0.1,0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.2,0.21,0.22,0.23,
                                    0.24,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1,
                                    1.05,1.1,1.15,1.2,1.25,1.3,1.35,1.4}; 

// Sample Parameters
int NUMBER_OF_ENSEMBLE_COPIES = 1000; // total number of samples

double findMean(std::vector<std::vector<double>> matrix, int column) {
    int numberRows = matrix.size();
    double sum = 0;
    for (std::vector<double> row:matrix) {
        sum += row[column-1];
    }
    return sum/numberRows;
}

double findSTD(std::vector<std::vector<double>> matrix, int column, double mean) {
    int numberRows = matrix.size();
    double sum = 0;
    for (std::vector<double> row:matrix) {
        sum += pow((row[column-1]-mean),2);   
    }
    return sqrt(sum/(numberRows-1));
}

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

    // check if this is the baseline simulation
    if (TOTAL_NUMBER[1]==0){
        baseline = true;
    } else {
        baseline = false;
    }

    // Create file to store grid
    std::string FILENAME = "Lattice-Grid.txt"; 

    std::vector<int> SICK_PLANTS; //total number of sick plants
    double totalPlants = TOTAL_NUMBER[0] + TOTAL_NUMBER[1];

    for(double radius : RADIUS_VECTOR){

        // creates directory based on probelm setup
        char DIRECTORYNAMESETUP1[50]; 
        sprintf(DIRECTORYNAMESETUP1, "radius=%.2f", radius);  
        char DIRECTORYNAMESETUP2[128]; 
        sprintf(DIRECTORYNAMESETUP2, "Pcrops%d_Psentinels%d_NumberOfEnsembleCopies=%d", 
            TOTAL_NUMBER[0], TOTAL_NUMBER[1], NUMBER_OF_ENSEMBLE_COPIES);   
        std::filesystem::path setupPath = std::filesystem::path(DIRECTORYNAMESETUP1) / 
                                        std::filesystem::path(DIRECTORYNAMESETUP2);
        std::filesystem::create_directories(setupPath);

        // save EDP data
        char EDPMEANSETUP[50]; 
        sprintf(EDPMEANSETUP, "EDPmeanN=%d,Delta=%d.txt", SAMPLE_SIZE, DELTA);   
        char EDPSTDSETUP[50]; 
        sprintf(EDPSTDSETUP, "EDPstdN=%d,Delta=%d.txt", SAMPLE_SIZE, DELTA); 
        std::filesystem::path FilePathMean = setupPath / EDPMEANSETUP;
        std::filesystem::path FilePathSTD = setupPath / EDPSTDSETUP;
        std::ofstream MyFileMean(FilePathMean);
        std::ofstream MyFileSTD(FilePathSTD);
        std::vector<std::vector<double>> iterationPrevalence = {}; // store detection prevalence for each sample of this iteration
    
        // Loop over samples
        for (int sampleCounter = 1; sampleCounter <= NUMBER_OF_ENSEMBLE_COPIES; sampleCounter++){
            double randSickFraction = sickDist(sickRNG);
            if (randSickFraction < TOTAL_NUMBER[0]/totalPlants) {
                SICK_PLANTS = {1, 0};
            } else {
                SICK_PLANTS = {0, 1};
            }
            std::cout<<"Radius = "<<radius<<", "<<"Sample = "<<sampleCounter<<std::endl;
            std::vector<double> sampleDetectionPrevalence;
    
            sampleDetectionPrevalence = simulate(argc, argv, BETAS, EPSILONS, GAMMAS, 
            TOTAL_NUMBER, SICK_PLANTS, SAMPLE_SIZE, DELTA, baseline, radius + epsilon, FILENAME);

            iterationPrevalence.push_back(sampleDetectionPrevalence);
        }

        if (TOTAL_NUMBER[1] == 0) {
        MyFileMean << "EDP Mean" << std::endl;
        MyFileSTD << "EDP STD" << std::endl;
        double EDPmean = findMean(iterationPrevalence, 0);
        double EDPstd = findSTD(iterationPrevalence, 0, EDPmean);
        MyFileMean << EDPmean << std::endl;
        MyFileSTD << EDPstd << std::endl;
        } 
        else { 
            MyFileMean << "Number of Sentinels, EDP Mean" << std::endl;
            MyFileSTD << "Number of Sentinels, EDP STD" << std::endl;
            for (int iteration = 1; iteration < SAMPLE_SIZE; iteration++){
                double EDPmean = findMean(iterationPrevalence, iteration);
                double EDPstd = findSTD(iterationPrevalence, iteration, EDPmean);
                MyFileMean << iteration << "," << EDPmean << std::endl;
                MyFileSTD << iteration << "," << EDPstd << std::endl;
            }
        }
    }
       
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
