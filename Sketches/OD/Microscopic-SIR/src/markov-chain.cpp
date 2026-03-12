/*
Oluwole Delano
12/2/26

*** Transition Matrix *** 
 { q_11 | q_12 | q_13 }
 { q_21 | q_22 | q_23 }
 { q_31 | q_32 | q_33 }
   -- where q_nm = q_n<-m = probability to transition from state n to state m
*/

/* 

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

int TIME_STEPS = 200; 
int NO_RUNS_I = 10; 
int NO_RUNS_J = 10; 
int NO_RUNS = NO_RUNS_I * NO_RUNS_J; 
int run_i = 0; // Track grid point in S
int run_j = 0; // Track grid point in I

// Counters to keep track of how many people are in each state
int NO_PEOPLE = 100; // Total number of people
int noS = 0;         // Initialise the number of susceptible people
int noI = 0;         // Initialise the number of infected people
int noR = 0;         // Initialise the number of recovered people
// Transition constants
double beta = 1.0 / static_cast<double>(NO_PEOPLE); 
double alpha = 0.05; 

// Transition probabilities
// -- where q_nm = q_n<-m = probability to transition from state n to state m
double Q21 = beta * noI; 
double Q11 = 1 - Q21;
double Q31 = 0.0;
double Q12 = 0.0;
double Q32 = alpha; 
double Q22 = 1 - Q32;
double Q13 = 0;
double Q23 = 0;
double Q33 = 1;

// Initialise State components (Note that the sum of qnm for each component must equal 1)
struct PeopleIndex { int p; }; 
struct state { int s; };                 // Track current state of entity
struct state1 { double q11, q21, q31; }; // Transition probabilities if s=1 "Susceptible"
struct state2 { double q12, q22, q32; }; // Transition probabilities if s=2 "Infected"
struct state3 { double q13, q23, q33; }; // Transition probabilities if s=3 "Recovered"

// Function to set the initial condition 
void initial_condition(flecs::world world, std::vector<flecs::entity> &p){
    // -- Set initial numbers of each population -- 
    noS = run_i; 
    noI = run_j; 
    noR = NO_PEOPLE - noS - noI; 
    // -- Set the initial transition probabilities -- 
    Q21 = beta * noI; 
    Q11 = 1 - Q21;
    // -- Initialise entities -- 
    for (int i = 0; i < noS; ++i) { 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<state>({1})
                .set<state1>({Q11, Q21, Q31})
        ); 
    } 
    for (int i = 0; i < noI; ++i) { 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({noS + i})
                .set<state>({2})
                .set<state2>({Q12, Q22, Q32})
        ); 
    }  
    for (int i = 0; i < noR; ++i) { 
        p.push_back( 
            world.entity() 
                .set<PeopleIndex>({noS + noI + i})
                .set<state>({3})
                .set<state3>({Q13, Q23, Q33})
        ); 
    }  
}

int main(int argc, char* argv[]) {

    // Start measuring run time of program
    clock_t t; 
    t = clock(); 

    // ******** Programming setup ******** 

    // --- File handling --- 

    // SIR

    // Create files in a for loop
    std::vector<std::vector<std::string>> fileNames; 
    std::ofstream MyFileSIR;
    for(int i = 0; i < NO_RUNS_I; i++){
        fileNames.push_back({}); 
        for(int j = 0;  j < NO_RUNS_J; j++){
            if(i + j < 10){
                // Create a file called SIR_"j".txt
                char file_name[50]; 
                sprintf(file_name, "SIR_%d-%d.txt", i, j); 
                fileNames[i].push_back(file_name); 

                MyFileSIR.open(fileNames[i][j]);
                // Check if file is open
                if (!MyFileSIR.is_open())
                {
                    std::cout<<"Error in creating file"<<std::endl; 
                    return 1; 
                }
                MyFileSIR << "Time,Susceptible,Infected,Recovered" << std::endl; 
                MyFileSIR.close(); 
            }
        }
    }

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

    // ******** Physics ********

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(2*NO_PEOPLE); 

    // Evolvolution for entities currently in state 3
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

    // Evolvolution for entities currently in state 2
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
                double q22 = 1 - q32;
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

    // Evolvolution for entities currently in state 1
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
                double q22 = 1 - q32;

                people[person.p].remove<state1>(); 
                people[person.p].set<state>({2});
                people[person.p].set<state2>({Q12, q22, q32}); 
            }
            else if( (rand > (s1.q11 + s1.q21)) && (rand < (s1.q11 + s1.q21 + s1.q31)) ){ 
                // Moving to state 3
                std::cout<<"ERROR: System cannot transition from state 1 to state 3"<<std::endl;
            }
        }); 

    // Run the world "NO_RUNS" times. Generate "NO_RUNS" text files each with a 
    // different initial conditions and total population sizes
    for(int i = 0; i < NO_RUNS_I; i++){
        for(int j = 0; j < NO_RUNS_J; j++){
            if(i + j < 10){
                // Open data file 
                MyFileSIR.open(fileNames[i][j]);
                // Check if file is open
                if (!MyFileSIR.is_open())
                {
                    std::cout<<"Error in opening file"<<std::endl; 
                    return 1; 
                }
                // Run the world TIME_STEPS times
                for(int k = 0; k < TIME_STEPS; k++){
                    MyFileSIR << k << "," << noS << "," << noI << "," << noR << std::endl; 
                    world.progress(); 
                }
                // Reset the initial condition for the next run
                people.clear(); 
                people.reserve(2*NO_PEOPLE);
                initial_condition(world, people); 
                // Close file
                MyFileSIR.close();
            }
            run_j += 1; 
        }
        run_j = 0; 
        run_i += 1; 
    }

    t = clock() - t; 
    double time_taken = ((double)t) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
    
}

*/