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

// Transition constants
double beta = 10 / static_cast<double>(numberOfPeople); 
double alpha = 0.001; 

// initial populations (n1, n2, n3)
int n2 = 1; // State 2 (infected)
int n1 = numberOfPeople-n2; // State 1 (susceptible)
int n3 = 0; // State 3 (recovered)
std::vector<int> population = {n1, n2, n3}; // where population updates occur
std::vector<std::vector<int>> infectionMatrix; // number of infected people at each point in space

int Lx = 50; // x-direction spatial extent of lattice in units of lattice spacing
int Ly = 50; // y-direction spatial extent of lattice in units of lattice spacing

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
        });

    // update susceptible markov probabilities
    world.system<State, MarkovProbabilities, Position>()
        .kind(markovPhase)
        .each([&](State &state, MarkovProbabilities& prob, Position &pos){
        // find number of infected people on your tile
        if (state.s == 1) {
            int infectedContact = infectionMatrix[pos.x][pos.y];
            prob.q1 = exp(-beta * infectedContact * timeStep);
            prob.q2 = 1-prob.q1;
            prob.q3 = 0;
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
            } else if( (rand > prob.q2) && (rand < (prob.q1 + prob.q2))){
                // transition to state 2 (infected)
                if (state.s == 1) {
                    population[1] += 1;
                    population[0] -= 1;  
                } else if (state.s == 3) {
                    population[1] += 1;
                    population[2] -= 1;  
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
