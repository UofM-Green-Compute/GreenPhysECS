#include <flecs.h>
#include <systems.h>
#include <vector>
#include <fstream>

std::vector<double> simulate(int argc, char* argv[], const std::vector<double> &betas, 
                            std::vector<double> &epsilons, std::vector<double> &gammas, 
                            std::vector<int> &totalPopulations, std::vector<int> &U0, 
                            int &sampleSize, int &delta, bool baseline, double radius, std::string filename3);  

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, int initialInfected, int plantType); 

void setupLattice(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, std::ofstream &MyFile); 