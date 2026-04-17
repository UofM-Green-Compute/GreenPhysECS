/*
This is a random walk SIR program

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
System 10: Move people who  are on the upper right corner
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
#include <system.h>

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
int numberOfPeople = 2000; // number of people in our system

// Transition constants
double beta = 10 / static_cast<double>(numberOfPeople); // infection rate
double alpha = 0.001; // recovery rate

// initial populations (n1, n2, n3)
int n2 = 1; // State 2 (infected)
int n1 = numberOfPeople-n2; // State 1 (susceptible)
int n3 = 0; // State 3 (recovered)
std::vector<int> population = {n1, n2, n3}; // where population updates occur
std::vector<std::vector<int>> infectionMatrix; // number of infected people at each point in space

int Lx = 100; // x-direction spatial extent of lattice in units of lattice spacing
int Ly = 100; // y-direction spatial extent of lattice in units of lattice spacing

struct Position { int x, y; }; // position struct
// These tags tell you the state of the entity
struct InfectedTag {}; // Important for infection matrix
struct State { int s; }; // Entity infection state
struct MarkovProbabilities { double q1, q2, q3; }; // Marko Chain Probabilities. q1 sends you to S, q2 to I, and q3 to R

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

void setupSystems(flecs::world &world, std::vector<std::vector<int>> &infectionMatrix) {
    updateInfectionMatrix(world, infectionMatrix);
    updateSusceptibleProbabilities(world, infectionMatrix, beta, timeStep);
}


int main(int argc, char* argv[]) {
    double t = 0; // initialize time = 0
    std::ofstream MyFilePerson1; // Create file variable
    MyFilePerson1.open("person1.txt"); // Open text file called "position.txt"
    if (!MyFilePerson1.is_open()) // Ensure file created correctly
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFilePerson1 << "Time, position_x, position_y" << std::endl; // Set column labels

    std::ofstream MyFilePerson2; // Person 2
    MyFilePerson2.open("person2.txt");
    if (!MyFilePerson2.is_open())
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFilePerson2 << "Time, position_x, position_y" << std::endl;

    std::ofstream MyFileInfection; // Create file variable
    MyFileInfection.open("SIR.txt"); // Open text file called "SIR.txt"
    if (!MyFileInfection.is_open()) // Ensure file created correctly
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFileInfection << "Time,Susceptible,Infected,Recovered" << std::endl; // Set column labels

    flecs::world world(argc, argv);

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

    setupSystems(world, infectionMatrix);
    
    // System to add tag to entities dependent on position
    world.system<Position>()
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
    
    // write initial data to file
    const Position& initialPos1 = people[0].get<Position>();
    MyFilePerson1 << 0 << ", " << initialPos1.x << ", " << initialPos1.y << std::endl; 
    const Position& initialPos2 = people[1].get<Position>();
    MyFilePerson2 << 0 << ", " << initialPos2.x << ", " << initialPos2.y << std::endl; 
    MyFileInfection << 0 << "," << population[0] << "," << population[1] << "," << 
    population[2] << std::endl; // Initial data
    
    
    for (int i = 0; i<=Lx; i++) {
        infectionMatrix.push_back({});
        for (int j = 0; j<=Ly; j++) {
            infectionMatrix[i].push_back(0);
        }
    }
    while (t<maxTime) {
        t += timeStep;
        world.progress();
        
        // write data to file
        const Position& initialPos1 = people[0].get<Position>();
        MyFilePerson1 << t << ", " << initialPos1.x << ", " << initialPos1.y << std::endl; 
        const Position& initialPos2 = people[1].get<Position>();
        MyFilePerson2 << t << ", " << initialPos2.x << ", " << initialPos2.y << std::endl; 
        MyFileInfection << t << "," << population[0] << "," << population[1] << "," << 
        population[2] << std::endl;

        // Print
        std::cout << t << "," << population[0] << "," << population[1] << "," << 
        population[2] <<"\n";
        std::cout << "----\n";
        // reset infection matrix
        for (int i = 0; i<=Lx; i++) {
            for (int j = 0; j<=Ly; j++) {
                infectionMatrix[i][j] = {0};
            }
        }
        }
    MyFilePerson1.close();
    MyFilePerson2.close();
    MyFileInfection.close();
}
