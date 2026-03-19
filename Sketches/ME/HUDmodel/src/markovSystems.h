#include <flecs.h>
#include <systems.h>
#include <vector>

void updateProbabilities(flecs::world &world, std::vector<int> &cropPopulation, std::vector<int> &sentinelPopulation, 
     std::vector<double> &infectionRates, std::vector<double> &scalings, std::vector<double> &presymptomaticTimes);

void transition(flecs::world &world, std::vector<int> &cropsPopulation, std::vector<int> &sentinelsPopulation);