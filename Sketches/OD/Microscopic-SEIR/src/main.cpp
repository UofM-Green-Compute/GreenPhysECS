/*
Oluwole Delano
26/2/26

*** Transition Matrix *** 
 { q_11 | q_12 | q_13 | q_14 }
 { q_21 | q_22 | q_23 | q_24 }
 { q_31 | q_32 | q_33 | q_34}
 { q_41 | q_42 | q_43 | q_44 }
   -- where q_nm = q_n<-m = probability to transition from state m to state n
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

int TIME_STEPS = 200; 

// Counters to keep track of how many people are in each state
int NO_PEOPLE = 100; // Total number of people
int noS = 0; 
int noE = 0; 
int noI = 0; 
int noR = 0; 
// Transition constants
double a = 0.05; 
double b = 0.1 / static_cast<double>(NO_PEOPLE); 
double c = 0.3;

// Transition probabilities
    // State 1 to some other state
double Q21 = b * noI;
double Q11 = 1 - Q21;
double Q31 = 0.0;
double Q41 = 0.0;
    // State 2 to some other state 
double Q12 = 0.0;
double Q32 = c;
double Q22 = 1 - Q32;
double Q42 = 0.0;
    // State 3 to some other state
double Q13 = 0.0;
double Q23 = 0.0;
double Q43 = a;
double Q33 = 1 - Q43;
    // State 4 to some other state
double Q14 = 0.0;
double Q24 = 0.0;
double Q34 = 0.0;
double Q44 = 1.0;

// Initialise state components (Note that the sum over m of qmn must equal 1 for each n = 1,2,3,4)
struct PeopleIndex { int p; }; 
struct state { int s; }; // Track state entity is in
struct state1 { double q11, q21, q31, q41; }; // "State 1 - Susceptible"
struct state2 { double q12, q22, q32, q42; }; // "State 2 - Exposed"
struct state3 { double q13, q23, q33, q43; }; // "State 3 - Infected"
struct state4 { double q14, q24, q34, q44; }; // "State 4 - Recovered"

int main(int argc, char* argv[]) {

    // Start measuring run time of program
    clock_t t; 
    t = clock(); 

    // *** Programming setup *** 

    // Opening data file 
    std::ofstream MyFileSEIR;
    MyFileSEIR.open("SEIR.txt");
    // Check if file is open
    if (!MyFileSEIR.is_open())
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1; 
    }
    MyFileSEIR << "Time,Susceptible,Exposed,Infected,Recovered" << std::endl; 

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    world.component<PeopleIndex>(); 
    world.component<state>(); 
    world.component<state1>(); 
    world.component<state2>(); 
    world.component<state3>(); 
    world.component<state4>(); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // *** Physics ***

    // Reserve space for people
    std::vector<flecs::entity> people; 
    people.reserve(2*NO_PEOPLE); 

    // Set the initial condition 
    //  -- Set initial numbers of each population -- 
    noI = 1; 
    noS = NO_PEOPLE - noI; 
    // -- Set the initial transition probabilities -- 
    double Q21 = b * noI; 
    double Q32 = c; 
    double Q43 = a; 
    double Q11 = 1 - Q21;
    double Q22 = 1 - Q32;
    double Q33 = 1 - Q43;
    // -- Initialise entities -- 
        // Initially susceptible 
    for (int i = 0; i < noS; ++i) { 
        people.push_back( 
            world.entity() 
                .set<PeopleIndex>({i})
                .set<state>({1})
                .set<state1>({Q11, Q21, Q31, Q41})
        ); 
    } 
        // Initially infected
    for (int i = 0; i < noI; ++i) { 
        people.push_back( 
            world.entity() 
                .set<PeopleIndex>({noS + i})
                .set<state>({3})
                .set<state3>({Q13, Q23, Q33, Q43})
        ); 
    }  

    // ** Systems ** 

    world.system<state4, PeopleIndex>()
        .each([&](state4& s4, PeopleIndex& person){
            double rand = dist(rng);
            if(rand < s4.q14){
                // Moving to state 1
                std::cout<<"ERROR: System cannot transition from state 4 to state 1"<<std::endl;
            }
            else if( (rand > s4.q14) && (rand < (s4.q14 + s4.q24)) ){
                // Moving to state 2
                std::cout<<"ERROR: System cannot transition from state 4 to state 2"<<std::endl;
            }
            else if( (rand > (s4.q14 + s4.q24)) && (rand < (s4.q14 + s4.q24 + s4.q34)) ){
                // Moving to state 3
                std::cout<<"ERROR: System cannot transition from state 4 to state 3"<<std::endl;
            }
            else if( (rand > (s4.q14 + s4.q24 + s4.q34)) && (rand < (s4.q14 + s4.q24 + s4.q34 + s4.q44)) ){
                // Staying in state 4
                people[person.p].set<state4>({Q14, Q24, Q34, Q44});
            }
        }); 

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
                people[person.p].set<state3>({Q13, Q23, Q33, Q43});
            }
            else if( (rand > (s3.q13 + s3.q23 + s3.q33)) && (rand < (s3.q13 + s3.q23 + s3.q33 + s3.q43)) ){
                // Moving to state 4
                noI -=1; 
                noR += 1; 
                people[person.p].remove<state3>(); 
                people[person.p].set<state>({4}); 
                people[person.p].set<state4>({Q14, Q24, Q34, Q44});
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
                double q32 = c; // AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
                double q22 = 1 - Q32;
                people[person.p].set<state2>({Q12, q22, q32, Q42}); 
            }
            else if( (rand > (s2.q12 + s2.q22)) && (rand < (s2.q12 + s2.q22 + s2.q32)) ){
                // Moving to state 3
                noE -=1; 
                noI += 1; 
                people[person.p].remove<state2>(); 
                people[person.p].set<state>({3}); 
                people[person.p].set<state3>({Q13, Q23, Q33, Q43}); 
            }
            else if( (rand > (s2.q12 + s2.q22 + s2.q32)) && (rand < (s2.q12 + s2.q22 + s2.q32 + s2.q42)) ){
                // Moving to state 4
                std::cout<<"ERROR: System cannot transition from state 2 to state 4"<<std::endl;
            }
        }); 

    world.system<state1, PeopleIndex>()
        .each([&](state1& s1, PeopleIndex& person){
            double rand = dist(rng); 
            if(rand < s1.q11){
                // Staying in state 1
                double q21 = b * noI; 
                double q11 = 1 - q21;
                people[person.p].set<state1>({q11, q21, Q31, Q41}); 
            }
            else if( (rand > s1.q11) && (rand < (s1.q11 + s1.q21)) ){
                // Moving to state 2
                noS -= 1; 
                noE += 1; 
                double q32 = a; 
                double q22 = 1 - Q32;

                people[person.p].remove<state1>(); 
                people[person.p].set<state>({2});
                people[person.p].set<state2>({Q12, q22, q32, Q42}); 
            }
            else if( (rand > (s1.q11 + s1.q21)) && (rand < (s1.q11 + s1.q21 + s1.q31)) ){ 
                // Moving to state 3
                std::cout<<"ERROR: System cannot transition from state 1 to state 3"<<std::endl;
            }
            else if( (rand > (s1.q11 + s1.q21)) && (rand < (s1.q11 + s1.q21 + s1.q31 + s1.q41)) ){
                // Moving to state 4
                std::cout<<"ERROR: System cannot transition from state 1 to state 4"<<std::endl;
            }
        }); 

    // Run the world TIME_STEPS times
    for(int i = 0; i < TIME_STEPS; i++){

        MyFileSEIR << i << "," << noS << "," << noE << "," << noI << "," << noR << std::endl; 
        world.progress(); 

    }

    t = clock() - t; 
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
    
}