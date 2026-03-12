#include <flecs.h>
#include <systems.h>
#include <vector>

void transition(flecs::entity &e, int &functionState, 
    std::vector<double> &probabilities, std::vector<int> &populationSizes);
