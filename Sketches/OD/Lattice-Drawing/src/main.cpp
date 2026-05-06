// main.cpp
#include <iostream>
#include <fstream>
#include <time.h> 
#include <cmath>
#include <vector>

// Constants for creating the lattice
double X_LIM = 1;
double Y_LIM = 1;
double C = 1; // Ratio A / B
double PSI = 2 * M_PI / 3; // Allowed values: 0 <= PSI <= PI (radians)
double epsilon = 1 * pow(10,-12); 
double RADIUS = 0.1 + epsilon; 

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {100, 5}; // total number of plants {crops,sentinels}

// Recursive function to setup the lattice 
std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename, std::string filename2, int no_columns){
    
    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename);
    std::ofstream MyFile2; 
    MyFile2.open(filename2);

    // Create a vector to store the lattice positions
    std::vector<std::vector<double>> GridPoints; 

    // Calculate lattice parameters
    double a = X_LIM / (double(no_columns)-1); 
    double b = C * a; 

    // Define terms for later use 
    int backtrack = 0; 
    double x = 0; 
    double y = 0; 

    int count = 0; 
    int j = 0; 
    while ( (y >= 0) && (y <= Y_LIM) ){

        // Find a valid x position for a row 
        if(PSI > M_PI / 2) { x = j * b * -cos(PSI); }
        else{ x = j * b * cos(PSI); } 

        // Set the value of y
        y = j * b * sin(PSI);

        // Backtrack to the first points within the limits on that row
        while (x>=-epsilon) {
            x-=a;
        }
        x+=a;

        // Loop through values of x on that row and store data
        while( (x >= -epsilon) && x <= (X_LIM) && (count < totalPopulation) ){

            MyFile << x << "," << y << "|" ;  
            MyFile2 << "\\node (" << count << ") " << "at (" << x << "," << y << ") {};" << std::endl;
            GridPoints.push_back({x,y}); 

            count += 1; 
            if( x > (X_LIM-a) ) { break; }
            x+=a; 

        }

        MyFile << std::endl; 
        if( y > (Y_LIM-(b * sin(PSI))) ) { break; }
        j+=1; 

    }

    MyFile.close(); 
    MyFile2.close(); 

    // Base case 
    if(count == totalPopulation){ return GridPoints; } 

    // Recursive call 
    return setupLattice(totalPopulation, filename, filename2, no_columns+1); 
}

// Recursive function to setup the lattice 
std::vector<double> latticeConstants(int totalPopulation, std::string filename, std::string filename2, int no_columns){
    
    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename);
    std::ofstream MyFile2; 
    MyFile2.open(filename2);

    // Calculate lattice parameters
    double a = X_LIM / (double(no_columns)-1); 
    double b = C * a; 

    // Create a vector to store the lattice positions
    std::vector<double> consts = {a,b}; 

    // Define terms for later use 
    int backtrack = 0; 
    double x = 0; 
    double y = 0; 

    int count = 0; 
    int j = 0; 
    while ( (y >= 0) && (y <= Y_LIM) ){

        // Find a valid x position for a row 
        if(PSI > M_PI / 2) { x = j * b * -cos(PSI); }
        else{ x = j * b * cos(PSI); } 

        // Set the value of y
        y = j * b * sin(PSI);

        // Backtrack to the first points within the limits on that row
        while (x>=-epsilon) {
            x-=a;
        }
        x+=a;

        // Loop through values of x on that row and store data
        while( (x >= -epsilon) && x <= (X_LIM) && (count < totalPopulation) ){

            count += 1; 
            if( x > (X_LIM-a) ) { break; }
            x+=a; 

        }

        MyFile << std::endl; 
        if( y > (Y_LIM-(b * sin(PSI))) ) { break; }
        j+=1; 

    }

    MyFile.close(); 
    MyFile2.close(); 

    // Base case 
    if(count == totalPopulation){ return consts; } 

    // Recursive call 
    return latticeConstants(totalPopulation, filename, filename2, no_columns+1); 
}

// Modify lattice
std::vector<std::vector<double>> modifiedLattice(std::vector<std::vector<double>> currentGrid, int totalPopulation, std::string filename, std::string filename2, int no_columns) {
    double max_y = 0; 
    double diff = 0; 
    
    // Find the maximum y value 
    for(int i = 0; i < currentGrid.size(); i++){
        if(currentGrid[i][1] > max_y) { max_y = currentGrid[i][1]; }
    }

    // 
    diff = Y_LIM - max_y; 

    std::cout<<"Okay "<<latticeConstants(totalPopulation, filename, filename2, no_columns)[0]<<std::endl; 
    std::cout<<"Okay 2 "<<latticeConstants(totalPopulation, filename, filename2, no_columns)[1]<<std::endl; 
}

// Recursive function to setup the lattice 
std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename, std::string filename2, double a){
    
    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename);
    std::ofstream MyFile2; 
    MyFile2.open(filename2);

    // Create a vector to store the lattice positions
    std::vector<std::vector<double>> GridPoints; 

    // Calculate lattice parameters
    double b = C * a; 
    double midpoint = 0; 

    // Define terms for later use 
    int backtrack = 0; 
    double x = 0; 
    double y = 0; 

    int count = 0; 
    int j = 0; 
    while ( (y >= 0) && (y <= Y_LIM) ){

        // Find a valid x position for a row 
        if(PSI > M_PI / 2) { x = j * b * -cos(PSI); }
        else{ x = j * b * cos(PSI); } 

        // Set the value of y
        y = j * b * sin(PSI);

        // Backtrack to the first points within the limits on that row
        while (x>=-epsilon) {
            x-=a;
        }
        x+=a;

        // Loop through values of x on that row and store data
        while( (x >= -epsilon) && x <= (X_LIM) ){

            MyFile << x << "," << y << "|" ;  
            MyFile2 << "\\node (" << count << ") " << "at (" << x << "," << y << ") {};" << std::endl;
            GridPoints.push_back({x,y}); 

            count += 1; 
            if( x > (X_LIM-a) ) { break; }
            x+=a; 

        }

        MyFile << std::endl; 
        if( y > (Y_LIM-(b * sin(PSI))) ) { break; }
        j+=1; 

    }

    MyFile.close(); 
    MyFile2.close(); 

    // Base case 
    if (count == totalPopulation){ return GridPoints; } 
    if (count < totalPopulation){ return setupLattice(totalPopulation, filename, filename2, 0.5 * a); } 
    if (count > totalPopulation){ return setupLattice(totalPopulation, filename, filename2, 1.5 * a); }

}

// Calculate distance between two points
double distance(std::vector<double> r_1, std::vector<double> r_2){

    double x = r_1[0] - r_2[0]; 
    double y = r_1[1] - r_2[1]; 

    double distance = std::sqrt( x*x+ y*y ); 

    return distance; 
}

// Set up edges between plants
void setupEdges(std::vector<std::vector<double>> gridPoints, std::string filename){

    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename); 

    // Loop through all entity combinations 
    for(int i = 0; i < gridPoints.size(); i++){
        for(int j = 0; j < gridPoints.size(); j++){
            if( distance(gridPoints[i],gridPoints[j]) <= RADIUS ){
                // Write an edge if entities are within a distace, RADIUS, of each other
                MyFile << "\\draw (" << gridPoints[i][0] << ", " << gridPoints[i][1] << ")" << " -- " 
                << "(" << gridPoints[j][0] << ", " << gridPoints[j][1] << ");" << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    /*
    This function:
    1) Creates and starts a program clock
    3) Runs the simulation
    4) Stops and prints program clock
    */

    // Start measuring run time of program
    clock_t tClock; 
    tClock = clock(); 

    // Create file to store grid
    std::string FILENAME = "Lattice-Grid.txt"; 
    std::string FILENAME2 = "Nodes.txt"; 
    std::string FILENAME3 = "Edges.txt"; 

    std::vector<int> SICK_PLANTS; //total number of sick plants
    double totalPlants = TOTAL_NUMBER[0] + TOTAL_NUMBER[1];

    // Draw the lattice
    std::vector<std::vector<double>> GridPoints = setupLattice(totalPlants, FILENAME, FILENAME2, X_LIM); 

    // Trying stuff
    modifiedLattice(GridPoints, totalPlants, FILENAME, FILENAME2, X_LIM); 

    // Write the edges 
    setupEdges(GridPoints, FILENAME3); 

    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
