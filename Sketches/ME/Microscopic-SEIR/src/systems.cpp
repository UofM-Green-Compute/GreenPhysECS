// systems.cpp
#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <vector>
#include <random>

// Tools for picking random numbers 
std::mt19937 randomNumberGeneratorEpidemics( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> distributionEpidemics(0,1); 

struct PeopleIndex { int p; }; 
struct State { int s; }; // Track current state of entity
struct TransitionProbabilities { std::vector<double> qnm; };

void updateProbabilities(flecs::world & world, std::vector<int> &population, 
    double const &infectionRate, double const &latencyRate, double const &recoveryRate, double const &timeStep) {
    /*
    This system updates the transition probabilities vector for each entity 
    */ 
   world.system<State, TransitionProbabilities>()
        .each([&](State& state, TransitionProbabilities &probabilities){
            if (state.s == 1){
                double q11 = exp(- infectionRate * population[2] * timeStep);
                double q21 = 1-q11;
                double q31 = 0;
                double q41 = 0;
                probabilities.qnm[0] = q11;
                probabilities.qnm[1] = q21;
                probabilities.qnm[2] = q31;
                probabilities.qnm[3] = q41;
            }
            else if (state.s == 2){
                double q12 = 0;
                double q22 = exp(- latencyRate * timeStep);
                double q32 = 1-q22;
                double q42 = 0;
                probabilities.qnm[0] = q12;
                probabilities.qnm[1] = q22;
                probabilities.qnm[2] = q32;
                probabilities.qnm[3] = q42;
            }
            else if (state.s == 3){
                double q13 = 0;
                double q23 = 0;
                double q33 = exp(- recoveryRate* timeStep);
                double q43 = 1-q33;
                probabilities.qnm[0] = q13;
                probabilities.qnm[1] = q23;
                probabilities.qnm[2] = q33;
                probabilities.qnm[3] = q43;
            }
            else if (state.s == 4){
                double q14 = 0;
                double q24 = 0;
                double q34 = 0;
                double q44 = 1;
                probabilities.qnm[0] = q14;
                probabilities.qnm[1] = q24;
                probabilities.qnm[2] = q34;
                probabilities.qnm[3] = q44;
            }
        }); 
}

void transition(flecs::world &world, std::vector<int> &population) {
    /*
    This decides which new state the entity should transition to. This depends only on its
    probability vector: p_vector[i] is the probabilities of transition to state s = i

    When the transition happens, the populationSizes vector is updated to reflect the new
    population state
    */ 
    world.system<State, TransitionProbabilities>()
        .each([&](State& state, TransitionProbabilities& p_vector){
            double rand = distributionEpidemics(randomNumberGeneratorEpidemics); 
            double probability_sum = 0;
            for(int i = 1; i <= (int) p_vector.qnm.size(); i++){
                probability_sum += p_vector.qnm[i-1]; 
                if ((rand < probability_sum) ){
                    population[state.s-1] -= 1;
                    population[i-1] += 1; 
                    state.s = i;
                    break; 
                }
            }
            //std::cout<<"system 1\n";
        }); 
    }
