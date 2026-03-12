#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 
#include "simulation.h"

// Initial Conditions
int NO_PEOPLE = 100; // Total number of people
int I0 = 10; // Number of initially infected peoples
double timeStep = 0.005; // Set timestep = 0.0001
double maxTime = 5; // Maximum Simulation Time
double beta = 7 / static_cast<double>(NO_PEOPLE); // infection rate
double alpha = 1; // recovery rate

int main(int argc, char* argv[]) {
    simulate(argc, argv, beta, alpha, NO_PEOPLE, I0, timeStep, maxTime);
}