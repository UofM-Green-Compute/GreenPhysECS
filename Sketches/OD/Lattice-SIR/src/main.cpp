/*
Oluwole Delano
Created 24/3/26
*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

double X_LIM = 1;
double Y_LIM = 1;

double a = 0.1;
double b = 0.2;
double psi = M_PI / 4; // Allowed values: 0 <= psi <= PI (radians)

struct grid_point { double x,y; }; 
struct PeopleIndex { int p; }; 
struct State { int s; }; // Track current state of entity

void buildLattice(flecs::world world, std::vector<flecs::entity> &p, std::ofstream &MyFile) {

    double x = 0; 
    double y = 0; 

    int i_lim = std::floor(X_LIM / a);
    int j_lim = std::floor(Y_LIM / b);

    int j = 0; 
    int no_a = 0; 
    while(y+a <= Y_LIM){
        y = j * b * sin(psi) ; 
        if(psi > M_PI / 2){ x = j * b * -cos(psi); }
        else{x = j * b * cos(psi); }
        no_a = std::floor(x/a); 
        x = x - no_a * a; 

        while(x+a <= X_LIM){
            x += a; 
            MyFile << x << "," << y << "|" ; 
            // Create an entity corresponding to this grid point
            p.push_back(
                world.entity()
                    .set<grid_point>({x,y})
            ); 
        }
        MyFile << std::endl; 
        j+=1; 
    }
    
}

void setupEntities(flecs::world world, std::vector<flecs::entity> &p, int totalPopulation, 
    int initialInfected, int initialRecovered){
    for (int i = 0; i < totalPopulation-initialInfected-initialRecovered; ++i) { 
        p[i].set<PeopleIndex>({i}); 
        p[i].set<State>({1}); 
    } 
    for (int i = 0; i < initialInfected; ++i) { 
        p[i].set<PeopleIndex>({totalPopulation-initialInfected-initialRecovered + i});
        p[i].set<State>({2});
    }
    for (int i = 0; i < initialRecovered; ++i) { 
        p[i].set<PeopleIndex>({totalPopulation-initialRecovered + i});
        p[i].set<State>({3});
    }
}

int main(int argc, char* argv[]) {

    // Create the world 
    flecs::world world;

    // Create vector for plant population
    std::vector<flecs::entity> population; 

    // Open files 
    std::ofstream MyFile; 
    MyFile.open("Lattice-Grid.txt"); 

    // Call fucntion to build up the lattice 
    buildLattice(world, population, MyFile); 

}

