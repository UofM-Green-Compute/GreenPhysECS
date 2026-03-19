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
    if (state == 1){
        double q11 = exp(-infectionRate * population_vector[1] * step);
        double q21 = 1-q11;        
        probabilities[0] = q11; 
        probabilities[1] = q21; 
    }
    else if (state == 2){
        double q22 = exp(-recoveryRate * step);
        double q12 = 1-q22;
        probabilities[0] = q12; 
        probabilities[1] = q22; 
    }
    return probabilities; 
}

void updateProbabilities(flecs::world & world, std::vector<int> &population, 
    double const &infectionRate, double const &recoveryRate, double const &timeStep) {
    /*
    This system updates the transition probabilities vector for each entity 
    */ 
   world.system<State>()
        .each([&](flecs::entity e, State& state){
            std::vector<double> qnm = calculateProbabilities(population, state.s, infectionRate, 
                recoveryRate, timeStep); 
            e.set<TransitionProbabilities>({qnm});
            //std::cout<<"system 2\n";
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
