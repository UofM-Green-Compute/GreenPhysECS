#include <flecs.h>
#include <systems.h>
#include <vector>
#include <fstream>

int simulate(int argc, char* argv[], const std::vector<double> betas, const std::vector<double> epsilons, 
    const std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int maxTime, std::string filename1, std::string filename2, double radius);
