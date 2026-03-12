#include <flecs.h>
#include <systems.h>
#include <vector>
#include <random>

// Tools for picking random numbers 
std::mt19937 rng( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> dist(0,1); 

void lksfbj(flecs::world world, flecs::entity &e, std::vector<flecs::entity> &person, 
    int &functionState, std::vector<double> &pVector, std::vector<int> &population_sizes) 
    { 
    double rand = dist(rng); 
    double probability_sum = 0; 
    bool transition = false; 
    for(int i = 1; i <= pVector.size(); i++){
        if (functionState != i) { 
            probability_sum += pVector[i-1]; 
            if ( (rand < probability_sum) ){
                population_sizes[functionState-1] -= 1;
                population_sizes[i-1] += 1; 
                e.set<State>({i});
                break; 
            }
        }
    }
}

