#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <filesystem>
#include "markovSystems.h"
#include "surveillance.h"

using namespace std;

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct Index { int plantNumber; }; // what is your index within your vector
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { vector<double> qnm; }; // Transition from state m to state n = 1,2, 3

// Tools for picking random numbers 
mt19937 rng(random_device{}()) ; 
uniform_real_distribution<double> dist(0,1); 

void setupEntities(flecs::world world, vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected, int plantType){
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({i})
                .set<MarkovState>({1})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
        ); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({totalPopulation-initialInfected + i})
                .set<MarkovState>({2})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
        );
    }
}

void setupComponents(flecs::world world){
    world.component<Index>();
    world.component<MarkovState>();
    world.component<PlantState>();
    world.component<TransitionProbabilities>();
}

void setupSystems(flecs::world &world, vector<int> &cropPopulation, vector<int> &sentinelPopulation,
    const vector<double> &infectionRates, vector<double> &scalings, vector<double> &presymptomaticTimes) {
    updateProbabilities(world, cropPopulation, sentinelPopulation, infectionRates, scalings, presymptomaticTimes);
    transition(world, cropPopulation, sentinelPopulation);
}

vector<double> simulate(int argc, char* argv[], const vector<double> betas, vector<double> epsilons, 
    vector<double> gammas, vector<int> totalPopulations, vector<int> U0, 
    vector<bool> detectionChecker, int sampleSize, int delta,
    filesystem::path filePath1, filesystem::path filePath2) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */

    // For each sampling strategy detection this goes up by one. If it equals the sampling size then
    // that means all strategies have detected the disease
    int detectionCounter = 0;

    // Fills the prevalence vector with zeroes so that it is the right size for indexing
    vector<double> sampleDetectionPrevalence;
    for (int detectionStrategy = 0; detectionStrategy<=sampleSize; detectionStrategy++){
        sampleDetectionPrevalence.push_back(0);
    }

    // Population vector will be updated and saved over time
    vector<int> cropsPopulationVector = {totalPopulations[0]-U0[0], U0[0], 0};
    vector<int> sentinelsPopulationVector = {totalPopulations[1]-U0[1], U0[1], 0};
    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    setupComponents(world);

    // Create a crop and sentinels vector and generate entities
    vector<flecs::entity> crops;
    vector<flecs::entity> sentinels; 
    crops.reserve(totalPopulations[0]);
    sentinels.reserve(totalPopulations[1]);
    setupEntities(world, crops, totalPopulations[0], U0[0], 0);
    setupEntities(world, sentinels, totalPopulations[1], U0[1], 1); 
    // Create the systems
    setupSystems(world, cropsPopulationVector, sentinelsPopulationVector, betas, epsilons, gammas);

    // Run the simulation
    ofstream MyFile1(filePath1); // crops
    ofstream MyFile2(filePath2); // sentinels

    // setup headers
    // crop header
    MyFile1 << "Time"; 
    for (int i=0; i< (int) crops.size(); i++) {
        char cropHeader[50]; 
        sprintf(cropHeader, "Crop%d",i); 
        MyFile1 << ","<<cropHeader;
    }
    MyFile1 << endl;
    // sentinel header
    MyFile2 << "Time"; 
    for (int i=0; i< (int) crops.size(); i++) {
        char sentinelHeader[50]; 
        sprintf(sentinelHeader, "Sentinel%d",i); 
        MyFile2 << ","<<sentinelHeader;
    }
    MyFile2 << endl;

    // initial conditions
    // crop initial conditions
    MyFile1 << 0; 
    for (int i=0; i< (int) crops.size(); i++) {
        const MarkovState &initialCropState = crops[i].get<MarkovState>();
        MyFile1 << ","<<initialCropState.s;
    }
    MyFile1 << endl;
    // sentinel initial conditions
    MyFile2 << 0; 
    for (int i=0; i< (int) sentinels.size(); i++) {
        const MarkovState &initialSentinelsState = crops[i].get<MarkovState>();
        MyFile2 << ","<<initialSentinelsState.s;
    }
    MyFile2 << endl;
    int time = 0;
    while (detectionCounter<sampleSize) {
        // infection spread
        world.progress();
        time +=1;
        // save infection to data to file
        MyFile1 << time; 
        MyFile2 << time;
        for (int i=0; i< (int) crops.size(); i++) {
            const MarkovState &cropState = crops[i].get<MarkovState>();
            MyFile1 << ","<<cropState.s;
        }
        MyFile1 << endl;
        // sentinel initial conditions
        MyFile2 << 0; 
        for (int i=0; i< (int) sentinels.size(); i++) {
            const MarkovState &sentinelsState = crops[i].get<MarkovState>();
            MyFile2 << ","<<sentinelsState.s;
        }
        MyFile2 << endl;

        // If t an integer numebr of delta
        // Performs all surveillance strategies that have not already detected the disease
        // If disease is detected then sampleDetectionPrevalence, detectionChecker and 
        // detectionCounter are updated
        if (time%delta==0){
            for (int detectionStrategy = 0; detectionStrategy<=sampleSize; detectionStrategy++){
                bool individualChecker = detectionChecker[detectionStrategy];
                if (individualChecker==false){
                    double prevalence = surveille(individualChecker, totalPopulations, 
                                                  detectionStrategy, sampleSize, time, filePath1, 
                                                  filePath2);
                    if (individualChecker==true) {
                        detectionChecker[detectionStrategy] = true;
                        detectionCounter += 1;
                        sampleDetectionPrevalence[detectionStrategy] = prevalence;
                    }
                }
            } 
        }
    }
    MyFile1.close();
    MyFile2.close();
    return sampleDetectionPrevalence;
}
