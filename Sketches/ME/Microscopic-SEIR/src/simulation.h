#include <flecs.h>
#include <systems.h>
#include <vector>

int simulate(int argc, char* argv[], const double infectionRate, const double latencyRate,
             const double recoveryRate, const int totalPopulation, const int initialInfected,
             const double step, const double totalTime, std::string filename);
