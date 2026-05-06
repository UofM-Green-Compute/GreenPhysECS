#include <flecs.h>
#include <systems.h>
#include <vector>

void updateProbabilities(flecs::world &world, const std::vector<double> &infectionRates, const std::vector<double> &scalings, 
    const std::vector<double> &presymptomaticTimes);

void countNeighbours(flecs::world &world, std::vector<flecs::entity> &crops,
    std::vector<flecs::entity> &sentinels, flecs::entity &Link); 

void transition(flecs::world &world, std::vector<int> &cropNumbers, std::vector<int> &sentinelNumbers);