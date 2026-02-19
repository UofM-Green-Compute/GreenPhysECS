/*
This code runs a random walk simulation for N entites in general.

The entities live on a lattice and have a probability at each time step
of moving or 
*/

#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <cmath>
#include <random>

double timeStep = 0.1; // time step in s
int numberOfSteps = 1000; // number of time steps
double latticeSpacing = 1; // lattice spacing in metres
// walking speed is in ms-1. if the spacing is latticeSpacing metres than after one second
// you expect that a person has made speed/latticeSpacing steps. so the probability of moving has to be
// such that the number of steps per second is 1. Therefore lambda = 1.
double speed = 1; 
double lambda = speed/latticeSpacing;
double pStay = exp(-lambda*timeStep); // probability of staying is an exponential with time
double pMove = 1-pStay; // probability of moving at any time step
int numberOfPeople = 1; // number of people in our system

int Lx = 50; // x-direction spatial extent of lattice in units of lattice spacing
int Ly = 50; // y-direction spatial extent of lattice in units of lattice spacing

struct Position { int x, y; }; // position struct

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

int main(int argc, char* argv[]) {
    std::ofstream MyFile; // Create file variable
    MyFile.open("position.txt"); // Open text file called "position.txt"
    if (!MyFile.is_open()) // Ensure file created correctly
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFile << "Time, position_x, position_y" << std::endl; // Set column labels

    flecs::world world(argc, argv);

    // Creates Phases which tell the program in which order to run the systems
    // This Phase sees if person is on edge, corner etc.
    flecs::entity findTagPhase = world.entity()
        .add(flecs::Phase); 
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

    // Create Nodes
    for (int index = 0; index <= numberOfPeople-1; ++index) {
        people.push_back(
            world.entity()
                .set<Position>({generatePosition(Lx), generatePosition(Ly)}) //random position on lattice
            );
    }

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
        });
    
    world.system<Position>()
        .with<LowerLeftTag>()
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
        });
    
    const Position& initialPos = people[0].get<Position>();
    MyFile << 0 << ", " << initialPos.x << ", " << initialPos.y << std::endl; 
    for (int i = 1; i <= numberOfSteps; i++) {
        
        world.progress();
        
        std::cout << i << "\n";
        std::cout << "----\n";
        
        const Position& pos = people[0].get<Position>();
        MyFile << i*timeStep << ", " << pos.x << ", " << pos.y << std::endl; 
    }
}
