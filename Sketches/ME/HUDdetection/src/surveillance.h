#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;
double surveille(bool &checker, const vector<int> totalPopulations, 
    const int detectionStrategy, const int totalSampling,
    const int time, filesystem::path filePath1, filesystem::path filePath2);
    