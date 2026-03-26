#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;
vector<double> surveille(vector<int> totalPopulations, const int maxTime, const int delta,
    filesystem::path filePath1, filesystem::path filePath2, int sampleSize);