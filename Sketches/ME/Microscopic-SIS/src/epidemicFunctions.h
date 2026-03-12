#include <flecs.h>
#include <systems.h>
#include <vector>

std::vector<double> calculateProbabilities(std::vector<int> population_vector, int state, 
    const double infectionRate, const double recoveryRate, const int NoS);

void transition(flecs::world world, flecs::entity &e, int &functionState, 
    std::vector<double> &probabilities, std::vector<int> &populationSizes);
