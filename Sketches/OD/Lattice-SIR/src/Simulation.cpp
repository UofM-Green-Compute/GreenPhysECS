#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include "markovSystems.h"

// Constants for creating the lattice
double X_LIM = 1;
double Y_LIM = 1;
double A = 0.1;
double C = 0.5; // Ratio A / B
double B_ = A * C;
double PSI = M_PI / 3; // Allowed values: 0 <= PSI <= PI (radians)

// Arbitrary constant
double epsilon = 0.00001; // Epsilon must be less than grid spacing 

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
std::mt19937 rng( std::random_device{}()  ); 

bool double_equals(double a, double b, double eps = epsilon)
{
    return std::abs(a - b) < eps;
}

// Recursive function to setup the lattice 
std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename, int n){

    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename); 

    // Create a vector to store the lattice positions
    std::vector<std::vector<double>> GridPoints; 

    int backtrack = 0; 
    double x = 0; 
    double y = 0; 

    int count = 0; 
    int j = 0; 
    while ( (y >= 0) && y <= (Y_LIM + n*B_) ){
        if(PSI > M_PI / 2){ x = j * B_ * -cos(PSI); }
        else{x = j * B_ * cos(PSI); }
        backtrack = std::floor(x/A); 
        x = x - backtrack * A; 
        y = j * B_*sin(PSI); 
        while( (x >= 0) && x <= (X_LIM + n*A) && count < totalPopulation){
            MyFile << x << "," << y << "|" ; 
            GridPoints.push_back({x,y}); 
            count += 1; 
            x+=A; 
        }
        j+=1; 
        MyFile << std::endl; 
    }

    MyFile.close(); 

    // Base case 
    if(count == totalPopulation){ std::cout<<"Satisfied"<<std::endl; return GridPoints; } 

    // Recursive call 
    return setupLattice(totalPopulation, filename, n+1); 

}


std::vector<double> latticeConstants(int totalPopulation){

    // Calculate the variables 
    double N_X = std::floor(((X_LIM-A) / A)+1);
    double N_Y = std::ceil(totalPopulation / N_X);
    double B = Y_LIM / ( (N_Y-1) * sin(PSI)); 

    // Modify vertical spacing based on the "excess"
    double excess = std::floor( A / (B * cos(PSI)) );
    double additionalRows = std::ceil( excess / N_X ); 
    N_Y += additionalRows; 
    B = Y_LIM / ( (N_Y-1) * sin(PSI)); 

    return {A,B,N_X,N_Y}; 

}

// std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename){
    
//     // Open files 
//     std::ofstream MyFile; 
//     MyFile.open(filename); 

//     // Create a vector to store the lattice positions
//     std::vector<std::vector<double>> GridPoints; 

//     // Initialise variables
//     double x = 0; 
//     double y = 0; 
//     int no_a = 0; 
//     int index = 0; 

//     // Calculate the variables 
//     double B = latticeConstants(totalPopulation)[1];
//     double N_Y = latticeConstants(totalPopulation)[3]; 

//     // Iterate to create the lattice
//     for(int j = 0; j < N_Y+1; j++){ // Change this to a while loop? 

//         y = j * B * sin(PSI); 

//         if(PSI > M_PI / 2){ x = j * B * -cos(PSI); }
//         else{x = j * B * cos(PSI); }
//         no_a = std::floor(x/A); 
//         x = x - no_a * A; 

//         while(index < totalPopulation){
//             if((x <= X_LIM)){
//                 MyFile << x << "," << y << "|" ; 
//                 GridPoints.push_back({x,y}); 
//                 index+=1;
//             }
//             else{ break; }
//             x+= A; 
//         }
//         MyFile << std::endl; 
//     }
//     return GridPoints; 
// }

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

void addLink(flecs::entity plant1, flecs::entity plant2, flecs::entity hLink, flecs::entity uLink,
             flecs::entity dLink, double radius){
    // Get distance between plants 
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
                flecs::entity hLink, flecs::entity uLink, flecs::entity dLink, int noCrops, 
                int noSentinels, double radius){
    // start with all the sentinel connections (no need to check the final sentinel as all links will have been made)
    for(int i = 0; i < noSentinels-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noSentinels; k++){
            // Remove any existing links 
            if (sentinelPopulation[i].has(hLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(hLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(uLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(uLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(dLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(dLink,cropPopulation[k]);
            }
            // Add new links 
            addLink(sentinelPopulation[i], sentinelPopulation[k], hLink, uLink, dLink, radius);
        }
        // compare with all crops
        for (int k = 0; k < noCrops; k++){
            // Remove any existing links 
            if (sentinelPopulation[i].has(hLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(hLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(uLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(uLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(dLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(dLink,cropPopulation[k]);
            }
            // Add new links 
            addLink(sentinelPopulation[i], cropPopulation[k], hLink, uLink, dLink, radius);
        }
    }
    // do the same with crops. (no need to compare with sentinels as that was done previously)
    for(int i = 0; i < noCrops-1; i++){
        // compare with all other sentinels
        for (int k = i+1; k < noCrops; k++){
            // Remove any existing links 
            if (sentinelPopulation[i].has(hLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(hLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(uLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(uLink,cropPopulation[k]);
            }
            if (sentinelPopulation[i].has(dLink,cropPopulation[k])){
                    sentinelPopulation[i].remove(dLink,cropPopulation[k]);
            }
            // Add new links 
            addLink(cropPopulation[i], cropPopulation[k], hLink, uLink, dLink, radius);
        }
    }
}

void setupSystems(flecs::world &world, std::vector<flecs::entity> &cropPopulation, std::vector<flecs::entity> &sentinelPopulation,
    std::vector<int> &cropNumbers, std::vector<int> &sentinelNumbers, const std::vector<double> &infectionRates, std::vector<double> &scalings, 
    std::vector<double> &presymptomaticTimes, flecs::entity &uLink, flecs::entity &dLink) {
    countNeighbours(world, cropPopulation, sentinelPopulation, uLink, dLink); 
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

    // Set up the lattice and network
    std::vector<std::vector<double>> gridPoints = setupLattice(PopulationSize, filename3, 0); 

    // Create a crop and sentinels vector and generate entities
    std::vector<flecs::entity> crops;
    std::vector<flecs::entity> sentinels; 
    crops.reserve(totalPopulations[0]);
    sentinels.reserve(totalPopulations[1]);
    setupEntities(world, crops, gridPoints, totalPopulations[0], U0[0], 0);
    setupEntities(world, sentinels, gridPoints, totalPopulations[1], U0[1], 1); 

    // Relationship types 
    flecs::entity healthyLink = world.entity(); // both plants are healthy
    flecs::entity undetectableLink = world.entity(); // at least one plant is undetectable
    flecs::entity detectableLink = world.entity(); // at least one plant is detectable

    // Create the systems
    setupSystems(world, crops, sentinels, cropsNumbersVector, sentinelsNumbersVector, betas, epsilons, gammas, undetectableLink, detectableLink);

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
        setupGraph(sentinels, crops, healthyLink, undetectableLink, detectableLink, totalPopulations[0], totalPopulations[1], radius); 
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