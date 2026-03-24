/*
Oluwole Delano
Created 24/3/26

*/

#include <flecs.h>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h> 

double a = 0.1;
double b = 0.2;
double psi = M_PI / 2; // In radians 

struct grid_point { double x,y; }; 

void buildLattice(flecs::world world, std::vector<flecs::entity> &p, std::ofstream &MyFile) {

    double x = 0; 
    double y = 0; 

    for(int i = 0; x<= 1; i++){

        x += a * i; 

        for(int j = 0; y <= 1; j++){

            y += b * sin(psi) * j; 

            p.push_back(
            world.entity()
                .set<grid_point>({x,y}) 
            ); 

            MyFile << x << "," << y << ";" ; 
        }

        MyFile << std::endl; 
    
    }
    

}

int main(int argc, char* argv[]) {

    // Create the world 
    flecs::world world;

    // Create vector for plant population
    std::vector<flecs::entity> population; 

    // Opne files 
    std::ofstream MyFile; 
    MyFile.open("Lattice-Grid.txt"); 

    // Call fucntion to build up the lattice 
    buildLattice(world, population, MyFile); 

}

