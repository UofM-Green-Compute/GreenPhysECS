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
#include "simulation.h"

using namespace std;

// Tools for picking random numbers 
mt19937 sickRNG(random_device{}()) ; 
uniform_real_distribution<double> sickDist(0,1); 

// Vectors of form {crops, sentinels}
vector<double> BETAS = {5*pow(10,-5),5*pow(10, -5)}; // daily per capita infection rate
vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
vector<double> GAMMAS = {452, 49};

// Initial Conditions
vector<int> TOTAL_NUMBER = {1000, 20}; // total number of plants
bool baseline;

// Detection Parameters
int DELTA = 30;
int SAMPLE_SIZE = 20;

// Sample Parameters
int sampleNumber = 1000; // total number of samples

double findMean(vector<vector<double>> matrix, int column) {
    int numberRows = matrix.size();
    double sum = 0;
    for (vector<double> row:matrix) {
        sum += row[column];
    }
    return sum/numberRows;
}

double findSTD(vector<vector<double>> matrix, int column, double mean) {
    int numberRows = matrix.size();
    double sum = 0;
    for (vector<double> row:matrix) {
        sum += pow((row[column]-mean),2);   
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
    
    vector<int> SICK_PLANTS; //total number of sick plants
    double totalPlants = TOTAL_NUMBER[0] + TOTAL_NUMBER[1];

    // creates directory based on probelm setup
    char DIRECTORYNAMESETUP[50]; 
    sprintf(DIRECTORYNAMESETUP, "Pcrops%d_Psentinels%d", 
        TOTAL_NUMBER[0], TOTAL_NUMBER[1]);   
    filesystem::path setupPath = DIRECTORYNAMESETUP;
    filesystem::create_directory(setupPath);

    // save EDP data
    char EDPMEANSETUP[50]; 
    sprintf(EDPMEANSETUP, "EDPmeanN=%d,Delta=%d.txt", 
        SAMPLE_SIZE, DELTA);   
    char EDPSTDSETUP[50]; 
    sprintf(EDPSTDSETUP, "EDPstdN=%d,Delta=%d.txt", 
        SAMPLE_SIZE, DELTA); 
    filesystem::path FilePathMean = setupPath / EDPMEANSETUP;
    filesystem::path FilePathSTD = setupPath / EDPSTDSETUP;
    ofstream MyFileMean(FilePathMean);
    ofstream MyFileSTD(FilePathSTD);
    vector<vector<double>> iterationPrevalence = {}; // store detection prevalence for each sample of this iteration

    // Loop over samples
    for (int sampleCounter = 1; sampleCounter <= sampleNumber; sampleCounter++){
        double randSickFraction = sickDist(sickRNG);
        if (randSickFraction < TOTAL_NUMBER[0]/totalPlants) {
            SICK_PLANTS = {1, 0};
        } else {
            SICK_PLANTS = {0, 1};
        }
        if (sampleCounter % 100 == 0) {
            cout<<"Sample = "<<sampleCounter<<endl;
        }
        vector<double> sampleDetectionPrevalence = simulate(argc, argv, BETAS, EPSILONS, GAMMAS, 
            TOTAL_NUMBER, SICK_PLANTS, SAMPLE_SIZE, DELTA, baseline);
        iterationPrevalence.push_back(sampleDetectionPrevalence);
    }

    if (TOTAL_NUMBER[1] == 0){
        MyFileMean << "EDP Mean" << endl;
        MyFileSTD << "EDP STD" << endl;
        double EDPmean = findMean(iterationPrevalence, 0);
        double EDPstd = findSTD(iterationPrevalence, 0, EDPmean);
        MyFileMean << EDPmean << endl;
        MyFileSTD << EDPstd << endl;
    } else { 
        MyFileMean << "Number of Sentinels, EDP Mean" << endl;
        MyFileSTD << "Number of Sentinels, EDP STD" << endl;
        for (int iteration = 0; iteration <= SAMPLE_SIZE; iteration++){
            double EDPmean = findMean(iterationPrevalence, iteration);
            double EDPstd = findSTD(iterationPrevalence, iteration, EDPmean);
            MyFileMean << iteration << "," << EDPmean << endl;
            MyFileSTD << iteration << "," << EDPstd << endl;
        }
    }
       
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    cout<<"RUN TIME: "<<time_taken<<"s"<<endl;
}
