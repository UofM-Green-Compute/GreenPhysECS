/*
Oluwole Delano
12/2/26
Mattias Evans
26/02/26
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include <numeric>
#include <algorithm>

int N = 100; // Total number of people
double timeStep = 0.005; // Set timestep = 0.0001
double maxTime = 5; // Maximum Simulation Time

int sampleCounter = 1; // Set sample counter = 1
int sampleNumber = 100; // total number of samples
// initial populations (n1, n2, n3)
int n1 = N-1; // State 1 (susceptible)
int n2 = 1; // State 2 (infected)
int n3 = 0; // State 3 (recovered)

// Transition constants
double beta = 7 / static_cast<double>(N); 
double alpha = 1; 

// Initialise State components
// State tells you if your in S, I, or R
struct State { int s; };
// Marko Chain Probabilities. q1 sends you to S, q2 to I, and q3 to R
struct MarkovProbabilities { double q1, q2, q3; }; 

void runModel(int argc, char* argv[], std::vector<int> &population1, 
    std::vector<int> &population2, std::vector<int> &population3, std::vector<double> &timeArray) {   
    double t = 0; // initialize time = 0
    timeArray = {0};
    std::vector<int> population = {n1, n2, n3}; // nS, nI, NR
    // Initial Transition Probabilties
    // Transition probabilities when in S
    double Q11 = exp(-beta * population[1] * timeStep);
    double Q21 = 1 - Q11;
    double Q31 = 0.0;
    // Transition probabilities when in I
    double Q12 = 0.0;
    double Q22 = exp(-alpha*timeStep);
    double Q32 = 1 - Q22;
    // Transition probabilities when in R
    double Q13 = 0;
    double Q23 = 0;
    double Q33 = 1;
    population1 = {n1};
    population2 = {n2};
    population3 = {n3};

    // Initialise flecs world
    flecs::world world(argc,argv);
    //world.set_threads(1); 

    // Creating components
    world.component<State>();
    world.component<MarkovProbabilities>(); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(N); 

    // Initialise Susceptible Entities 
    for (int i = 0; i < n1; ++i) { 
        people.push_back( 
            world.entity() 
                .set<State>({1})
                .set<MarkovProbabilities>({0, 0, 0})
        ); 
    }
    // Initialise the Infected Entity
    for (int i = 0; i < n2; ++i) { 
        people.push_back( 
            world.entity() 
                .set<State>({2})
                .set<MarkovProbabilities>({0, 0, 0})
        );
    }  

    // Update Entity Transition Probabilities
    world.system<State, MarkovProbabilities>()
        //.multi_threaded()
        .each([&](State& state, MarkovProbabilities&prob){
            if (state.s == 1){
                prob.q1 = Q11;
                prob.q2 = Q21;
                prob.q3 = Q31;
            } else if (state.s == 2) {
                prob.q1 = Q12;
                prob.q2 = Q22;
                prob.q3 = Q32;
            } else if (state.s == 3) {
                prob.q1 = Q13;
                prob.q2 = Q23;
                prob.q3 = Q33;
            }
        });
    
    // Update State
    world.system<State, MarkovProbabilities>()
        //.multi_threaded()
        .each([&](State& state, MarkovProbabilities&prob){
            double rand = dist(rng);
            if(rand < prob.q1){
                // transition to state 1 (susceptible) 
                if (state.s == 2) {
                    population[0] += 1;
                    population[1] -= 1;  
                } else if (state.s == 3) {
                    population[0] += 1;
                    population[2] -= 1;  
                }
                state.s = 1;
            } else if( (rand > prob.q2) && (rand < (prob.q1 + prob.q2))){
                // transition to state 2 (infected)
                if (state.s == 1) {
                    population[1] += 1;
                    population[0] -= 1;  
                } else if (state.s == 3) {
                    population[1] += 1;
                    population[2] -= 1;  
                }
                state.s = 2;
            } else if((rand > (prob.q1 + prob.q2)) && (rand < (prob.q1 + prob.q2 + prob.q3))){
                // transition to state 3 (recovered)
                if (state.s == 1) {
                    population[2] += 1;
                    population[0] -= 1;  
                } else if (state.s == 2) {
                    population[2] += 1;
                    population[1] -= 1;  
                }
                state.s = 3;
            }
        });

    while (t+timeStep<maxTime) {
        world.progress();
        t += timeStep;
        timeArray.push_back(t);
        // Update Transition Probabilties
        // Transition probabilities when in S
        Q11 = exp(-beta * population[1] * timeStep);
        Q21 = 1 - Q11;
        Q31 = 0.0;
        // Transition probabilities when in I
        Q12 = 0.0;
        Q22 = exp(-alpha*timeStep);
        Q32 = 1 - Q22;
        // Transition probabilities when in R
        Q13 = 0;
        Q23 = 0;
        Q33 = 1;
        population1.push_back(population[0]);
        population2.push_back(population[1]);
        population3.push_back(population[2]);  
    }
}

double findMean(std::vector<int> intArray) {
    double intArraySum = std::accumulate(intArray.begin(), intArray.end(), 0.0);
    double intArrayMean = static_cast<double>(intArraySum) / intArray.size();
    return intArrayMean;
}

double findSTD(std::vector<int> intArray, double mean) {
    double varianceSum = 0.0;
    for (int value : intArray) {
        varianceSum += std::pow(static_cast<double>(value) - mean, 2);
    }
    double variance = static_cast<double>(varianceSum) / intArray.size();
    double standardDeviation = std::sqrt(variance);
    return standardDeviation;
}

int main(int argc, char* argv[]) {
    std::ofstream MyFileSample;
    MyFileSample.open("sample.txt");
    std::ofstream MyFile1;
    MyFile1.open("state1.txt"); 
    std::ofstream MyFile2;
    MyFile2.open("state2.txt");
    std::ofstream MyFile3;
    MyFile3.open("state3.txt");

    MyFileSample << "time,susceptible,infected,recovered" << std::endl; // Set column labels
    MyFile1 << "time,state1Mean,state1lower,state1upper" << std::endl; // Set column labels
    MyFile2 << "time,state2Mean,state2lower,state2upper" << std::endl; // Set column labels
    MyFile3 << "time,state3Mean,state3lower,state3upper" << std::endl; // Set column labels
    // These vectors are population versus time for a single individual sample
    std::vector<double> individualTimes;
    std::vector<int> individualPopulation1; // Susceptible
    std::vector<int> individualPopulation2; // Infected
    std::vector<int> individualPopulation3; // Recovered

    double timeSamples;
    // this is a vector of individualPopulation vectors for each individual sample
    std::vector<std::vector<int>> populationSamples1; // Susceptible
    std::vector<std::vector<int>> populationSamples2; // Infected
    std::vector<std::vector<int>> populationSamples3; // Recovered

    // Run the simulation 10 times
    clock_t tClock;
    tClock = clock();  
    while (sampleCounter <= sampleNumber){
        std::cout<<"Sample = "<< sampleCounter <<std::endl;
        runModel(argc,argv, individualPopulation1, individualPopulation2, individualPopulation3, 
            individualTimes);
        // Record the first sample as an individual case
        if (sampleCounter == 1){
            for(int i = 0; i < (int) individualPopulation1.size(); i++) {
                MyFileSample << i*timeStep << "," << individualPopulation1[i] << "," << 
                individualPopulation2[i] << "," << individualPopulation3[i] << std::endl; 
            }
        }
        MyFileSample.close();
        sampleCounter += 1; // increase sample counter
        populationSamples1.push_back(individualPopulation1); // record susceptible population vector
        populationSamples2.push_back(individualPopulation2); // record infected population vector
        populationSamples3.push_back(individualPopulation3); // record recovered population vector   
    }
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;

    // declare mean and standard deviation of variables as a function of time
    std::vector<double> population1Mean;
    std::vector<double> population1STD;
    std::vector<double> population2Mean;
    std::vector<double> population2STD;
    std::vector<double> population3Mean;
    std::vector<double> population3STD;

    int vectorTimeLength = populationSamples1[0].size(); // how many time entries are there
    std::vector<int> values1; //values which will be used to compute standard deviation and mean of susceptible
    std::vector<int> values2; //values which will be used to compute standard deviation and mean of infected
    std::vector<int> values3; //values which will be used to compute standard deviation and mean of recovered
    for (int i=0; i<vectorTimeLength; i++) {
        for (int j = 0; j<sampleNumber; j++) {   
            values1.push_back(populationSamples1[j][i]);
            values2.push_back(populationSamples2[j][i]);
            values3.push_back(populationSamples3[j][i]); 
        }
        double mean1 = findMean(values1); // mean number of susceptible people
        population1Mean.push_back(mean1);
        double std1 = findSTD(values1, mean1); // standard deviation of susceptible people
        population1STD.push_back(std1);
        double mean2 = findMean(values2); // mean number of infected people
        population2Mean.push_back(mean1);
        double std2 = findSTD(values2, mean2); // standard deviation of infected people
        population2STD.push_back(std2);
        double mean3 = findMean(values3); // mean number of recovered people
        population3Mean.push_back(mean1);
        double std3 = findSTD(values3, mean3); // standard deviation of recovered people
        population3STD.push_back(std3);
        timeSamples = i*timeStep;

        // write data to txt file
        MyFile1 << timeSamples << "," << mean1 << "," << mean1-std1 << "," << mean1+std1 << std::endl; 
        MyFile2 << timeSamples << "," << mean2 << "," << mean2-std2 << "," << mean2+std2 << std::endl;  
        MyFile3 << timeSamples << "," << mean3 << "," << mean3-std3 << "," << mean3+std3 << std::endl;  

        // reset values vector
        values1.clear();
        values2.clear();
        values3.clear();
    }
    MyFile1.close();
    MyFile2.close();
    MyFile3.close();
}