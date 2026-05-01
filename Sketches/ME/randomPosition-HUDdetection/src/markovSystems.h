#include <flecs.h>
#include <systems.h>
#include <vector>

void updateProbabilities(flecs::world &world, const std::vector<double> &infectionRates, const std::vector<double> &scalings, 
    const std::vector<double> &presymptomaticTimes);

void transition(flecs::world &world, std::vector<int> &cropsPopulation, std::vector<int> &sentinelsPopulation);

void updateGraph(flecs::world &world, std::vector<flecs::entity> &crops, std::vector<flecs::entity> &sentinels,
                 flecs::entity &hLink, flecs::entity &uLink, flecs::entity &dLink);