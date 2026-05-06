#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <vector>
#include <random>

// Tools for picking random numbers 
std::mt19937 randomNumberGeneratorEpidemics( std::random_device{}() ) ; 
std::uniform_real_distribution<double> distributionEpidemics(0,1); 

struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2,3
struct InfectedSentinelConnections { int u,d; }; // No. of infected sentinels an individual is connected to. u:undetectable, d:detectable
struct InfectedCropConnections { int u,d; }; // No. of infected crops an individual is connected to. u:undetectable, d:detectable

enum states {healthy = 1, undetectable = 2, detectable = 3};
enum plantType {cropType = 0, sentinelType = 1};

// Count the number of each population type that an entity is connected to 
void countNeighbours(flecs::world &world, std::vector<flecs::entity> &crops,
    std::vector<flecs::entity> &sentinels, flecs::entity &Link){

    world.system<InfectedSentinelConnections, InfectedCropConnections>()
        .each([&](flecs::entity e, InfectedSentinelConnections &sentinelInfected, 
                  InfectedCropConnections &cropInfected){

            sentinelInfected.u = 0; 
            sentinelInfected.d = 0;
            for(int sentinelIndex = 0; sentinelIndex < (int) sentinels.size(); sentinelIndex++){
                if( e.has(Link,sentinels[sentinelIndex]) && sentinels[sentinelIndex].get<MarkovState>().s == undetectable ) { 
                    sentinelInfected.u += 1;
                }
                else if( e.has(Link,sentinels[sentinelIndex]) && sentinels[sentinelIndex].get<MarkovState>().s == detectable ) { 
                    sentinelInfected.d += 1; 
                }
            }

            cropInfected.u = 0; 
            cropInfected.d = 0;
            for(int cropIndex = 0; cropIndex < (int) crops.size(); cropIndex++){
                if( e.has(Link,crops[cropIndex]) && crops[cropIndex].get<MarkovState>().s == undetectable ) { 
                    cropInfected.u += 1; 
                }
                else if( e.has(Link,crops[cropIndex]) && crops[cropIndex].get<MarkovState>().s == detectable ) { 
                    cropInfected.d += 1; 
                }
            }   
    }); 
}

// Update the probabilities vector for each entity
void updateProbabilities(flecs::world &world, const std::vector<double> &infectionRates, const std::vector<double> &scalings, 
    const std::vector<double> &presymptomaticTimes) {
    /*
    This system updates the transition probabilities vector for each entity 
    */
    world.system<MarkovState, PlantState, InfectedSentinelConnections, InfectedCropConnections>()
        .each([infectionRates, scalings, presymptomaticTimes](flecs::entity e, MarkovState &markovState, PlantState &plantState, 
                  InfectedSentinelConnections &sentinelInfected, InfectedCropConnections &cropInfected){
            std::vector<double> qnm = {0, 0, 0};
            if (markovState.s == healthy){
                double infectionForce = infectionRates[cropType] * (scalings[cropType] * cropInfected.u + cropInfected.d)
                               + infectionRates[sentinelType] * (scalings[sentinelType] * sentinelInfected.u + sentinelInfected.d);
                double q21 = infectionForce;
                double q11 = 1-q21;
                double q31 = 0;        
                qnm[0] = q11;
                qnm[1] = q21; 
                qnm[2] = q31;
            } else if (markovState.s == undetectable){
                double q12 = 0;
                double q32 = 1/presymptomaticTimes[plantState.type];
                double q22 = 1-q32;
                qnm[0] = q12;
                qnm[1] = q22; 
                qnm[2] = q32;
            } else if (markovState.s == detectable) {
                double q13 = 0;
                double q23 = 0;
                double q33 = 1;
                qnm[0] = q13; 
                qnm[1] = q23; 
                qnm[2] = q33;
            }
            e.set<TransitionProbabilities>({qnm});
        }); 
}

// Transition an entity from one markov state to another
void transition(flecs::world &world, std::vector<int> &cropNumbers, std::vector<int> &sentinelNumbers) {
    /*
    This decides which new state the entity should transition to. This depends only on its
    probability vector: p_vector[i-1] is the probabilities of transition to state s = i

    When the transition happens, the populationSizes vector is updated to reflect the new
    population state
    */ 
    world.system<MarkovState, PlantState, TransitionProbabilities>()
        .each([&](flecs::entity e, MarkovState& markovState, PlantState &plantState, TransitionProbabilities& p_vector){
            double rand = distributionEpidemics(randomNumberGeneratorEpidemics); 
            double probability_sum = 0;

            for(int i = 1; i <= (int) p_vector.qnm.size(); i++){
                probability_sum += p_vector.qnm[i-1];
                if ((rand < probability_sum)){
                    if (plantState.type == 0) {
                        cropNumbers[markovState.s-1] -= 1;
                        cropNumbers[i-1] += 1; 
                    } else if (plantState.type == 1) {
                        sentinelNumbers[markovState.s-1] -= 1;
                        sentinelNumbers[i-1] += 1; 
                    }
                    e.set<MarkovState>({i});
                    break; 
                } 
            }
        }); 
    }