#include <flecs.h>
#include <systems.h>
#include <vector>
#include <fstream>

std::vector<std::vector<int>> simulate(int argc, char* argv[], const std::vector<double> betas, const std::vector<double> epsilons, 
    const std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int maxTime, double radius);