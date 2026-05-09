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

std::vector<double> calculateProbabilities(std::vector<int> population_vector, int state,
    const double infectionRate, const double recoveryRate, const double step){
    std::vector<double> probabilities = {0, 0}; 
    
    return probabilities; 
}

void updateProbabilities(flecs::world & world, std::vector<int> &population, 
    double const &infectionRate, double const &recoveryRate, double const &timeStep) {
    /*
    This system updates the transition probabilities vector for each entity 
    */ 
   world.system<State, TransitionProbabilities>()
        .each([&](State& state, TransitionProbabilities &probabilities){
            if (state.s == 1){
                double q11 = exp(-infectionRate * population[1] * timeStep);
                double q21 = 1-q11;        
                probabilities.qnm[0] = q11; 
                probabilities.qnm[1] = q21; 
            }
            else if (state.s == 2){
                double q22 = exp(-recoveryRate * timeStep);
                double q12 = 1-q22;
                probabilities.qnm[0] = q12; 
                probabilities.qnm[1] = q22; 
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
        .each([&](flecs::entity e, State& state, TransitionProbabilities& p_vector){
            double rand = distributionEpidemics(randomNumberGeneratorEpidemics); 
            double probability_sum = 0;
            for(int i = 1; i <= (int) p_vector.qnm.size(); i++){
                if (state.s != i) { 
                    probability_sum += p_vector.qnm[i-1]; 
                    if ((rand < probability_sum) ){
                        population[state.s-1] -= 1;
                        population[i-1] += 1; 
                        e.set<State>({i});
                        break; 
                    }
                }
            }
            //std::cout<<"system 1\n";
        }); 
    }
