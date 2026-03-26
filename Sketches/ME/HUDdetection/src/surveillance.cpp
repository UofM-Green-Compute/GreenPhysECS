#include <flecs.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <bits/stdc++.h>

using namespace std;

#include <sstream>
#include <vector>

void selectIndices(vector<int> &shuffledIndices, vector<int> &detectionIndices, int sampleSize) {
    /*
    This function shuffles shuffledIndices, and populates detectionIndices with first sampleSize indices
    */
    auto rngShuffle = default_random_engine {};
    shuffle(begin(shuffledIndices), end(shuffledIndices), rngShuffle);
    for(unsigned int i = 0; i < (unsigned) sampleSize; i++) {
        detectionIndices.push_back(shuffledIndices[i]);
    }
}

vector<int> split(const string &s, char delimiter) {     
    vector<int> tokens;     
    string token;     
    istringstream tokenStream(s);     
    while (getline(tokenStream, token, delimiter)) {      
        tokens.push_back(stoi(token));     
    }     
    return tokens;  
}

vector<vector<int>> readData(ifstream &file){
    vector<string> dataString;
    string str;
    string header;
    if (file >> header) {
    }
    while (file >> str) {
        dataString.push_back(str);
    }
    vector<vector<int>> data;
    for (string rowString : dataString) {
        vector<int> currentRow;
        currentRow = split(rowString, ',');
        data.push_back(currentRow);
    }
    return data;
}

double calculatePrevalence(vector<int> vector1, vector<int>vector2, vector<int> populationVector){
    int undetectedCropCount = count(vector1.begin(), vector1.end(), 2);
    int detectedCropCount = count(vector1.begin(), vector1.end(), 3);
    int undetectedSentinelCount = count(vector2.begin(), vector2.end(), 2);
    int detectedSentinelCount = count(vector2.begin(), vector2.end(), 3);
    double sum = undetectedCropCount + detectedCropCount
               + undetectedSentinelCount + detectedSentinelCount;
    double prevalence = sum/(populationVector[0] + populationVector[1]);
    return prevalence;
}

vector<double> surveille(vector<int> totalPopulations, const int maxTime, const int delta,
    filesystem::path filePath1, filesystem::path filePath2, 
    int sampleSize) {
    ifstream MyFile1(filePath1); // crops
    ifstream MyFile2(filePath2); // crops
    vector<int> cropShuffledIndices; // This vector contains indices of crops which will be checked
    vector<int> sentinelShuffledIndices; // This vector contains indices of crops which will be checked
    vector<int> cropDetectionIndices; // This vector contains indices of crops which will be checked
    vector<int> sentinelDetectionIndices; // This vector contains indices of crops which will be checked
    // fill the shuffled indices vector with unshuffled indices (they will be shuffled later)
    for (int integer = 0; integer < totalPopulations[0]; integer++) {
        cropShuffledIndices.push_back(integer);
    }
    for (int integer = 0; integer < totalPopulations[1]; integer++) {
        sentinelShuffledIndices.push_back(integer);
    }
    int maxNumber = maxTime/delta;

    // Opening the file
    vector<vector<int>> cropData = readData(MyFile1);
    vector<vector<int>> sentinelData = readData(MyFile2);
    
    // detection
    vector<double> diseasePrevalenceVector;
    // loop through each iteration of Nc,Ns
    for (int iteration = 0; iteration<=sampleSize; iteration++){
        vector<int> sampleSizes = {iteration, sampleSize-iteration};
        // for each iteration of Nc,Ns try detecting the disease. if that works break
        // surveillance
        int detectionNumber = 0;
        bool detectOutbreak = false;
        while ((detectionNumber<=maxNumber) && (detectOutbreak == false)) {
            int row = detectionNumber*delta;
            selectIndices(cropShuffledIndices, cropDetectionIndices, sampleSizes[0]);
            selectIndices(sentinelShuffledIndices, sentinelDetectionIndices, sampleSizes[1]);
            if (sampleSizes[0]!=0) {
                for (int cropIndex : cropDetectionIndices){
                    if (cropData[row][cropIndex+1]==3){
                        detectOutbreak = true;
                    }
                }
            }
            if (sampleSizes[1]!=0) {
                for (int sentinelIndex : sentinelDetectionIndices) {
                    if (sentinelData[row][sentinelIndex+1]==3){
                        detectOutbreak = true;
                    }
                }
            }
            detectionNumber +=1;
        }
        if (detectOutbreak == true) {
            double iterationPrevalence = calculatePrevalence(cropData[detectionNumber*delta], 
                sentinelData[detectionNumber*delta], totalPopulations);
            diseasePrevalenceVector.push_back(iterationPrevalence);
        } else {
            // if nothing is detected it will record as 0 and then it knows to remove
            // this data point
            diseasePrevalenceVector.push_back(0);
        }
    }
    return diseasePrevalenceVector;
}
