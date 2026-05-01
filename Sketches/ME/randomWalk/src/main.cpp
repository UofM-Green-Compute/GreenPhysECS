/*
main.cpp
There are 10 systems
System 1: Add tags to entities depending on if they're on a wall, corner, bulk
System 2: Move people who are on the left wall
System 3: Move people who are on the upper wall
System 4: Move people who are on the right wall
System 5: Move people who are on the lower wall
System 6: Move people who are in the upper left corner
System 7: Move people who are in the upper right corner
System 8: Move people who are in the lower left corner
System 9: Move people who are in the lower right corner
System 10: Move everyone else who is in the bulk
*/

#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <cmath>
#include <random>
#include <filesystem>

double timeStep = 0.1; // Set timestep = 0.0001
double maxTime = 5000; // Maximum Simulation Time
double latticeSpacing = 1; // lattice spacing in metres
// walking speed is in ms-1. if the spacing is latticeSpacing metres than after one second
// you expect that a person has made speed/latticeSpacing steps. so the probability of moving has to be
// such that the number of steps per second is 1. Therefore lambda = 1.
double speed = 1; 
double lambda = speed/latticeSpacing;
double pMove = lambda*timeStep; // probability of moving at any time step
double pStay = 1-pMove; // probability of staying is an exponential with time
int numberOfPeople = 1; // number of people in our system
int Lx = 50; // x-direction spatial extent of lattice in each direction in units of lattice spacing
int Ly = 50; // y-direction spatial extent of lattice in each direction in units of lattice spacing

struct Position { int x, y; }; // position struct

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

double generateProbability() {
    // generates a random number between 0 and 1 for use in markov chain probability
    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<double> distrib(0, 1); // uniform double distribution [0, 1]
    return distrib(gen);
}

void setupComponents(flecs::world &world){
    // what these components do is outlined when the structs are defined
    world.component<Position>();
    world.component<BulkTag>();
    world.component<UpTag>();
    world.component<DownTag>();
    world.component<LeftTag>();
    world.component<RightTag>();
    world.component<UpperLeftTag>();
    world.component<UpperRightTag>();
    world.component<LowerLeftTag>();
    world.component<LowerRightTag>();
}

void setupEntities(flecs::world &world, std::vector<flecs::entity> &p, 
                   std::vector<std::ofstream> &files){
    // generate your healthy entities
    // total number of healthy entities is totalPopulation-initialInfected
    for (int i = 0; i < numberOfPeople; ++i) { 
        p.push_back( 
            world.entity() 
                .set<Position>({0})
        ); 
    
        char FILENAME[50]; 
        sprintf(FILENAME, "walker_%d.txt", i+1);
        files.emplace_back(FILENAME);
    }
}

int main(int argc, char* argv[]) {
    double t = 0; // initialize time = 0
    std::ofstream MyFile; // Create file variable
    MyFile.open("people.txt"); // Open text file called "position.txt"
    if (!MyFile.is_open()) // Ensure file created correctly
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFile << "Time, position_x, position_y" << std::endl; // Set column labels

    flecs::world world(argc, argv);

    // Create components inside world
    setupComponents(world);

    // Initialize the walkers
    std::vector<flecs::entity> people;
    std::vector<std::ofstream> files;
    people.reserve(numberOfPeople);
    setupEntities(world, people, files);
    
    // System to add tag to entities dependent on position
    world.system<Position>()
        .each([&](flecs::entity e, Position& pos){
            if (pos.x == -Lx && pos.y == -Ly) {
                e.add<UpperLeftTag>();
            } else if (pos.x == Lx && pos.y == -Ly) {
                e.add<UpperRightTag>();
            } else if (pos.x == -Lx && pos.y == Ly) {
                e.add<LowerLeftTag>();
            } else if (pos.x == Lx && pos.y == Ly) {
                e.add<LowerRightTag>();
            } else if (pos.x == -Lx) {
                e.add<LeftTag>();
            } else if (pos.x == Lx) {
                e.add<RightTag>();
            } else if (pos.y == -Ly) {
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
    for (int i = 0; i < numberOfPeople; ++i) {
        const Position& pos = people[i].get<Position>();
        files[i] << t << ", " << pos.x << ", " << pos.y << std::endl; 
    } 

    while (t<maxTime) {
        t += timeStep;
        world.progress();
        // write data to file
        for (int i = 0; i < numberOfPeople; ++i) {
            const Position& pos = people[i].get<Position>();
            files[i] << t << ", " << pos.x << ", " << pos.y << std::endl; 
        }
    }
    for (int i = 0; i < numberOfPeople; ++i) {
        files[i].close();
    }
}
