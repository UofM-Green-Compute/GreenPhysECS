#include <iostream>
#include <fstream>
#include <time.h> 
#include <sstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <filesystem>
#include "simulation.h"
#include "surveillance.h"

using namespace std;

// Vectors of form {crops, sentinels}
vector<double> BETAS = {5*pow(10,-5),5*pow(10, -5)}; // daily per capita infection rate
vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
vector<double> GAMMAS = {452, 49};

// Initial Conditions
vector<int> TOTAL_NUMBER = {1000, 50}; // total number of plants
vector<int> SICK_PLANTS = {1, 0}; //total number of sick plants

// Detection Parameters
int DELTA = 30;
int SAMPLE_SIZE = 50;
vector<bool> DETECTION_CHECKER; // each index says if that strategy has detected the disease


// Sample Parameters
int sampleNumber = 5; // total number of samples

double findMean(vector<vector<double>> matrix, int column) {
    int numberRows = matrix.size();
    double sum = 0;
    for (vector<double> row:matrix) {
        if (row[column]==0){
            numberRows -= 1; // from surveillance.cpp this means ignore the sample
        } else {
            sum += row[column];
        }
    }
    return sum/numberRows;
}

double findSTD(vector<vector<double>> matrix, int column, double mean) {
    int numberRows = matrix.size();
    double sum = 0;
    for (vector<double> row:matrix) {
        if (row[column]==0){
            numberRows -= 1; // from surveillance.cpp this means ignore the sample
        } else {
            sum += pow((row[column]-mean),2);
        }    
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

    // fills the detectionChecker vector with falses as no disease detected yet
    for (int sentinelStrategy = 0; sentinelStrategy <= SAMPLE_SIZE; sentinelStrategy++){
        DETECTION_CHECKER.push_back(false);
    }

    // Start measuring run time of program
    clock_t tClock; 
    tClock = clock(); 

    // creates directory based on probelm setup
    char DIRECTORYNAMESETUP[50]; 
    sprintf(DIRECTORYNAMESETUP, "Pcrops%d_Psentinels%d", 
        TOTAL_NUMBER[0], TOTAL_NUMBER[1]);   
    filesystem::path setupPath = DIRECTORYNAMESETUP;
    filesystem::create_directory(setupPath);

        // save EDP data
    filesystem::path FilePathMean = setupPath / "EDPmean.txt";
    filesystem::path FilePathSTD = setupPath / "EDPstd.txt";
    ofstream MyFileMean(FilePathMean);
    ofstream MyFileSTD(FilePathSTD);
    MyFileMean << "Number of Sentinels, EDP Mean" << endl;
    MyFileSTD << "Number of Sentinels, EDP STD" << endl;
    vector<vector<double>> iterationPrevalence = {}; // store detection prevalence for each sample of this iteration

    // Loop over samples
    for (int sampleCounter = 1; sampleCounter <= sampleNumber; sampleCounter++){
        cout<<"Sample = "<<sampleCounter<<endl;
        char FILENAME1[50]; 
        char FILENAME2[50]; 
        sprintf(FILENAME1, "HUDcrops_%d.txt", sampleCounter);
        sprintf(FILENAME2, "HUDsentinels_%d.txt", sampleCounter);
        filesystem::path FILEPATH1 = setupPath / FILENAME1;
        filesystem::path FILEPATH2 = setupPath / FILENAME2;
        vector<double> sampleDetectionPrevalence = simulate(argc, argv, BETAS, EPSILONS, GAMMAS, 
            TOTAL_NUMBER, SICK_PLANTS, DETECTION_CHECKER, SAMPLE_SIZE, DELTA, FILEPATH1, FILEPATH2);
        for (int iteration = 0; iteration <= SAMPLE_SIZE; iteration++){
            cout<<sampleDetectionPrevalence[iteration]<<endl;
        }
        iterationPrevalence.push_back(sampleDetectionPrevalence);
    }

    for (int iteration = 0; iteration <= SAMPLE_SIZE; iteration++){

        double EDPmean = findMean(iterationPrevalence, iteration);
        double EDPstd = findSTD(iterationPrevalence, iteration, EDPmean);
        MyFileMean << (SAMPLE_SIZE-iteration) << "," << EDPmean << endl;
        MyFileSTD << (SAMPLE_SIZE-iteration) << "," << EDPstd << endl;
    }
        
    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    cout<<"RUN TIME: "<<time_taken<<"s"<<endl;
}
