#include <flecs.h>
#include <systems.h>
#include <vector>
#include <fstream>

int simulate(int argc, char* argv[], const std::vector<double> betas, std::vector<double> epsilons, 
    std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int delta, const std::vector<int> sampleSizes, const int maxTime, std::string filename1, 
    std::string filename2);