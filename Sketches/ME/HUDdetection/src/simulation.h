#include <vector>

using namespace std;

std::vector<double> simulate(int argc, char* argv[], const vector<double> &betas, vector<double> &epsilons, 
    vector<double> &gammas, vector<int> &totalPopulations, vector<int> &U0, int &sampleSize, int &delta, bool baseline);