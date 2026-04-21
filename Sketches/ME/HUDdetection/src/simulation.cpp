// simulation.cpp
#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <filesystem>
#include "markovSystems.h"

using namespace std;

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct Index { int plantNumber; }; // what is your index within your vector
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { vector<double> qnm; }; // Transition from state m to state n = 1,2, 3
struct InfectedConnections { int u,d; }; // No. of infected people an individual is connected to. u:undetectable, d:detectable

// Tools for picking random numbers from a uniform distribution
mt19937 rng(random_device{}()) ; 
uniform_real_distribution<double> dist(0,1); 

// states for readability
enum states {healthy = 1, undetectable = 2, detectable = 3};
enum plantType {cropType = 0, sentinelType = 1};

void setupComponents(flecs::world &world){
    // what these components do is outlined when the structs are defined
    world.component<Index>();
    world.component<MarkovState>();
    world.component<PlantState>();
    world.component<TransitionProbabilities>();
    world.component<InfectedConnections>();
}

void setupEntities(flecs::world &world, vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected, int plantType){
    // Qnm will be used to initialise the transition probabilities
    // later a system will update this with the correct probabilities
    vector<double> Qnm = {0, 0, 0};

    // generate your healthy entities
    // total number of healthy entities is totalPopulation-initialInfected
    for (int i = 0; i < totalPopulation-initialInfected; ++i) { 
        p.push_back( 
            world.entity() 
                .set<Index>({i})
                .set<MarkovState>({healthy})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
                .set<InfectedConnections>({0,0})
        ); 
    } 
    // generate your undetecatble but infected entities
    // total number of infected entities is initialInfected
    // all initial infected entities are undetectable
    for (int i = 0; i < initialInfected; ++i) { 
        p.push_back( 
            world.entity() 
                .set<Index>({totalPopulation-initialInfected + i})
                .set<MarkovState>({undetectable})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
        );
    }
}


void setupSystems(flecs::world &world, vector<int> &cropPopulation, vector<int> &sentinelPopulation,
    const vector<double> &infectionRates, vector<double> &scalings, vector<double> &presymptomaticTimes) {
    // Update probabilities acts on the TransitionProbabilities component of the entities
    updateProbabilities(world, cropPopulation, sentinelPopulation, infectionRates, scalings, presymptomaticTimes);
    // transition acts on the MarkovState component of the entities
    transition(world, cropPopulation, sentinelPopulation);
}

void findFirstDetection(vector<int> &indices, int &maxSample, 
                    const vector<flecs::entity> &plantVector, int &firstPlant) {
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

void baselineSurveillance(const vector<flecs::entity> &cropVector, vector<int> &cropIndices, 
                          vector<double> &prevalenceVector, const vector<int> &cropsPopulationVector, 
                          const vector<int> &totalPopulations, int &maxCropSample, bool &baselineChecker) {
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

void strategySurveillance(const vector<flecs::entity> &sentinelVector,
                          const vector<flecs::entity> &cropVector,
                          vector<int> &sentinelIndices, vector<int> &cropIndices, 
                          vector<double> &prevalenceVector, int totalSample, 
                          const vector<int> &cropsPopulationVector, 
                          const vector<int> &totalPopulations, int &maxSentinelSample,
                          int &maxCropSample, vector<bool> &strategyChecker, 
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
    for (int s = 0; s <= totalSample; s++) {
        if (strategyChecker[s]) {
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
            strategyChecker[s] = true;
            strategyCounter++;
            prevalenceVector[s] = prevalence;
        }
    }
    return;
}

vector<double> simulate(int argc, char* argv[], const vector<double> &betas, vector<double> &epsilons, 
    vector<double> &gammas, vector<int> &totalPopulations, vector<int> &U0, int &sampleSize, int &delta, bool baseline) {
    /*
    This function
    1) Initializes all necessary variables
    2) Creates a world
    3) Sets up the entities, components, and systems in the world
    4) Runs the simulation and carries out surveillance
    5) Returns the disease prevalence for the baseline case or for different strategies
    */
    // for every simulation, the counters, checkers, strategy sizes and detection prevalences need to be reset
    bool baselineChecker = false;
    vector<bool> strategyChecker;
    int strategyCounter = 0;
    int maxSentinelSample = sampleSize;
    int maxCropSample = sampleSize;
    vector<double> sampleDetectionPrevalence;
    // Fills the prevalence vector with zeroes so that it is the right size for indexing
    // and initialises the strategyChecker to false for all strategies
    if (baseline == false) {
        for (int detectionStrategy = 0; detectionStrategy<=sampleSize; detectionStrategy++){
            sampleDetectionPrevalence.push_back(0);
            strategyChecker.push_back(false);
        }
    } else {
        sampleDetectionPrevalence.push_back(0);
    }
    // these indices are shuffled every delta and are used to select plants for surveillance
    vector<int> cropDetectionIndices(totalPopulations[0]);
    vector<int> sentinelDetectionIndices(totalPopulations[1]);
    // fill the shuffled indices vector with unshuffled indices (they will be shuffled later)
    iota(cropDetectionIndices.begin(), cropDetectionIndices.end(), 0);
    iota(sentinelDetectionIndices.begin(), sentinelDetectionIndices.end(), 0);
    
    // Population vector is used to calculate probabilities and will be updated over time
    vector<int> cropsPopulationVector = {totalPopulations[0]-U0[0], U0[0], 0};
    vector<int> sentinelsPopulationVector = {totalPopulations[1]-U0[1], U0[1], 0};
    // Initialise flecs world
    flecs::world world(argc,argv);
    // Create components
    setupComponents(world);
    // Create a crop and sentinels vector and generate entities
    vector<flecs::entity> crops;
    vector<flecs::entity> sentinels; 
    crops.reserve(totalPopulations[0]);
    sentinels.reserve(totalPopulations[1]);
    setupEntities(world, crops, totalPopulations[0], U0[0], cropType);
    setupEntities(world, sentinels, totalPopulations[1], U0[1], sentinelType); 
    // Create the systems
    setupSystems(world, cropsPopulationVector, sentinelsPopulationVector, betas, epsilons, gammas);
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
        }
    } else {
        while (strategyCounter<=sampleSize) {
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
