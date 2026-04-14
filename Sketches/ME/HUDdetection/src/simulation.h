std::vector<double> simulate(int argc, char* argv[], const std::vector<double> betas, std::vector<double> epsilons, 
    std::vector<double> gammas, std::vector<int> totalPopulations, std::vector<int> U0,
    const int maxTime, std::vector<bool> detectionChecker, int sampleSize, int delta,
    std::filesystem::path filePath1, std::filesystem::path filePath2);