/*
Oluwole Delano
12/2/26

*** Transition Matrix *** 
 { q_11 | q_12 | q_13 }
 { q_21 | q_22 | q_23 }
 { q_31 | q_32 | q_33 }
   -- where q_nm = q_n<-m = probability to transition from state n to state m
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

int TIME_STEPS = 200; 

// Counters to keep track of how many people are in each state
int NO_PEOPLE = 100000; // Total number of people
int noS = 0; 
int noI = 0; 
int noR = 0; 
// Transition constants
double beta = 1.0 / static_cast<double>(NO_PEOPLE); 
double alpha = 0.05; 

// Transition probability functions 
double calculate_Q21(int nS, int nI, int nR) { return beta * nI; }
double calculate_Q32(int nS, int nI, int nR) { return alpha; }
double Q11 = 1 - calculate_Q21(noS, noI, noR);
double Q22 = 1 - calculate_Q32(noS, noI, noR);
// Transition probabilities
double Q31 = 0.0;
double Q12 = 0.0;
double Q13 = 0;
double Q23 = 0;
double Q33 = 1;

// Initialise State components (Note that the sum of qmn for each component must equal 1)
struct PeopleIndex { int p; }; 
struct state { int s; }; 
struct state1 { double q11, q21, q31; }; // "Susceptible"
struct state2 { double q12, q22, q32; }; // "Infected"
struct state3 { double q13, q23, q33; }; // "Recovered"

int main(int argc, char* argv[]) {

    // Start measuring run time of program
    clock_t t; 
    t = clock(); 

    // *** Programming setup *** 

    // Opening data file 
    std::ofstream MyFileSIR;
    MyFileSIR.open("SIR.txt");
    // Check if file is open
    if (!MyFileSIR.is_open())
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1; 
    }
    MyFileSIR << "Time step | Susceptible (S) | Infected (I) | Recovered (R)" << std::endl; 

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    world.component<PeopleIndex>(); 
    world.component<state>(); 
    world.component<state1>(); 
    world.component<state2>(); 
    world.component<state3>(); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // *** Physics ***

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(2*NO_PEOPLE); 

    // Set the initial condition 
    //  -- Set initial numbers of each population -- 
    noI = 2; 
    noS = NO_PEOPLE - noI; 
    // -- Set the initial transition probabilities -- 
    double Q21 = calculate_Q21(noS, noI, noR); 
    double Q32 = calculate_Q32(noS, noI, noR); 
    double Q11 = 1 - Q21;
    double Q22 = 1 - Q32;
    // -- Initialise entities -- 
    for (int i = 0; i < noS; ++i) { 
        people.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<state>({1})
                .set<state1>({Q11, Q21, Q31})
        ); 
    } 
    for (int i = 0; i < noI; ++i) { 
        people.push_back( 
            world.entity() 
                .set<PeopleIndex>({noS + i})
                .set<state>({2})
                .set<state2>({Q12, Q22, Q32})
        ); 
    }  

    // Update each entity in the world
    world.system<state3, PeopleIndex>()
        .each([&](state3& s3, PeopleIndex& person){
            double rand = dist(rng);
            if(rand < s3.q13){
                // Moving to state 1
                std::cout<<"ERROR: System cannot transition from state 3 to state 1"<<std::endl;
            }
            else if( (rand > s3.q13) && (rand < (s3.q13 + s3.q23)) ){
                // Moving to state 2
                std::cout<<"ERROR: System cannot transition from state 3 to state 2"<<std::endl;
            }
            else if( (rand > (s3.q13 + s3.q23)) && (rand < (s3.q13 + s3.q23 + s3.q33)) ){
                // Staying in state 3
                people[person.p].set<state3>({Q13, Q23, Q33});
            }
        }); 

    world.system<state2, PeopleIndex>()
        .each([&](state2& s2, PeopleIndex& person){
            double rand = dist(rng); 
            if(rand < s2.q12){
                // Moving to state 1
                std::cout<<"ERROR: System cannot transition from state 2 to state 1"<<std::endl;
            }
            else if( (rand > s2.q12) && (rand < (s2.q12 + s2.q22)) ){
                // Staying in state 2
                double q32 = alpha; 
                double q22 = 1 - Q32;
                people[person.p].set<state2>({Q12, q22, q32}); 
            }
            else if( (rand > (s2.q12 + s2.q22)) && (rand < (s2.q12 + s2.q22 + s2.q32)) ){
                // Moving to state 3
                noI -=1; 
                noR += 1; 
                people[person.p].remove<state2>(); 
                people[person.p].set<state>({3}); 
                people[person.p].set<state3>({Q13, Q23, Q33}); 
            }
        }); 

    world.system<state1, PeopleIndex>()
        .each([&](state1& s1, PeopleIndex& person){
            double rand = dist(rng); 
            if(rand < s1.q11){
                // Staying in state 1
                double q21 = beta * noI; 
                double q11 = 1 - q21;
                people[person.p].set<state1>({q11, q21, Q31}); 
            }
            else if( (rand > s1.q11) && (rand < (s1.q11 + s1.q21)) ){
                // Moving to state 2
                noS -= 1; 
                noI += 1; 
                double q32 = alpha; 
                double q22 = 1 - Q32;

                people[person.p].remove<state1>(); 
                people[person.p].set<state>({2});
                people[person.p].set<state2>({Q12, q22, q32}); 
            }
            else if( (rand > (s1.q11 + s1.q21)) && (rand < (s1.q11 + s1.q21 + s1.q31)) ){ 
                // Moving to state 3
                std::cout<<"ERROR: System cannot transition from state 1 to state 3"<<std::endl;
            }

        }); 

    // Run the world TIME_STEPS times
    for(int i = 0; i < TIME_STEPS; i++){

        MyFileSIR << i << "," << noS << "," << noI << "," << noR << std::endl; 
        world.progress(); 

    }

    t = clock() - t; 
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
    
}