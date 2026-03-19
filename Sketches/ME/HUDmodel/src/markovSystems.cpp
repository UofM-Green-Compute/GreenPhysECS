#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <vector>
#include <random>

// Tools for picking random numbers 
std::mt19937 randomNumberGeneratorEpidemics( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> distributionEpidemics(0,1); 

struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2, 3

std::vector<double> calculateProbabilities(std::vector<int> crop_vector, std::vector<int> sentinel_vector, int state,
    std::vector<double> InfectionRatesVector, std::vector<double> scalingsVector, 
    std::vector<double> presymptomaticTimesVector,
    int plantType){
    //std::cout<<"WHats going on?? - Three non blondes"<<InfectionRatesVector[0]<<"\n";
    //std::cout<<"WHats going on?? - Three non blondes"<<InfectionRatesVector[1]<<"\n";   
    std::vector<double> probabilities = {0, 0, 0};
    double infectionForce = InfectionRatesVector[0] * (scalingsVector[0] * crop_vector[0] + crop_vector[1])
                          + InfectionRatesVector[1] * (scalingsVector[1] * sentinel_vector[0] + sentinel_vector[1]);
    //std::cout<< InfectionRatesVector[0] << ", " << scalingsVector[0] << "," << crop_vector[0] <<"\n";
    //std::cout<<infectionForce<<"\n";
    if (state == 1){
        double q21 = infectionForce;
        double q11 = 1-q21;
        double q31 = 0;        
        probabilities[0] = q11; 
        probabilities[1] = q21; 
        probabilities[2] = q31;
    } else if (state == 2){
        double q12 = 0;
        double q32 = 1/presymptomaticTimesVector[plantType];
        double q22 = 1-q32;
        probabilities[0] = q12; 
        probabilities[1] = q22; 
        probabilities[2] = q32;
    } else if (state == 3) {
        double q13 = 0;
        double q23 = 0;
        double q33 = 1;
        probabilities[0] = q13; 
        probabilities[1] = q23; 
        probabilities[2] = q33;
    }
    return probabilities; 
}

void updateProbabilities(flecs::world &world, std::vector<int> &cropPopulation, std::vector<int> &sentinelPopulation, 
    std::vector<double> &infectionRates, std::vector<double> &scalings, 
    std::vector<double> &presymptomaticTimes) {
    std::cout<<"boo: "<<infectionRates[0]<<"\n";
    /*
    This system updates the transition probabilities vector for each entity 
    */ 
   world.system<MarkovState, PlantState>()
        .each([&](flecs::entity e, MarkovState &markovState, PlantState &plantState){
            std::cout<<"boo: "<<infectionRates[0]<<"\n";
            std::cout<<"bonjour: "<< cropPopulation[0]<<"\n";
            std::vector<double> qnm = calculateProbabilities(cropPopulation, sentinelPopulation, markovState.s,
                infectionRates, scalings, presymptomaticTimes, plantState.type); 
            e.set<TransitionProbabilities>({qnm});
            //std::cout<<"system 2\n";
        }); 
}

void transition(flecs::world &world, std::vector<int> &cropsPopulation, std::vector<int> &sentinelsPopulation) {
    /*
    This decides which new state the entity should transition to. This depends only on its
    probability vector: p_vector[i] is the probabilities of transition to state s = i

    When the transition happens, the populationSizes vector is updated to reflect the new
    population state
    */ 
    world.system<MarkovState, PlantState, TransitionProbabilities>()
        .each([&](flecs::entity e, MarkovState& markovState, PlantState &plantState, TransitionProbabilities& p_vector){
            double rand = distributionEpidemics(randomNumberGeneratorEpidemics); 
            double probability_sum = 0;
            
            for(int i = 1; i <= (int) p_vector.qnm.size(); i++){
                probability_sum += p_vector.qnm[i-1];
                if ((rand < probability_sum) ){
                    if (plantState.type == 0) {
                        cropsPopulation[markovState.s-1] -= 1;
                        cropsPopulation[i-1] += 1; 
                    } else if (plantState.type == 1) {
                        sentinelsPopulation[markovState.s-1] -= 1;
                        sentinelsPopulation[i-1] += 1; 
                    }
                    e.set<MarkovState>({i});
                    break; 
                } 
            }
            //std::cout<<"system 1\n";
        }); 
    }
