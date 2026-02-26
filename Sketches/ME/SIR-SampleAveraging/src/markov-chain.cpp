/*
Oluwole Delano
12/2/26
Mattias Evans
26/02/26
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

int N = 100; // Total number of people
double t = 0; // Set time = 0
double timeStep = 0.01; // Set timestep = 0.01

// initial populations (n1, n2, n3)
int n1 = N-1; // State 1 (susceptible)
int n2 = 1; // State 2 (infected)
int n3 = 0; // State 3 (recovered)
std::vector<int> population = {N-1, 1, 0}; // nS, nI, NR

// Transition constants
double beta = 7 / static_cast<double>(N); 
double alpha = 1; 

// Initial Transition Probabilties
// Transition probabilities when in S
double Q11 = exp(-beta * population[1] * timeStep);
double Q21 = 1 - Q11;
double Q31 = 0.0;

// Transition probabilities when in I
double Q12 = 0.0;
double Q22 = exp(-alpha*timeStep);
double Q32 = 1 - Q22;

// Transition probabilities when in R
double Q13 = 0;
double Q23 = 0;
double Q33 = 1;

// Initialise State components
// State tells you if your in S, I, or R
struct State { int s; };
// Marko Chain Probabilities. q1 sends you to S, q2 to I, and q3 to R
struct MarkovProbabilities { double q1, q2, q3; }; 

int main(int argc, char* argv[]) {

    // Opening data file 
    std::ofstream MyFileSIR;
    MyFileSIR.open("SIR.txt");
    // Check if file is open
    if (!MyFileSIR.is_open())
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1; 
    }

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    world.component<State>();
    world.component<MarkovProbabilities>(); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(N); 

    // Initialise Susceptible Entities 
    for (int i = 0; i < n1; ++i) { 
        people.push_back( 
            world.entity() 
                .set<State>({1})
                .set<MarkovProbabilities>({0, 0, 0})
        ); 
    } 
    // Initialise the Infected Entity
    for (int i = 0; i < n2; ++i) { 
        people.push_back( 
            world.entity() 
                .set<State>({2})
                .set<MarkovProbabilities>({0, 0, 0})
        );
    }  

    // Update Entity Transition Probabilities
    world.system<State, MarkovProbabilities>()
        .each([&](State& state, MarkovProbabilities&prob){
            if (state.s == 1){
                prob.q1 = Q11;
                prob.q2 = Q21;
                prob.q3 = Q31;
            } else if (state.s == 2) {
                prob.q1 = Q12;
                prob.q2 = Q22;
                prob.q3 = Q32;
            } else if (state.s == 3) {
                prob.q1 = Q13;
                prob.q2 = Q23;
                prob.q3 = Q33;
            }
        });
    
    // Update State
    world.system<State, MarkovProbabilities>()
        .each([&](State& state, MarkovProbabilities&prob){
            double rand = dist(rng);
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
            } else if((rand > (prob.q1 + prob.q2)) && (rand < (prob.q1 + prob.q2 + prob.q3))){
                // transition to state 3 (recovered)
                if (state.s == 1) {
                    population[2] += 1;
                    population[0] -= 1;  
                } else if (state.s == 2) {
                    population[2] += 1;
                    population[1] -= 1;  
                }
                state.s = 3;
            }
        }); 
    
    MyFileSIR << "Time,Susceptible,Infected,Recovered" << std::endl; // Set column labels
    MyFileSIR << t << "," << population[0] << "," << population[1] << "," << 
        population[2] << std::endl; // Initial data
    // Run the world TIME_STEPS times
    while (population[1]>0) {
        world.progress();
        t += timeStep;
        MyFileSIR << t << "," << population[0] << "," << population[1] << "," 
                  << population[2] << std::endl; 
        // Update Transition Probabilties
        // Transition probabilities when in S
        Q11 = exp(-beta * population[1] * timeStep);
        Q21 = 1 - Q11;
        Q31 = 0.0;
        // Transition probabilities when in I
        Q12 = 0.0;
        Q22 = exp(-alpha*timeStep);
        Q32 = 1 - Q22;
        // Transition probabilities when in R
        Q13 = 0;
        Q23 = 0;
        Q33 = 1;
    }
    std::cout<<"RUN TIME: "<<t<<"days"<<std::endl;
}