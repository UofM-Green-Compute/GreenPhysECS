/*
Oluwole Delano
12/2/26

*** Transition Matrix *** 
 { q_11 | q_12 | q_13 }
 { q_21 | q_22 | q_23 }
 { q_31 | q_32 | q_33 }
   --> where q_nm = q_n<-m = probability to transition from state n to state m
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>

int TIME_STEPS = 50; 

double Q11 = 2.0 / 3.0;
double Q21 = 1.0 / 3.0;
double Q31 = 0.0;
double Q12 = 1.0 / 4.0;
double Q22 = 1.0 / 2.0;
double Q32 = 1.0 / 4.0;
double Q13 = 0.0;
double Q23 = 0.0;
double Q33 = 1.0;

// Initialise State Tags (Note that the sum of qmn for each component must equal 1)
struct state { int s; }; 
struct state1 { double q11, q21, q31; }; 
struct state2 { double q12, q22, q32; }; 
struct state3 { double q13, q23, q33; }; 

int main(int argc, char* argv[]) {

    // *** Programming setup *** 

    // Opening data file 
    std::ofstream MyFile;
    MyFile.open("3-state-markov-chain.txt");
    // Check if file is open
    if (!MyFile.is_open())
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1; 
    }
    MyFile << "Time step | State" << std::endl; 

    std::cout<<"Hello world I'm a 3 state Markov chain"<<std::endl; 

    // Initialise flecs world
    flecs::world world(argc,argv);

    // Creating components
    world.component<state1>(); 
    world.component<state2>(); 
    world.component<state3>(); 

    // Tools for picking random numbers 
    std::mt19937 rng( std::random_device{}()  ) ; 
    std::uniform_real_distribution<double> dist(0,1); 

    // *** Physics ***

    // Set the initial condition
    auto counter = world.entity(); 
    counter.set<state>({1}); 
    counter.set<state1>({Q11, Q21, Q13}); 

    // Change the state of the system
    world.system<state1>()
        .each([&](state1& s1){
            double rand = dist(rng); 
            if(rand < s1.q11){
                std::cout<<"Staying in state 1"<<std::endl; 
            }
            else if( (rand > s1.q11) && (rand < (s1.q11 + s1.q21)) ){
                std::cout<<"Moving to state 2"<<std::endl; 
                counter.remove<state1>(); 
                counter.set<state>({2});
                counter.set<state2>({Q12, Q22, Q32}); 
            }
            else if( (rand > (s1.q11 + s1.q21)) && (rand < (s1.q11 + s1.q21 + s1.q31)) ){ 
                std::cout<<"Moving to state 3"<<std::endl;  
                counter.remove<state1>();
                counter.set<state>({3});
                counter.set<state3>({Q13, Q23, Q33}); 
            }
        }); 
    
    world.system<state2>()
        .each([&](state2& s2){
            double rand = dist(rng); 
            if(rand < s2.q12){
                std::cout<<"Moving to state 1"<<std::endl; 
                counter.remove<state2>(); 
                counter.set<state>({1}); 
                counter.set<state1>({Q11, Q21, Q31});
            }
            else if( (rand > s2.q12) && (rand < (s2.q12 + s2.q22)) ){
                std::cout<<"Staying in state 2"<<std::endl; 
            }
            else if( (rand > (s2.q12 + s2.q22)) && (rand < (s2.q12 + s2.q22 + s2.q32)) ){
                std::cout<<"Moving to state 3"<<std::endl; 
                counter.remove<state2>(); 
                counter.set<state>({3}); 
                counter.set<state3>({Q13, Q23, Q33}); 
            }
        }); 

    world.system<state3>()
        .each([&](state3& s3){
            double rand = dist(rng);
            if(rand < s3.q13){
                std::cout<<"Moving to state 1"<<std::endl; 
                counter.remove<state3>(); 
                counter.set<state>({1}); 
                counter.set<state1>({Q11, Q21, Q31});
            }
            else if( (rand > s3.q13) && (rand < (s3.q13 + s3.q23)) ){
                std::cout<<"Moving to state 2"<<std::endl;
                counter.set<state>({2});
                counter.remove<state3>(); 
                counter.set<state2>({Q12, Q22, Q32});  
            }
            else if( (rand > (s3.q13 + s3.q23)) && (rand < (s3.q13 + s3.q23 + s3.q33)) ){
                std::cout<<"Staying in state 3"<<std::endl; 
            }
            
        }); 

    // Run the world TIME_STEPS times
    for(int i = 0; i < TIME_STEPS; i++){

        MyFile << i << "," << counter.get<state>().s <<std::endl; 
        world.progress(); 

    }

}