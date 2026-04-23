#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include "markovSystems.h"

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct Index { int plantNumber; }; // what is your index within your vector
struct InfectedSentinelConnections { int u,d; }; // No. of infected sentinels an individual is connected to. u:undetectable, d:detectable
struct InfectedCropConnections { int u,d; }; // No. of infected crops an individual is connected to. u:undetectable, d:detectable
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2, 3
struct Position { double x, y; }; // random position inside a box (between 0 and 1)

// Tools for picking random numbers 
std::mt19937 rng(std::random_device{}()) ; 
std::uniform_real_distribution<double> dist(0,1); 

enum states {healthy = 1, undetectable = 2, detectable = 3};

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected, int plantType){
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        // x and y are the random positions of the crops
        double x = dist(rng); 
        double y = dist(rng);
        std::vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({i})
                .set<MarkovState>({1})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
                .set<Position>({x, y})
                .set<InfectedSentinelConnections>({0,0})
                .set<InfectedCropConnections>({0,0})
        ); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        // x and y are the random positions of the crops
        double x = dist(rng); 
        double y = dist(rng); 
        std::vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({totalPopulation-initialInfected + i})
                .set<MarkovState>({2})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
                .set<Position>({x, y})
                .set<InfectedSentinelConnections>({0,0})
                .set<InfectedCropConnections>({0,0})
        );
    }
}

void setupComponents(flecs::world world){
    world.component<Index>();
    world.component<InfectedSentinelConnections>();
    world.component<InfectedCropConnections>();
    world.component<MarkovState>();
    world.component<PlantState>();
    world.component<TransitionProbabilities>();
    world.component<Position>();
}

void setupSystems(flecs::world &world, std::vector<flecs::entity> &crops, std::vector<flecs::entity> &sentinels,
                  flecs::entity &hLink, flecs::entity &uLink, flecs::entity &dLink, 
                  const std::vector<double> infectionRates, const std::vector<double> scalings, 
                  const std::vector<double> &presymptomaticTimes, std::vector<int> &cropPopulation, 
                  std::vector<int> &sentinelPopulation) {
    updateInfectedNumbers(world, sentinels, crops, uLink, dLink);
    updateProbabilities(world, infectionRates, scalings, presymptomaticTimes);
    transition(world, cropPopulation, sentinelPopulation);
    updateGraph(world, crops, sentinels, hLink, uLink, dLink); 
}

double findPlantDistance (flecs::entity plant1, flecs::entity plant2) {
    double deltaX;
    double deltaY;
    double distance;
    deltaX = plant2.get<Position>().x - plant1.get<Position>().x;
    deltaY = plant2.get<Position>().y - plant1.get<Position>().y;
    distance = std::sqrt(deltaX*deltaX + deltaY*deltaY);
    return distance;
}

void addLink(flecs::entity plant1, flecs::entity plant2, flecs::entity hLink, flecs::entity uLink,
             flecs::entity dLink, double radius){
    // used to check if inside radius
    double distance = findPlantDistance(plant1, plant2);
    if (distance>radius) {
        // if distance > radius, don't make a connection
        return;
    }
    // if either plant is detectable, add detectable link
    if (plant1.get<MarkovState>().s == detectable || plant2.get<MarkovState>().s == detectable) {
        plant1.add(dLink, plant2);
        plant2.add(dLink, plant1);
    }
    else if (plant1.get<MarkovState>().s == undetectable || plant2.get<MarkovState>().s == undetectable) {
        plant1.add(uLink, plant2);
        plant2.add(uLink, plant1);
    }
    else {
        plant1.add(hLink, plant2);
        plant2.add(hLink, plant1);
    }
}

void setupGraph(std::vector<flecs::entity> &sentinelPopulation, std::vector<flecs::entity> &cropPopulation,
                flecs::entity hLink, flecs::entity uLink, flecs::entity dLink, int noSentinels, 
                int noCrops, double radius){
    // start with all the sentinel connections (no need to check the final sentinel as all links will have been made)
    for(int i = 0; i < noSentinels-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noSentinels; k++){
            addLink(sentinelPopulation[i], sentinelPopulation[k], hLink, uLink, dLink, radius);
        }
        // compare with all crops
        for (int k = 0; k < noCrops; k++){
            addLink(sentinelPopulation[i], cropPopulation[k], hLink, uLink, dLink, radius);
        }
    }
    // do the same with crops. (no need to compare with sentinels as that was done previously)
    for(int i = 0; i < noCrops-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noCrops; k++){
            addLink(cropPopulation[i], cropPopulation[k], hLink, uLink, dLink, radius);
        }
    }
}

int simulate(int argc, char* argv[], const std::vector<double> betas, const std::vector<double> epsilons, 
    const std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int maxTime, std::string filename1, std::string filename2, double radius) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */

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
    // Relationship types 
    flecs::entity healthyLink = world.entity(); // both plants are healthy
    flecs::entity undetectableLink = world.entity(); // at least one plant is undetectable
    flecs::entity detectableLink = world.entity(); // at least one plant is detectable
    setupGraph(sentinels, crops, healthyLink, undetectableLink, detectableLink, totalPopulations[1], 
               totalPopulations[0], radius);
    // Create the systems
    setupSystems(world, crops, sentinels, healthyLink, undetectableLink, detectableLink,
                 betas, epsilons, gammas, cropsPopulationVector, sentinelsPopulationVector);

    // Run the simulation
    std::ofstream MyFile1; // crops
    std::ofstream MyFile2; // sentinels
    MyFile1.open(filename1);
    MyFile2.open(filename2);
    MyFile1 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    MyFile2 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    double time = 0;
    MyFile1 << time << "," << cropsPopulationVector[0] << "," << cropsPopulationVector[1] << ","
            << cropsPopulationVector[2] << std::endl;  
    MyFile2 << time << "," << sentinelsPopulationVector[0] << "," << sentinelsPopulationVector[1] << ","
            << sentinelsPopulationVector[2] << std::endl;  
    
    while (time<maxTime) {
        world.progress();
        time += 1;
        std::cout<<time<<std::endl;    
        // update sample variables
        std::cout << time << "," << cropsPopulationVector[0] << "," << cropsPopulationVector[1] << ","
            << cropsPopulationVector[2] << std::endl; 
        MyFile1 << time << "," << cropsPopulationVector[0] << "," << cropsPopulationVector[1] << ","
            << cropsPopulationVector[2] << std::endl;  
        MyFile2 << time << "," << sentinelsPopulationVector[0] << "," << sentinelsPopulationVector[1] << ","
                << sentinelsPopulationVector[2] << std::endl;  
    }
    MyFile1.close();
    MyFile2.close();
    return 0;
}
