#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <algorithm>
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
                  flecs::entity &networkLink, const std::vector<double> infectionRates, 
                  const std::vector<double> scalings, const std::vector<double> &presymptomaticTimes, 
                  std::vector<int> &cropPopulation, std::vector<int> &sentinelPopulation) {
    countNeighbours(world, crops, sentinels, networkLink); 
    updateProbabilities(world, infectionRates, scalings, presymptomaticTimes);
    transition(world, cropPopulation, sentinelPopulation);
    
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

void addLink(flecs::entity &plant1, flecs::entity &plant2, flecs::entity &networkLink, 
             double &radius){
    // used to check if inside radius
    double distance = findPlantDistance(plant1, plant2);
    if (distance<radius) {
        // if distance < radius, add a link between both plants
        plant1.add(networkLink, plant2);
        plant2.add(networkLink, plant1); 
    }
    return;
}

void setupGraph(std::vector<flecs::entity> &sentinelPopulation, std::vector<flecs::entity> &cropPopulation,
                flecs::entity &networkLink, int noSentinels, 
                int noCrops, double radius){
    // start with all the sentinel connections (no need to check the final sentinel as all links will have been made)
    for(int i = 0; i < noSentinels-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noSentinels; k++){
            addLink(sentinelPopulation[i], sentinelPopulation[k], networkLink, radius);
        }
        // compare with all crops
        for (int k = 0; k < noCrops; k++){
            addLink(sentinelPopulation[i], cropPopulation[k], networkLink, radius);
        }
    }
    // do the same with crops. (no need to compare with sentinels as that was done previously)
    for(int i = 0; i < noCrops-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noCrops; k++){
            addLink(cropPopulation[i], cropPopulation[k], networkLink, radius);
        }
    }
}

void findFirstDetection(std::vector<int> &indices, int &maxSample, 
                    const std::vector<flecs::entity> &plantVector, int &firstPlant) {
    /*
    This function return the index of the first plant in the sample
    to detect an infection. this means the number of plants of that 
    type in a sample is firstPlant + 1. Because all strategies look
    at the same plant as their nth plant, all strategies which check
    firstPlant + 1 or more of this plant type will detect the outbreak.
    
    If, however, this function returns firstPlant = -1, then that means
    there was no detection by any strategy.
    */
    for (int i = 0; i < maxSample; i++) { // check however many plants are needed to cover all strategies
        int index = indices[i]; // this tells you which index plant in the plantVector is the i'th plant to check
        const MarkovState &plantState = plantVector[index].get<MarkovState>(); // check the plants state
        if (plantState.s == detectable) {
            firstPlant = i; // if the i'th plant is in the detectable category then all strategies that check i+1 or more plants will detect the disease 
            break;
        }
    }
}

void baselineSurveillance(const std::vector<flecs::entity> &cropVector, std::vector<int> &cropIndices, 
                          std::vector<double> &prevalenceVector, const std::vector<int> &cropsPopulationVector, 
                          const std::vector<int> &totalPopulations, int &maxCropSample, bool &baselineChecker) {
    /*
    This function looks for an infected crop (there are no sentinels as this is the
    baseline case). If there is an infected crop in the surveillance sample,
    it calculates the disease prevalence and tells the while loop to stop running
    by setting baselineChecker = true
    */
    // for information on what firstCrop does, see findFirstDetection
    int firstCrop = -1;
    findFirstDetection(cropIndices, maxCropSample, cropVector, firstCrop);
    // if firstCrop == -1 then there was no detection so don't calculate prevalence
    if (firstCrop == -1) {
        return;
    }
    // if we get here there has been at least one detection
    prevalenceVector[0] = ((double) cropsPopulationVector[1] + (double) cropsPopulationVector[2]) / 
                          (totalPopulations[0]); // the prevalence is the total number of infected crops divide by the total number of crops
    // in the baseline case there is only a single boolean that needs to be made true
    baselineChecker = true;
}

void strategySurveillance(const std::vector<flecs::entity> &sentinelVector,
                          const std::vector<flecs::entity> &cropVector,
                          std::vector<int> &sentinelIndices, std::vector<int> &cropIndices, 
                          std::vector<double> &prevalenceVector, int totalSample, 
                          const std::vector<int> &cropsPopulationVector, 
                          const std::vector<int> &totalPopulations, int &maxSentinelSample,
                          int &maxCropSample, std::vector<bool> &strategyChecker, 
                          int &strategyCounter) {
    /*
    This function looks for infected crops and sentinels. If there are infected crops and/or
    sentinels in a surveillance sample, all samples with that many crops/sentinels or more will
    detect the infection because I do not shuffle between surveillance samples, only between time steps.
    This is because it saves computational time, and any shuffle is valid as long as it changes in time.

    Having detected an outbreak and measured the prevalence, the strategyChecker[s] will become true and 
    the strategy counter will increase for s = number of sentinels in surveillance strategy
    */
    double prevalence;
    int firstSentinel = -1;
    int firstCrop = -1;
    // for information on what firstCrop and firstSentinel do, see findFirstDetection
    if (maxSentinelSample != 0){ // no need to call function if there will be no detecting
        findFirstDetection(sentinelIndices, maxSentinelSample, sentinelVector, firstSentinel);
    }
    
    if (maxCropSample != 0){
        findFirstDetection(cropIndices, maxCropSample, cropVector, firstCrop);
    }
    
    // if firstPlant =-1 no detection so don't calculate prevalence or reduce strategy scope
    if (firstSentinel == -1 && firstCrop == -1) {
        return;
    }

    // if firstPlant != -1 then a plant was measured as detectable and so all strategies 
    // with that many plants or more no longer need to be checked
    if (firstSentinel != -1) {
        maxSentinelSample = firstSentinel;
    }
    if (firstCrop != -1) {
        maxCropSample = firstCrop;
    }

    // if we get here there has been at least one detection so we calculate prevalence
    prevalence = ((double) cropsPopulationVector[1] + (double) cropsPopulationVector[2]) / 
                 (totalPopulations[0]); // the prevalence is the total number of infected crops divide by the total number of crops
    
    // for each strategy we have:
    // s = sentinel sample size
    // c = sampleSize - s is crop sample size
    //
    // loop through all sentinel samples sizes and see if s > firstSentinel or c > firstCrop. 
    // If either is true then the strategy will have detected the outbreak because all strategies
    // check the same plants (for example all strategies checking 20 or more crops will check the
    // same crop as their 20th crop)
    // sampleDetectionPrevalence[s]=prevalence (exception if s or c = -1 then no detection)
    int c;
    for (int s = 1; s < totalSample; s++) { // s = 1 in randomPosition case
        
        if (strategyChecker[s-1]) {
            // skip if sample was detected in a previous iteration
            continue;
        }
        c = totalSample - s;  
        // if there was a crop detection and no sentinel detection then firstSentinel == -1
        // and so sentinelDetects = false. if there was a sentinal detection but this strategy
        // didn't check enough sentinels to reach the detectable one then sentinelDetects == false
        // otherwise sentinelDetects == true
        bool sentinelDetects = (firstSentinel != -1) && (s > firstSentinel);
        // same idea with cropDetects
        bool cropDetects = (firstCrop != -1) && (c > firstCrop);
        // if a sampling strategy detects either a sentinel or crop with the disease then its 
        // strategyChecker bool becomes true, the strategyCounter goes up and the prevalence at
        // the point of detection is recorded
        if (sentinelDetects || cropDetects) {
            strategyChecker[s-1] = true;
            strategyCounter++;
            prevalenceVector[s-1] = prevalence;
        }
    }
    return;
}

std::vector<double> simulate(int argc, char* argv[], const std::vector<double> &betas, 
                            std::vector<double> &epsilons, std::vector<double> &gammas, 
                            std::vector<int> &totalPopulations, std::vector<int> &U0, 
                            int &sampleSize, int &delta, bool baseline, double radius) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */
    // for every simulation, the counters, checkers, strategy sizes and detection prevalences need to be reset
    bool baselineChecker = false;
    std::vector<bool> strategyChecker;
    int strategyCounter = 0;
    int maxSentinelSample = sampleSize;
    int maxCropSample = sampleSize;
    std::vector<double> sampleDetectionPrevalence;
    // Fills the prevalence vector with zeroes so that it is the right size for indexing
    // and initialises the strategyChecker to false for all strategies
    if (baseline == false) {
        for (int detectionStrategy = 1; detectionStrategy<sampleSize; detectionStrategy++){
            sampleDetectionPrevalence.push_back(0);
            strategyChecker.push_back(false);
        }
    } else {
        sampleDetectionPrevalence.push_back(0);
    }
    // these indices are shuffled every delta and are used to select plants for surveillance
    std::vector<int> cropDetectionIndices(totalPopulations[0]);
    std::vector<int> sentinelDetectionIndices(totalPopulations[1]);
    // fill the shuffled indices vector with unshuffled indices (they will be shuffled later)
    std::iota(cropDetectionIndices.begin(), cropDetectionIndices.end(), 0);
    std::iota(sentinelDetectionIndices.begin(), sentinelDetectionIndices.end(), 0);

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
    flecs::entity networkLink = world.entity(); // plants are in a link with each other
    setupGraph(sentinels, crops, networkLink, totalPopulations[1], 
               totalPopulations[0], radius);
    // Create the systems
    setupSystems(world, crops, sentinels, networkLink, betas, epsilons, gammas, 
                 cropsPopulationVector, sentinelsPopulationVector);
    
    // initial time is some random point in [0,Delta]
    double randTimeFraction = dist(rng); 
    int time = static_cast<int>(delta * randTimeFraction);
    if (baseline == true) {
        while (baselineChecker==false) {
            // infection spread
            world.progress();
            time +=1;
            // if time is not an integer multiple of delta then do not carry out surveillance
            // and move to the next time step
            if (time%delta!=0) {
                continue;
            }
            // shuffle the indices for surveillance sampling
            shuffle(begin(cropDetectionIndices), end(cropDetectionIndices), rng);
            shuffle(begin(sentinelDetectionIndices), end(sentinelDetectionIndices), rng);
            // implement surveillance
            baselineSurveillance(crops, cropDetectionIndices, sampleDetectionPrevalence, 
                                 cropsPopulationVector, totalPopulations, maxCropSample, baselineChecker);
            if (time > 20000) {
                sampleDetectionPrevalence.clear();
                sampleDetectionPrevalence.push_back({-1}); // now if function simulate returns -1 you no to rerun
            }
        }
    } else {
        while (strategyCounter<=sampleSize-2) {
            // infection spread
            world.progress();
            time +=1;
            // if time is not an integer multiple of delta then do not carry out surveillance
            // and move to the next time step
            if (time%delta!=0) {
                continue;
            }
            // shuffle the indices for surveillance sampling
            shuffle(begin(cropDetectionIndices), end(cropDetectionIndices), rng);
            shuffle(begin(sentinelDetectionIndices), end(sentinelDetectionIndices), rng);
            // implement surveillance
            strategySurveillance(sentinels, crops, sentinelDetectionIndices, 
                                    cropDetectionIndices, sampleDetectionPrevalence, 
                                    sampleSize, cropsPopulationVector, totalPopulations, maxSentinelSample, 
                                    maxCropSample, strategyChecker, strategyCounter);
        }
    }
    return sampleDetectionPrevalence;
}
