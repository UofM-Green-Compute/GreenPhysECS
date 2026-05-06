#include <flecs.h>
#include <systems.h>
#include <vector>

void updateProbabilities(flecs::world & world, std::vector<int> &population, 
    double const &infectionRate, double const &recoveryRate, double const &timeStep);

void transition(flecs::world &world, std::vector<int> &population);