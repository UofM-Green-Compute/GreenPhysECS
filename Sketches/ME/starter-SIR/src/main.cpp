/*
This code runs a simulation of an epidemic in
a population of N people.

There are 3 categories:
-Susceptible (S) with nS people
-Infected (I) with nI people
-Recovered (R) with nR people

nS + nI + nR = N is a constant

This code uses the Gillespie Algorithm for simulating
continuous time processes:
1) set t=0
2) Determine possible states and transition rates W_i
    - lambda = sum_i W_i
3) Draw a random tau using the folowwing process
    - Select a random number r from a uniform distribution 
      over (0,1)
    - Set tau = -ln(r)/lambda
    - Increment time t += tau
4) Execute one of the possible events with probability
   W_i/lambda and change state accordingly
5) Go back to step 2

The program is currently set to finish when I=0

For the SIR model:
- W1 = beta * nS * nI
- W2 = gamma * nI
- lambda = nI * (gamma + beta * nS)
- tau = -ln(r) / (nI * (gamma + beta * nS))
*/
#include <iostream>
#include <fstream> 
#include <vector>
#include <cmath>
#include <random>

int N = 30;
double t = 0; // Set time = 0
double W1; // Transition rate for infection
double W2; // Transition rate for recovery
double lambda; // Decay rate used to find event time


// initial (nS, nI, nR)
std::vector<int> population = {N-1, 1, 0};

// Infection Rate in number of people infected per day per 
// infected person
double Beta = 5/static_cast<double>(N);

// Recovery Rate in number of people recovered per day per
// infected person
double Gamma = 1;

// This function is used to generate a time when an event happens
double generateTau(double generatingLambda)
{
    double tau;
    std::mt19937 rngTAU(std::random_device{}()); // random number generator
    std::uniform_real_distribution<double> ZeroOneTAU(0, 1); // uniform distribution
    double rTAU = ZeroOneTAU(rngTAU); // random number from a uniform distribution
    tau = -log(rTAU)/generatingLambda; // generates random event time
    return tau;
}

int main() {

    std::ofstream MyFile; // Create file variable
    MyFile.open("SIR.txt"); // Open text file called "SIR.txt"
    if (!MyFile.is_open()) // Ensure file created correctly
    {
        std::cout<<"Error in creating file"<<std::endl; 
        return 1;
    }
    MyFile << "Time (days), nS, nI, nR" << std::endl; // Set column labels

    while (population[1]>0) {
        W1 = Beta * population[0] * population[1]; // Transition rate (infection)
        W2 = Gamma * population[1]; // Transition rate (Recovery)
        lambda = W1 + W2; // Overall decay rate for an event to happen
        t += generateTau(lambda); //increase time by tau
        std::mt19937 rngEVENT(std::random_device{}()); // random number generator
        std::uniform_real_distribution<double> ZeroOneEVENT(0, 1); // uniform distribution
        double rEVENT = ZeroOneEVENT(rngEVENT); // random number from a uniform distribution
        if (rEVENT < W1/lambda) {
            // Event 1 happens: a susceptible person becomes infected
            population[0] -= 1;
            population[1] += 1;
        } else {
            // Event 2 happens: an infected person becomes recovered
            population[1] -= 1;
            population[2] += 1;
        }
        std::cout << t << ", " << population[0] << ", " << population[1] << "," << 
        population[2] <<"\n";
        MyFile << t << ", " << population[0] << ", " << population[1] << "," << 
        population[2] << std::endl;
    }
    MyFile.close();
}