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
double RADIUS = 0.5;

// Vectors of form {crops, sentinels}
std::vector<double> BETAS = {5*pow(10,-5),5*pow(10,-5)}; // daily per capita infection rate
std::vector<double> EPSILONS = {0.015, 0.1}; //scaling parameters
std::vector<double> GAMMAS = {452, 49};

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {100, 100}; // total number of plants
std::vector<int> SICK_PLANTS = {5, 0}; //total number of sick plants

// Sample Parameters
int sampleCounter = 1; // Set sample counter = 1
int sampleNumber = 100; // total number of samples

// used for readability
enum sampleDataIndices {time_index = 0, cropsH_index = 1, cropsU_index = 2, cropsD_index = 3, 
                        sentinelsH_index = 4, sentinelsU_index = 5, sentinelsD_index = 6};

double findMean(std::vector<std::vector<int>> matrix, int column) {
    int numberRows = matrix.size();
    double sum = 0;
    for (std::vector<int> row:matrix) {
        sum += row[column];
    }
    return ((double) sum)/numberRows;
}

double findSTD(std::vector<std::vector<int>> matrix, int column, double mean) {
    int numberRows = matrix.size();
    double sum = 0;
    for (std::vector<int> row:matrix) {
        sum += pow((row[column]-mean),2);   
    }
    return sqrt(((double) sum)/(numberRows-1));
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

    std::vector<int> timeVector;
    std::vector<std::vector<int>> cropH_matrix;
    std::vector<std::vector<int>> cropU_matrix;
    std::vector<std::vector<int>> cropD_matrix;
    std::vector<std::vector<int>> sentinelH_matrix;
    std::vector<std::vector<int>> sentinelU_matrix;
    std::vector<std::vector<int>> sentinelD_matrix;
    // Loop over samples
    while (sampleCounter <= sampleNumber){
        std::cout<<"Sample = "<< sampleCounter <<std::endl;
        std::vector<std::vector<int>> sampleData = simulate(argc, argv, BETAS, EPSILONS, GAMMAS, 
                                                            TOTAL_NUMBER, SICK_PLANTS, MAX_TIME, 
                                                            RADIUS);
        std::vector<int> cropH_vector;
        std::vector<int> cropU_vector;
        std::vector<int> cropD_vector;     
        std::vector<int> sentinelH_vector;
        std::vector<int> sentinelU_vector;
        std::vector<int> sentinelD_vector;                         
        for (std::vector<int> snapshot : sampleData) {
            if (sampleCounter == 1){
                timeVector.push_back(snapshot[time_index]);
            }
            cropH_vector.push_back(snapshot[cropsH_index]);
            cropU_vector.push_back(snapshot[cropsU_index]);
            cropD_vector.push_back(snapshot[cropsD_index]);
            sentinelH_vector.push_back(snapshot[sentinelsH_index]);
            sentinelU_vector.push_back(snapshot[sentinelsU_index]);
            sentinelD_vector.push_back(snapshot[sentinelsD_index]);
        }
        cropH_matrix.push_back(cropH_vector);
        cropU_matrix.push_back(cropU_vector);
        cropD_matrix.push_back(cropD_vector);
        sentinelH_matrix.push_back(sentinelH_vector);
        sentinelU_matrix.push_back(sentinelU_vector);
        sentinelD_matrix.push_back(sentinelD_vector);
        sampleCounter += 1;
    }

    // save data
    std::ofstream fileCropH_mean(std::filesystem::path("cropH_mean.txt"));
    std::ofstream fileCropH_std(std::filesystem::path("cropH_std.txt"));
    std::ofstream fileCropU_mean(std::filesystem::path("cropU_mean.txt"));
    std::ofstream fileCropU_std(std::filesystem::path("cropU_std.txt"));
    std::ofstream fileCropD_mean(std::filesystem::path("cropD_mean.txt"));
    std::ofstream fileCropD_std(std::filesystem::path("cropD_std.txt"));
    std::ofstream fileSentinelH_mean(std::filesystem::path("sentinelH_mean.txt"));
    std::ofstream fileSentinelH_std(std::filesystem::path("sentinelH_std.txt"));
    std::ofstream fileSentinelU_mean(std::filesystem::path("sentinelU_mean.txt"));
    std::ofstream fileSentinelU_std(std::filesystem::path("sentinelU_std.txt"));
    std::ofstream fileSentinelD_mean(std::filesystem::path("sentinelD_mean.txt"));
    std::ofstream fileSentinelD_std(std::filesystem::path("sentinelD_std.txt"));

    fileCropH_mean << "# time, mean" << "\n";
    fileCropH_std << "# time, std" << "\n";
    fileCropU_mean << "# time, mean" << "\n";
    fileCropU_std << "# time, std" << "\n";
    fileCropD_mean << "# time, mean" << "\n";
    fileCropD_std << "# time, std" << "\n";
    fileSentinelH_mean << "# time, mean" << "\n";
    fileSentinelH_std << "# time, std" << "\n";
    fileSentinelU_mean << "# time, mean" << "\n";
    fileSentinelU_std << "# time, std" << "\n";
    fileSentinelD_mean << "# time, mean" << "\n";
    fileSentinelD_std << "# time, std" << "\n";

    for (int time = 0; time < (int) timeVector.size(); time++) {
        double cropH_mean = findMean(cropH_matrix, time);
        fileCropH_mean << time << "," << cropH_mean << "\n";
        double cropH_STD = findSTD(cropH_matrix, time, cropH_mean);
        fileCropH_std << time << "," << cropH_STD << "\n";
        double cropU_mean = findMean(cropU_matrix, time);
        fileCropU_mean << time << "," << cropU_mean << "\n";
        double cropU_STD = findSTD(cropU_matrix, time, cropU_mean);
        fileCropU_std << time << "," << cropU_STD << "\n";
        double cropD_mean = findMean(cropD_matrix, time);
        fileCropD_mean << time << "," << cropD_mean << "\n";
        double cropD_STD = findSTD(cropD_matrix, time, cropD_mean);
        fileCropD_std << time << "," << cropD_STD << "\n";
        double sentinelH_mean = findMean(sentinelH_matrix, time);
        fileSentinelH_mean << time << "," << sentinelH_mean << "\n";
        double sentinelH_STD = findSTD(sentinelH_matrix, time, sentinelH_mean);
        fileSentinelH_std << time << "," << sentinelH_STD << "\n";
        double sentinelU_mean = findMean(sentinelU_matrix, time);
        fileSentinelU_mean << time << "," << sentinelU_mean << "\n";
        double sentinelU_STD = findSTD(sentinelU_matrix, time, sentinelU_mean);
        fileSentinelU_std << time << "," << sentinelU_STD << "\n";
        double sentinelD_mean = findMean(sentinelD_matrix, time);
        fileSentinelD_mean << time << "," << sentinelD_mean << "\n";
        double sentinelD_STD = findSTD(sentinelD_matrix, time, sentinelD_mean);
        fileSentinelD_std << time << "," << sentinelD_STD << "\n";
    }
    fileCropH_mean.close();
    fileCropH_std.close();
    fileCropU_mean.close();
    fileCropU_std.close();
    fileCropD_mean.close();
    fileCropD_std.close();
    fileSentinelH_mean.close();
    fileSentinelH_std.close();
    fileSentinelU_mean.close();
    fileSentinelU_std.close();
    fileSentinelD_mean.close();
    fileSentinelD_std.close();

    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
