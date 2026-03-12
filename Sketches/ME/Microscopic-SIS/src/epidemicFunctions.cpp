#include <flecs.h>
#include <systems.h>
#include <vector>
#include <random>
#include "epidemicFunctions.h"

// Tools for picking random numbers 
std::mt19937 rng( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> dist(0,1); 

struct State { int s; };

void transition(flecs::world world, flecs::entity &e, 
    int &functionState, std::vector<double> &probabilities, std::vector<int> &populationSizes) {
    /*
    This decides which new state the entity should transition to. This depends only on its
    probability vector: probabilities[i] is the probabilities of transition to state s = i

    When the transition happens, the populationSizes vector is updated to reflect the new
    population state
    */ 
    double rand = dist(rng); 
    double probability_sum = 0; 
    bool transition = false; 
    for(int i = 1; i <= probabilities.size(); i++){
        if (functionState != i) { 
            probability_sum += probabilities[i-1]; 
            if ( (rand < probability_sum) ){
                populationSizes[functionState-1] -= 1;
                populationSizes[i-1] += 1; 
                e.set<State>({i});
                break; 
            }
        }
    }
}


