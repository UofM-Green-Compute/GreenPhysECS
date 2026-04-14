/*
This program runs for a given ensemble sample with a given detection strategy at 
a given time to check if the detection strategy successfully detected the disease.
If it did, then the detection checker goes true for that index.

This program returns the detection prevelance
*/
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
    double detectionPrevalence = sum/(populationVector[0] + populationVector[1]);
    return detectionPrevalence;
}

double surveille(bool &checker, const vector<int> totalPopulations, const int sentinelSample, 
    const int totalSampling, const int time, filesystem::path filePath1, filesystem::path filePath2) {
    int prevalence = 0; // default return value if no disease is detected
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

    // Opening the file
    vector<vector<int>> cropData = readData(MyFile1);
    vector<vector<int>> sentinelData = readData(MyFile2);

    // try detecting the disease. if that works, set checker = true and record prevalence
    int cropSample = totalSampling-sentinelSample;
    selectIndices(cropShuffledIndices, cropDetectionIndices, cropSample);
    selectIndices(sentinelShuffledIndices, sentinelDetectionIndices, sentinelSample);
    if (cropSample!=0) {
        for (int cropIndex : cropDetectionIndices){
            if (cropData[time+1][cropIndex+1]==3){
                checker = true;
            }
        }
    }
    if (sentinelSample!=0) {
        for (int sentinelIndex : sentinelDetectionIndices) {
            if (sentinelData[time+1][sentinelIndex+1]==3){
                checker = true;
            }
        }
    }
    
    if (checker == true) {
        prevalence = calculatePrevalence(cropData[time+1], sentinelData[time+1], totalPopulations);
    }
    return prevalence;
}
