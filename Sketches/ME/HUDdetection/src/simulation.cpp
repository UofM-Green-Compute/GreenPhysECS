#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include "markovSystems.h"

// if this bool is set to true then simulation will stop


// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct Index { int plantNumber; }; // what is your index within your vector
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2, 3

// Tools for picking random numbers 
std::mt19937 rng(std::random_device{}()) ; 
std::uniform_real_distribution<double> dist(0,1); 

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected, int plantType){
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({i})
                .set<MarkovState>({1})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
        ); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        std::vector<double> Qnm = {0, 0, 0}; 
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

void setupSystems(flecs::world &world, std::vector<int> &cropPopulation, std::vector<int> &sentinelPopulation,
    const std::vector<double> &infectionRates, std::vector<double> &scalings, std::vector<double> &presymptomaticTimes) {
    updateProbabilities(world, cropPopulation, sentinelPopulation, infectionRates, scalings, presymptomaticTimes);
    transition(world, cropPopulation, sentinelPopulation);
}

void selectIndices(std::vector<int> &shuffledIndices, std::vector<int> &detectionIndices, int sampleSize) {
    /*
    This function shuffles shuffledIndices, and populates detectionIndices with first sampleSize indices
    */
    auto rngShuffle = std::default_random_engine {};
    std::shuffle(std::begin(shuffledIndices), std::end(shuffledIndices), rngShuffle);
    for(unsigned int i = 0; i < (unsigned) sampleSize; i++) {
        detectionIndices.push_back(shuffledIndices[i]);
    }
}

void detect(std::vector<flecs::entity> plants, std::vector<int> &detectionIndices, bool &outbreakDetection) {
    /*
    This functions loops through the detection indices to check if a plant in the sample is 
    infected
    */
    //std::cout<<"bonjour\n";
    for (int index : detectionIndices) {
        const MarkovState &state = plants[index].get<MarkovState>();
        if (state.s == 3) {// if the plant is detectable 
            outbreakDetection = true;
            break;           
        }
    }
}

int simulate(int argc, char* argv[], const std::vector<double> betas, std::vector<double> epsilons, 
    std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int delta, const std::vector<int> sampleSizes, const int maxTime, std::string filename1, 
    std::string filename2) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */
    bool outbreakDetection = false;
    std::vector<int> cropShuffledIndices; // This vector contains indices of crops which will be checked
    std::vector<int> sentinelShuffledIndices; // This vector contains indices of crops which will be checked
    std::vector<int> cropDetectionIndices; // This vector contains indices of crops which will be checked
    std::vector<int> sentinelDetectionIndices; // This vector contains indices of crops which will be checked
    // fill the shuffled indices vector with unshuffled indices (they will be shuffled later)
    for (int integer = 0; integer < totalPopulations[0]; integer++) {
        cropShuffledIndices.push_back(integer);
    }
    for (int integer = 0; integer < totalPopulations[1]; integer++) {
        sentinelShuffledIndices.push_back(integer);
    }

    // Population vector will be updated and saved over time
    std::vector<int> cropsPopulationVector = {totalPopulations[0]-U0[0], U0[0], 0};
    std::vector<int> sentinelsPopulationVector = {totalPopulations[1]-U0[1], U0[1], 0};
    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    setupComponents(world);

    // Create a crop and sentinels vector and generate entities
    std::vector<flecs::entity> crops;
    std::vector<flecs::entity> sentinels; 
    crops.reserve(totalPopulations[0]);
    sentinels.reserve(totalPopulations[1]);
    setupEntities(world, crops, totalPopulations[0], U0[0], 0);
    setupEntities(world, sentinels, totalPopulations[1], U0[1], 1); 
    // Create the systems
    setupSystems(world, cropsPopulationVector, sentinelsPopulationVector, betas, epsilons, gammas);

    // Run the simulation
    std::ofstream MyFile1; // crops
    std::ofstream MyFile2; // sentinels
    MyFile1.open(filename1);
    MyFile2.open(filename2);
    MyFile1 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    MyFile2 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    MyFile1 << 0 << "," << cropsPopulationVector[0] << "," << cropsPopulationVector[1] << ","
            << cropsPopulationVector[2] << std::endl;  
    MyFile2 << 0 << "," << sentinelsPopulationVector[0] << "," << sentinelsPopulationVector[1] << ","
            << sentinelsPopulationVector[2] << std::endl;
    for (int time = 1;time<=maxTime;time++) {
        // infection spread
        world.progress();

        // save infection to data to file
        MyFile1 << time << "," << cropsPopulationVector[0] << "," << cropsPopulationVector[1] << ","
            << cropsPopulationVector[2] << std::endl;  
        MyFile2 << time << "," << sentinelsPopulationVector[0] << "," << sentinelsPopulationVector[1] << ","
                << sentinelsPopulationVector[2] << std::endl;

        // detection
        std::cout<<time<<"\n";
        selectIndices(cropShuffledIndices, cropDetectionIndices, sampleSizes[0]);
        selectIndices(sentinelShuffledIndices, sentinelDetectionIndices, sampleSizes[1]);
        if (time%delta == 0){
            detect(sentinels, sentinelDetectionIndices, outbreakDetection);
            if (outbreakDetection == false){
                detect(crops, cropDetectionIndices, outbreakDetection);
            }
        }
        if (outbreakDetection == true) {
            std::cout<<"ahghhgh theres an outbreak!"<<"\n";
            break;
        } 
    }
    MyFile1.close();
    MyFile2.close();
    return 0;
}
