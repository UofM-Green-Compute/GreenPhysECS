#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include "markovSystems.h"

// Constants for creating the lattice
double X_LIM = 1;
double Y_LIM = 1;
double C = 1; // Ratio A / B
double PSI = 2 * M_PI / 3; // Allowed values: 0 <= PSI <= PI (radians)

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct Index { int plantNumber; }; // what is your index within your vector
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2,3
struct GridPoint { double x,y; }; // Location of the plant in the lattice
struct InfectedSentinelConnections { int u,d; }; // No. of infected sentinels an individual is connected to. u:undetectable, d:detectable
struct InfectedCropConnections { int u,d; }; // No. of infected crops an individual is connected to. u:undetectable, d:detectable

enum states {healthy = 1, undetectable = 2, detectable = 3};
enum plantType {cropType = 0, sentinelType = 1};

// Tools for picking random numbers 
std::mt19937 rng( std::random_device{}() ); 

// Recursive function to setup the lattice 
std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename, int no_columns){

    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename); 

    // Create a vector to store the lattice positions
    std::vector<std::vector<double>> GridPoints; 

    // Calculate lattice parameters
    double a = X_LIM / (double(no_columns)-1); 
    double b = C * a; 

    // Define terms for later use 
    int backtrack = 0; 
    double x = 0; 
    double y = 0; 

    int count = 0; 
    int j = 0; 
    while ( (y >= 0) && (y <= Y_LIM-(b * sin(PSI))) ){
        if(PSI > M_PI / 2){ x = j * b * -cos(PSI); }
        else{x = j * b * cos(PSI); }
        backtrack = std::floor(x/a); 
        x = x - backtrack * a; 
        y = j * b * sin(PSI);
        while( ((x >= 0) && x <= (X_LIM-a)) && (count < totalPopulation) ){
            MyFile << x << "," << y << "|" ; 
            GridPoints.push_back({x,y}); 
            count += 1; 
            x+=a; 
        }
        j+=1; 
        MyFile << std::endl; 
    }

    MyFile.close(); 

    // Base case 
    if(count == totalPopulation){ return GridPoints; } 

    // Recursive call 
    return setupLattice(totalPopulation, filename, no_columns+1); 
}

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, std::vector<std::vector<double>> &GridPoints, 
    int populationSize, int initialInfected, int plantType){

    int noDeleted = populationSize - GridPoints.size(); 

    for (int i = 0; i < populationSize-initialInfected; ++i) { 
        // Randomly choose a grid point
        if(populationSize-noDeleted-1 < 0) {std::cout<<"ERROR: Grid Points vector is empty"<<std::endl; break; }
        std::uniform_int_distribution<int> dist(0,populationSize-noDeleted-1); 
        int l = dist(rng); 
        // Push back entities to a vector 
        std::vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({i})
                .set<MarkovState>({1})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
                .set<InfectedSentinelConnections>({0,0})
                .set<InfectedCropConnections>({0,0})
                .set<GridPoint>({GridPoints[l][0],GridPoints[l][1]})
        ); 
        // Delete the used coordinate from gridpoints vector
        GridPoints.erase(GridPoints.begin() + l);
        noDeleted += 1; 
    }
    for (int i = 0; i < initialInfected; ++i) {
        // Randomly choose a grid point 
        if(populationSize-noDeleted-1 < 0) {std::cout<<"ERROR (setupEntities loop 2): Grid Points vector is empty"<<std::endl; break; }
        std::uniform_int_distribution<int> dist(0,populationSize-noDeleted-1); 
        int m = dist(rng);
        // Push back entities to a vector
        std::vector<double> Qnm = {0, 0, 0}; 
        p.push_back( 
            world.entity() 
                .set<Index>({populationSize-initialInfected + i})
                .set<MarkovState>({2})
                .set<PlantState> ({plantType})
                .set<TransitionProbabilities>({Qnm})
                .set<InfectedSentinelConnections>({0,0})
                .set<InfectedCropConnections>({0,0})
                .set<GridPoint>({GridPoints[m][0],GridPoints[m][1]})
        );
        // Delete the used coordinate from gridpoints vector
        GridPoints.erase(GridPoints.begin() + m);
        noDeleted += 1;
    }
}

void setupComponents(flecs::world world){
    world.component<Index>();
    world.component<MarkovState>();
    world.component<PlantState>();
    world.component<TransitionProbabilities>();
    world.component<InfectedSentinelConnections>(); 
    world.component<InfectedCropConnections>(); 
    world.component<GridPoint>(); 
}

double findPlantDistance (flecs::entity plant1, flecs::entity plant2) {
    double deltaX;
    double deltaY;
    double distance;
    deltaX = plant2.get<GridPoint>().x - plant1.get<GridPoint>().x;
    deltaY = plant2.get<GridPoint>().y - plant1.get<GridPoint>().y;
    distance = std::sqrt(deltaX*deltaX + deltaY*deltaY);

    return distance;
}

void addLink(flecs::entity plant1, flecs::entity plant2, flecs::entity Link, double radius){
    // Get distance between plants 
    double distance = findPlantDistance(plant1, plant2);
    // std::cout<<(distance<=radius)<<std::endl; 
    if (distance>radius) {
        // if distance > radius, don't make a connection
        return;
    }
    else{
        plant1.add(Link, plant2);
        plant2.add(Link, plant1);
        // std::cout<<"Ayo"<<std::endl; 
    }
}

void setupGraph(std::vector<flecs::entity> &sentinelPopulation, std::vector<flecs::entity> &cropPopulation,
                flecs::entity Link, int noCrops, int noSentinels, double radius){
    // start with all the sentinel connections (no need to check the final sentinel as all links will have been made)
    for(int i = 0; i < noSentinels-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noSentinels; k++){
            addLink(sentinelPopulation[i], sentinelPopulation[k], Link, radius);
        }
        // compare with all crops
        for (int k = 0; k < noCrops; k++){ 
            addLink(sentinelPopulation[i], cropPopulation[k], Link, radius);
        }
    }
    // do the same with crops. (no need to compare with sentinels as that was done previously)
    for(int i = 0; i < noCrops-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noCrops; k++){
            addLink(cropPopulation[i], cropPopulation[k], Link, radius);
        }
    }
}

void setupSystems(flecs::world &world, std::vector<flecs::entity> &cropPopulation, std::vector<flecs::entity> &sentinelPopulation,
    std::vector<int> &cropNumbers, std::vector<int> &sentinelNumbers, const std::vector<double> &infectionRates, std::vector<double> &scalings, 
    std::vector<double> &presymptomaticTimes, flecs::entity &Link) {
    countNeighbours(world, cropPopulation, sentinelPopulation, Link); 
    updateProbabilities(world, infectionRates, scalings, presymptomaticTimes);
    transition(world, cropNumbers, sentinelNumbers);
}

int simulate(int argc, char* argv[], const std::vector<double> betas, std::vector<double> epsilons, 
    std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int maxTime, double radius, std::string filename1, std::string filename2, std::string filename3) {
    /*
    This function
    1) Initializes a population vector
    2) Creates a world
    3) Sets up the entities, components, and systems
    4) Runs the simulation
    5) Saves the data to a data file every time step
    */

    // Get the total population size from the individual vectors 
    int PopulationSize = 0; 
    for(int i = 0; i < totalPopulations.size(); i++){
        PopulationSize += totalPopulations[0]; 
    }

    // Population vector will be updated and saved over time
    std::vector<int> cropsNumbersVector = {totalPopulations[0]-U0[0], U0[0], 0};
    std::vector<int> sentinelsNumbersVector = {totalPopulations[1]-U0[1], U0[1], 0};

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    setupComponents(world);

    // Set up the lattice 
    std::vector<std::vector<double>> gridPoints = setupLattice(PopulationSize, filename3, 2); 

    // Create a crop and sentinels vector and generate entities
    std::vector<flecs::entity> crops;
    std::vector<flecs::entity> sentinels; 
    crops.reserve(totalPopulations[0]);
    sentinels.reserve(totalPopulations[1]);
    setupEntities(world, crops, gridPoints, totalPopulations[0], U0[0], 0);
    setupEntities(world, sentinels, gridPoints, totalPopulations[1], U0[1], 1); 

    // Relationship types 
    flecs::entity plantLink = world.entity(); // both plants are healthy

    // Set up the graph 
    setupGraph(sentinels, crops, plantLink, totalPopulations[0], totalPopulations[1], radius);

    // Create the systems
    setupSystems(world, crops, sentinels, cropsNumbersVector, sentinelsNumbersVector, betas, epsilons, gammas, plantLink); 

    // Run the simulation
    std::ofstream MyFile1; // crops
    std::ofstream MyFile2; // sentinels
    MyFile1.open(filename1);
    MyFile2.open(filename2);
    MyFile1 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    MyFile2 << "Time,Healthy,Undetectable,Detectable" << std::endl; 
    double time = 0;
    MyFile1 << time << "," << cropsNumbersVector[0] << "," << cropsNumbersVector[1] << ","
            << cropsNumbersVector[2] << std::endl;  
    MyFile2 << time << "," << sentinelsNumbersVector[0] << "," << sentinelsNumbersVector[1] << ","
            << sentinelsNumbersVector[2] << std::endl;  
    while (time<maxTime) {
        world.progress();
        time += 1;
        // update sample variables
        MyFile1 << time << "," << cropsNumbersVector[0] << "," << cropsNumbersVector[1] << ","
            << cropsNumbersVector[2] << std::endl;  
        MyFile2 << time << "," << sentinelsNumbersVector[0] << "," << sentinelsNumbersVector[1] << ","
                << sentinelsNumbersVector[2] << std::endl;  
    }
    MyFile1.close();
    MyFile2.close();
    return 0;
}