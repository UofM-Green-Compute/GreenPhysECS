/*
This code ensemble averages the random walk SIR code.

There are 13 systems
System 1: Update infection matrix so entities can see number of infected people on their tile
System 2: Upate susceptible people probabilities based on infection matrix
System 3: Carry out transitions using a random number generator
System 4: Add Position tags to entities depending on if they're on a wall, corner, bulk
System 5: Move people who are on the left wall
System 6: Move people who are on the upper wall
System 7: Move people who are on the right wall
System 8: Move people who are on the lower wall
System 9: Move people who are on the upper left corner
System 10: Move people who are on the upper right corner
System 11: Move people who are on the lower left corner
System 12: Move people who are on the lower right corner
System 13: Move everyone else who is in the bulk
*/

#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <cmath>
#include <random>

double timeStep = 0.1; // Set timestep = 0.0001
double maxTime = 5000; // Maximum Simulation Time
double latticeSpacing = 1; // lattice spacing in metres
// walking speed is in ms-1. if the spacing is latticeSpacing metres than after one second
// you expect that a person has made speed/latticeSpacing steps. so the probability of moving has to be
// such that the number of steps per second is 1. Therefore lambda = 1.
double speed = 1; 
double lambda = speed/latticeSpacing;
double pStay = exp(-lambda*timeStep); // probability of staying is an exponential with time
double pMove = 1-pStay; // probability of moving at any time step
int numberOfPeople = 100; // number of people in our system

// ensemble parameters
int sampleCounter = 1; // Set sample counter = 1
int sampleNumber = 100; // total number of samples

// Transition constants
double beta = 10 / static_cast<double>(numberOfPeople); // infection rate
double alpha = 0.001; // recovery rate

// initial populations (n1, n2, n3)
int n2 = 10; // State 2 (infected)
int n1 = numberOfPeople-n2; // State 1 (susceptible)
int n3 = 0; // State 3 (recovered)

int Lx = 50; // x-direction spatial extent of lattice in units of lattice spacing
int Ly = 50; // y-direction spatial extent of lattice in units of lattice spacing

struct Position { int x, y; }; // position struct
// These tags tell you the state of the entity
struct InfectedTag {}; // Important for infection matrix
struct State { int s; }; // Entity infection state
struct MarkovProbabilities { double q1, q2, q3; }; // Markov Chain Probabilities. q1 sends you to S, q2 to I, and q3 to R

// These Tags tell you the type of tile the entity is on
struct BulkTag {};
struct LeftTag {};
struct UpTag {};
struct DownTag {};
struct RightTag {};
struct UpperLeftTag{};
struct UpperRightTag{};
struct LowerLeftTag{};
struct LowerRightTag{};

int generatePosition(int length) {
    // generates a random position on the lattice
    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, length); // uniform integer distribution [0, length]
    return distrib(gen);
}

double generateProbability() {
    // generates a random number between 0 and 1 for use in markov chain probability
    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double> distrib(0, 1); // uniform double distribution [0, 1]
    return distrib(gen);
}

void runModel(int argc, char* argv[], std::vector<int> &population1, 
    std::vector<int> &population2, std::vector<int> &population3, std::vector<double> &timeArray) {
    // preamble
    double t = 0; // initialize time = 0
    timeArray = {0};
    std::vector<int> population = {n1, n2, n3}; // nS, nI, NR
    std::vector<std::vector<int>> infectionMatrix; // number of infected people at each point in space
    population1 = {n1};
    population2 = {n2};
    population3 = {n3};

    flecs::world world(argc, argv);

    // Create Phases which tell the program in which order to run the system
    // Creates a matrix of infected people
    flecs::entity matrixPhase = world.entity()
        .add(flecs::Phase);
    // This Phase updates the SIR transition probabilities for each susceptible entity based on
    // number of infected people on the same tile
    flecs::entity markovPhase = world.entity()
        .add(flecs::Phase)
        .depends_on(matrixPhase);
     // This phase changes states dependenent on probabilities found in previous system
    flecs::entity transitionPhase = world.entity()
        .add(flecs::Phase)
        .depends_on(markovPhase); 
    // This Phase sees if person is on edge, corner etc.
    flecs::entity findTagPhase = world.entity()
        .add(flecs::Phase)
        .depends_on(transitionPhase);
    // These phases calculate new position. calculation depends on tag
    flecs::entity leftWallMove = world.entity() 
        .add(flecs::Phase) 
        .depends_on(findTagPhase);
    flecs::entity upWallMove = world.entity()
        .add(flecs::Phase)
        .depends_on(leftWallMove);
    flecs::entity rightWallMove = world.entity()
        .add(flecs::Phase)
        .depends_on(upWallMove);
    flecs::entity downWallMove = world.entity()
        .add(flecs::Phase)
        .depends_on(rightWallMove);
    flecs::entity upLeftCornerMove = world.entity()
        .add(flecs::Phase)
        .depends_on(downWallMove);
    flecs::entity upRightCornerMove = world.entity()
        .add(flecs::Phase)
        .depends_on(upLeftCornerMove);
    flecs::entity downLeftCornerMove = world.entity()
        .add(flecs::Phase)
        .depends_on(upRightCornerMove);
    flecs::entity downRightCornerMove = world.entity()
        .add(flecs::Phase)
        .depends_on(downLeftCornerMove);
    flecs::entity bulkMove = world.entity()
        .add(flecs::Phase)
        .depends_on(downRightCornerMove);

    // Create components inside world
    world.component<Position>();
    world.component<InfectedTag>();
    world.component<State>();
    world.component<MarkovProbabilities>();
    world.component<BulkTag>();
    world.component<UpTag>();
    world.component<DownTag>();
    world.component<LeftTag>();
    world.component<RightTag>();
    world.component<UpperLeftTag>();
    world.component<UpperRightTag>();
    world.component<LowerLeftTag>();
    world.component<LowerRightTag>();

    // Initialize the people
    std::vector<flecs::entity> people;
    people.reserve(numberOfPeople);

    // Generate susceptible people
    for (int i = 0; i < n1; ++i) {
        people.push_back(
            world.entity()
                .set<Position>({generatePosition(Lx), generatePosition(Ly)}) //random position on lattice
                .set<MarkovProbabilities>({0, 0, 0})
                .set<State>({1})
            );
    }
    
    // Initialise the Infected people
    for (int i = 0; i < n2; ++i) { 
        people.push_back( 
            world.entity() 
                .add<InfectedTag>()
                .set<Position>({generatePosition(Lx), generatePosition(Ly)}) //random position on lattice
                .set<MarkovProbabilities>({0, exp(-alpha*timeStep), 1-exp(-alpha*timeStep)})
                .set<State>({2})      
        );
    }
    
    // update infection matrix
    world.system<Position>()
        .kind(matrixPhase)
        .with<InfectedTag>()
        .each([&](Position &pos){
            infectionMatrix[pos.x][pos.y]+=1;
            //std::cout<<"Segmentation fault??? 1"<<"\n";
        });

    // update susceptible markov probabilities
    world.system<State, MarkovProbabilities, Position>()
        .kind(markovPhase)
        .each([&](State &state, MarkovProbabilities& prob, Position &pos){
        // find number of infected people on your and update probabilities
        if (state.s == 1) {
            int infectedContact = infectionMatrix[pos.x][pos.y];
            prob.q1 = exp(-beta * infectedContact * timeStep);
            prob.q2 = 1-prob.q1;
            prob.q3 = 0;
            //std::cout<<"Segmentation Fault??? 2"<<"\n";
        }
        });

    world.system<State, MarkovProbabilities>()
        .kind(transitionPhase)
        .each([&](flecs::entity e, State& state, MarkovProbabilities& prob){
            double rand = generateProbability(); // random number between 0 and 1
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
            } else if((rand > prob.q1) && (rand < (prob.q1 + prob.q2))){
                // transition to state 2 (infected)
                if (state.s == 1) {
                    population[1] += 1;
                    population[0] -= 1;
                    e.add<InfectedTag>();
                } else if (state.s == 3) {
                    population[1] += 1;
                    population[2] -= 1;  
                    e.add<InfectedTag>();
                }
                state.s = 2;
                prob.q1 = 0;
                prob.q2 = exp(-alpha*timeStep);
                prob.q3 = 1-prob.q2;
            } else if((rand > (prob.q1 + prob.q2)) && (rand < (prob.q1 + prob.q2 + prob.q3))){
                // transition to state 3 (recovered)
                if (state.s == 1) {
                    population[2] += 1;
                    population[0] -= 1;
                } else if (state.s == 2) {
                    population[2] += 1;
                    population[1] -= 1;
                    e.remove<InfectedTag>();
                }
                state.s = 3;
                prob.q1 = 0;
                prob.q2 = 0;
                prob.q3 = 1;
                //std::cout<<"Segmentation Fault??? 3"<<"\n";
            }
        });

    // System to add tag to entities dependent on position
    world.system<Position>()
        .kind(findTagPhase)
        .each([&](flecs::entity e, Position& pos){
            if (pos.x == 0 && pos.y == 0) {
                e.add<UpperLeftTag>();
            } else if (pos.x == Lx && pos.y == 0) {
                e.add<UpperRightTag>();
            } else if (pos.x == 0 && pos.y == Ly) {
                e.add<LowerLeftTag>();
            } else if (pos.x == Lx && pos.y == Ly) {
                e.add<LowerRightTag>();
            } else if (pos.x == 0) {
                e.add<LeftTag>();
            } else if (pos.x == Lx) {
                e.add<RightTag>();
            } else if (pos.y == 0) {
                e.add<UpTag>();
            } else if (pos.y == Ly) {
                e.add<DownTag>();
            } else {
                e.add<BulkTag>();
            }
            //std::cout<<"Segmentation Fault??? 4"<<"\n";
        });
    
    world.system<Position>()
        .with<LeftTag>()
        .kind(leftWallMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/3) {
            // Event 1 happens: move up
            pos.y -= 1;
        } else if (p < 2*pMove/3) {
            // Event 2 happens: move right
            pos.x += 1;
        } else if (p < pMove) {
            // Event 3 happens: move down
            pos.y +=1;
        } // Else Event 4: Stay still
        e.remove<LeftTag>(); // We no longer know whether the entity is on the left wall
        //std::cout<<"Segmentation Fault??? 5"<<"\n";
        });
    
    world.system<Position>()
        .with<UpTag>()
        .kind(upWallMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/3) {
            // Event 1 happens: move left
            pos.x -= 1;
        } else if (p < 2*pMove/3) {
            // Event 2 happens: move down
            pos.y += 1;
        } else if (p < pMove) {
            // Event 3 happens: move right
            pos.x +=1;
        } // Else Event 4: Stay still
        e.remove<UpTag>(); // We no longer know whether the entity is on the upper wall
        //std::cout<<"Segmentation Fault??? 6"<<"\n";
        });

    world.system<Position>()
        .with<RightTag>()
        .kind(rightWallMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/3) {
            // Event 1 happens: move up
            pos.y -= 1;
        } else if (p < 2*pMove/3) {
            // Event 2 happens: move left
            pos.x -= 1;
        } else if (p < pMove) {
            // Event 3 happens: move down
            pos.y +=1;
        } // Else Event 4: Stay still
        e.remove<RightTag>(); // We no longer know whether the entity is on the right wall
        //std::cout<<"Segmentation Fault??? 7"<<"\n";
        });

    world.system<Position>()
        .with<DownTag>()
        .kind(downWallMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/3) {
            // Event 1 happens: move left
            pos.x -= 1;
        } else if (p < 2*pMove/3) {
            // Event 2 happens: move up
            pos.y -= 1;
        } else if (p < pMove) {
            // Event 3 happens: move right
            pos.x +=1;
        } // Else Event 4: Stay still
        e.remove<DownTag>(); // We no longer know whether the entity is on the lower wall
        //std::cout<<"Segmentation Fault??? 8"<<"\n";
        });

    world.system<Position>()
        .with<UpperLeftTag>()
        .kind(upLeftCornerMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/2) {
            // Event 1 happens: move right
            pos.x += 1;
        } else if (p < pMove) {
            // Event 2 happens: move down
            pos.y += 1;
        } // Else Event 3: Stay still
        e.remove<UpperLeftTag>(); // We no longer know whether the entity is on upper left corner
        //std::cout<<"Segmentation Fault??? 9"<<"\n";
        });
    
    world.system<Position>()
        .with<UpperRightTag>()
        .kind(upRightCornerMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/2) {
            // Event 1 happens: move left
            pos.x -= 1;
        } else if (p < pMove) {
            // Event 2 happens: move down
            pos.y += 1;
        } // Else Event 3: Stay still
        e.remove<UpperRightTag>(); // We no longer know whether the entity is on upper right corner
        //std::cout<<"Segmentation Fault??? 10"<<"\n";
        });

    world.system<Position>()
        .with<LowerLeftTag>()
        .kind(downLeftCornerMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/2) {
            // Event 1 happens: move right
            pos.x += 1;
        } else if (p < pMove) {
            // Event 2 happens: move up
            pos.y -= 1;
        } // Else Event 3: Stay still
        e.remove<LowerLeftTag>(); // We no longer know whether the entity is on lower left corner
        //std::cout<<"Segmentation Fault??? 11"<<"\n";
        });
    
    world.system<Position>()
        .with<LowerRightTag>()
        .kind(downRightCornerMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/2) {
            // Event 1 happens: move left
            pos.x -= 1;
        } else if (p < pMove) {
            // Event 2 happens: move up
            pos.y -= 1;
        } // Else Event 3: Stay still
        e.remove<LowerRightTag>(); // We no longer know whether the entity is on lower right corner
        //std::cout<<"Segmentation Fault??? 12"<<"\n";
        });

    world.system<Position>()
        .with<BulkTag>()
        .kind(bulkMove)
        .each([&](flecs::entity e, Position& pos){
        double p = generateProbability(); // number between 0 and 1
        if (p < pMove/4) {
            // Event 1 happens: move left
            pos.x -= 1;
        } else if (p < pMove/2) {
            // Event 2 happens: move up
            pos.y -= 1;
        } else if (p < 3*pMove/4) {
            // Event 3 happens: move right
            pos.x += 1;
        } else if (p < pMove) {
            // Event 4 happens: move down
            pos.y += 1;
        } // Else Event 5: Stay still
        e.remove<BulkTag>(); // We no longer know whether the entity is on lower right corner
        //std::cout<<"Segmentation Fault??? 13"<<"\n";
        });
    
    for (int i = 0; i<=Lx; i++) {
        infectionMatrix.push_back({});
        for (int j = 0; j<=Ly; j++) {
            infectionMatrix[i].push_back(0);
        }
    }
    while (t<maxTime) {
        world.progress();
        t += timeStep;
        // update sample variables
        timeArray.push_back(t);
        population1.push_back(population[0]);
        population2.push_back(population[1]);
        population3.push_back(population[2]);  
        // reset infection matrix
        for (int i = 0; i<=Lx; i++) {
            for (int j = 0; j<=Ly; j++) {
                infectionMatrix[i][j] = {0};
            }
        }
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
    // Infection Files
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
        runModel(argc,argv, individualPopulation1, individualPopulation2, individualPopulation3, 
            individualTimes);
        // Record the first sample as an individual case
        if (sampleCounter == 1){
            for(int i = 0; i < (int) individualPopulation1.size(); i++) {
                MyFileSample << i*timeStep << "," << individualPopulation1[i] << "," << 
                individualPopulation2[i] << "," << individualPopulation3[i] << std::endl; 
            }
        }
        std::cout<<"Sample = "<< sampleCounter <<std::endl;
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
