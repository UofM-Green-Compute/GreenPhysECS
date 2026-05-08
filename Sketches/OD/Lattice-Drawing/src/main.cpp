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
double RADIUS = 0.12 + epsilon; 

// Initial Conditions
std::vector<int> TOTAL_NUMBER = {100, 5}; // total number of plants {crops,sentinels}

// Recursive function to setup the lattice 
std::vector<std::vector<double>> setupLatticeOlder(int totalPopulation, std::string filename, std::string filename2, int no_columns){
    
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
    return setupLatticeOlder(totalPopulation, filename, filename2, no_columns+1); 
}

// Function to setup the lattice - better maximises the space used
std::vector<std::vector<double>> setupLattice(int totalPopulation, std::string filename, std::string filename2){
    
    // maximum number of iterations
    int maxIteration = 100;
    // very small number to handle edge cases
    double epsilon = pow(10,-7);

    // Open files 
    std::ofstream MyFile; 
    MyFile.open(filename); 
    std::ofstream MyFile2; 
    MyFile2.open(filename2);

    // As the iteration continues, lower_a and lower_b will be the highest values of 
    // a and b yet found which fit all grid point. while upper_a and upper_b will
    // be lowest values of a and b yet found which do not fit all grid points. these 
    // will converge on the best a and b. after 100 iterations, lower_a and lower_b 
    // will be chosen
    double lower_a = 0;
    double upper_a = X_LIM;
    double upper_b = C * upper_a; 

    double iteration_a = X_LIM;
    double iteration_b = C * iteration_a; 
    for (int iterationCounter = 0; iterationCounter <= maxIteration; iterationCounter++){
        // Define terms for later use 
        double x = 0; 
        double y = 0; 
        int count = 0; 
        int j = 0;
        while ( (y > -epsilon) && (y < Y_LIM+epsilon) ){
            // Find a valid x position for a row 
            if(PSI > M_PI / 2) { 
                x = j * iteration_b * -cos(PSI); 
            }
            else{ 
                x = j * iteration_b * cos(PSI); 
            } 
            // Backtrack to the first points within the limits on that row
            while (x > iteration_a-epsilon) {
                x-=iteration_a;
            }
            // Set the value of y
            y = j * iteration_b * sin(PSI);
            while( (x > -epsilon) && x <= (X_LIM+epsilon) && (count < totalPopulation) ){

                count += 1; 
                if( x > (X_LIM-iteration_a) ) { 
                    break; 
                }
                x+=iteration_a; 

            }
            if(y > (Y_LIM-(iteration_b*sin(PSI)))){ break; }
            j+=1; 
        }
        if (count < totalPopulation){
            // too few gridpoints: a and b are too large
            upper_a = iteration_a;
            upper_b = iteration_b;
            // take midpoint between currend and lower bound
            iteration_a = (lower_a+iteration_a)/2;
            iteration_b = C*iteration_a;
        }
        if (count >= totalPopulation){
            // enough to fit: a and b too small or just right
            lower_a = iteration_a;
            // take midpoint between currend and lower bound
            iteration_a = (upper_a+iteration_a)/2;
            iteration_b = C*iteration_a;
        }
    }

    // now pick lower_a and lower_b as the highest survivors which can fit everything
    double lower_b = C*lower_a; 
    double x = 0; 
    double y = 0; 
    int count = 0; 
    int j = 0;
    // Create a vector to store the lattice positions
    std::vector<std::vector<double>> GridPoints; 
    while ( (y > -epsilon) && (y < Y_LIM+epsilon) ){
        // Find a valid x position for a row 
        if(PSI > M_PI / 2) { 
            x = j * lower_b * -cos(PSI); 
        }
        else{ 
            x = j * lower_b * cos(PSI); 
        } 
        // Backtrack to the first points within the limits on that row
        while (x > lower_a-epsilon) {
            x-=lower_a;
        }
        // Set the value of y
        y = j * lower_b * sin(PSI);
        // Loop through values of x on that row and store data
        int xCount = 0;
        while( (x >= -epsilon) && x <= (X_LIM+epsilon) && (count < totalPopulation) ){

            MyFile << x << "," << y << "|";
            MyFile2 << "\\node (" << count << ") " << "at (" << x << "," << y << ") {};" << std::endl;
            GridPoints.push_back({x, y});
            count += 1;
            if( x > (X_LIM-lower_a) ) { break; }
            x += lower_a;
        }
        MyFile << std::endl; 
        if(y > (Y_LIM-(lower_b*sin(PSI)))){ break; }
        j+=1; 
    }
    
    MyFile.close(); 

    return GridPoints;
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
            if( (distance(gridPoints[i],gridPoints[j]) <= RADIUS) && (i != j) ){
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
    std::vector<std::vector<double>> GridPoints = setupLattice(totalPlants, FILENAME, FILENAME2); 

    // Write the edges 
    setupEdges(GridPoints, FILENAME3); 

    // Record How long the simulation took
    tClock = clock() - tClock; 
    double time_taken = ((double)tClock) / CLOCKS_PER_SEC;  
    std::cout<<"RUN TIME: "<<time_taken<<"s"<<std::endl;
}
