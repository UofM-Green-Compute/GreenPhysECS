#include <flecs.h>
#include <systems.h>
#include <iostream>
#include <vector>
#include <random>

// Tools for picking random numbers 
std::mt19937 randomNumberGeneratorEpidemics( std::random_device{}()  ) ; 
std::uniform_real_distribution<double> distributionEpidemics(0,1); 

struct Index { int plantNumber; }; // what is your index within your vector
struct MarkovState { int s; }; // Track current markov state of entity
struct PlantState { int type; }; // 0: crop. 1: sentinel
struct TransitionProbabilities { std::vector<double> qnm; }; // Transition from state m to state n = 1,2,3
struct InfectedSentinelConnections { int u,d; }; // No. of infected sentinels an individual is connected to. u:undetectable, d:detectable
struct InfectedCropConnections { int u,d; }; // No. of infected crops an individual is connected to. u:undetectable, d:detectable

enum states {healthy = 1, undetectable = 2, detectable = 3};
enum plantType {cropType = 0, sentinelType = 1};

void updateInfectedNumbers(flecs::world &world, std::vector<flecs::entity> &sentinels,
    std::vector<flecs::entity> &crops, flecs::entity &uLink, flecs::entity &dLink){
    world.system<InfectedSentinelConnections, InfectedCropConnections>()
        .each([&](flecs::entity e, InfectedSentinelConnections &sentinelInfected, 
                  InfectedCropConnections &cropInfected){
            
            sentinelInfected.u = 0; 
            sentinelInfected.d = 0;
            for(int sentinelIndex = 0; sentinelIndex < (int) sentinels.size(); sentinelIndex++){
                if(e.has(uLink,sentinels[sentinelIndex])) { 
                    sentinelInfected.u += 1; 
                }
                else if(e.has(dLink,sentinels[sentinelIndex])) { 
                    sentinelInfected.d += 1; 
                }
            }
            cropInfected.u = 0; 
            cropInfected.d = 0;
            for(int cropIndex = 0; cropIndex < (int) crops.size(); cropIndex++){
                if(e.has(uLink,crops[cropIndex])) { 
                    cropInfected.u += 1; 
                }
                else if(e.has(dLink,crops[cropIndex])) { 
                    cropInfected.d += 1; 
                }
            }    
        }); 
}

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
                //std::cout<<infectionForce<<std::endl;
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

void transition(flecs::world &world, std::vector<int> &cropsPopulation, std::vector<int> &sentinelsPopulation) {
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
                if (markovState.s == undetectable){
                    //std::cout<<p_vector.qnm[0]<<", "<<p_vector.qnm[1]<<", "<<p_vector.qnm[2]<<std::endl;
                    //std::cout<<rand<<std::endl;
                }
                // if probability sum > rand then move onto the next state
                if (rand>probability_sum) {
                    continue;
                }
                if (plantState.type == cropType) {
                    cropsPopulation[markovState.s-1] -= 1;
                    cropsPopulation[i-1] += 1;
                } else if (plantState.type == sentinelType) {
                    sentinelsPopulation[markovState.s-1] -= 1;
                    sentinelsPopulation[i-1] += 1; 
                }

                e.set<MarkovState>({i});
                break;    
            }
            //std::cout<<"system 1\n";
        }); 
    }

void updateGraph(flecs::world &world, std::vector<flecs::entity> &crops, std::vector<flecs::entity> &sentinels,
                 flecs::entity &hLink, flecs::entity &uLink, flecs::entity &dLink) {
    // Update the links between entities

    // if entity in D
        // if e.has(hlink) or e.has(uLink) -----> remove link and add dLink
        // if e.has(dLink) -----> do nothing


    // if entity in U
        // if e.has(hLink) -----> remove hLink and add uLink
        // if e.has(uLink) or e.has(dLink) ----> do nothing

    // only change e.has not plant[index].has because that might mess with thing if
    // systems run parallel in future
    world.system<Index, MarkovState, PlantState>()
        .each([&](flecs::entity e, Index& plantIndex, MarkovState& state, PlantState &plantState){
            // update links with crops
            for(int j = 0; j < (int) crops.size(); j++){
                // if entity is a crop, no link to update
                // check D. if yes, follow above outlined algorithm
                if (plantIndex.plantNumber == j && plantState.type == cropType){
                    continue;
                }
                if (state.s == detectable || crops[j].get<MarkovState>().s == detectable) {
                    if (e.has(dLink,crops[j])){
                        continue;
                    }
                    if (e.has(hLink,crops[j])){
                        e.remove(hLink,crops[j]);
                        e.add(dLink,crops[j]);
                    }
                    if (e.has(uLink,crops[j])){
                        e.remove(uLink,crops[j]);
                        e.add(dLink,crops[j]);
                    }
                }
                // check U. if yes, follow above outlined algorithm
                if (state.s == undetectable || crops[j].get<MarkovState>().s == undetectable) {
                    if (e.has(uLink,crops[j])||e.has(dLink,crops[j])){
                        continue;
                    }
                    if (e.has(hLink,crops[j])){
                        e.remove(hLink,crops[j]); 
                        e.add(uLink,crops[j]);
                    }   
                }
            }  
            // update links with sentinels
            for(int j = 0; j < (int) sentinels.size(); j++){
                // if entity is a crop, no link to update
                if (plantIndex.plantNumber == j && plantState.type == sentinelType){
                    continue;
                }
                                // check D. if yes, follow above outlined algorithm
                if (state.s == detectable || sentinels[j].get<MarkovState>().s == detectable) {
                    if (e.has(dLink,sentinels[j])){
                        continue;
                    }
                    if (e.has(hLink,sentinels[j])){
                        e.remove(hLink,sentinels[j]);
                        e.add(dLink,sentinels[j]);
                    }
                    if (e.has(uLink,sentinels[j])){
                        e.remove(uLink,sentinels[j]);
                        e.add(dLink,sentinels[j]);
                    }
                }
                // check U. if yes, follow above outlined algorithm
                if (state.s == undetectable || sentinels[j].get<MarkovState>().s == undetectable) {
                    if (e.has(uLink,sentinels[j])||e.has(dLink,sentinels[j])){
                        continue;
                    }
                    if (e.has(hLink,sentinels[j])){
                        e.remove(hLink,sentinels[j]); 
                        e.add(uLink,sentinels[j]);
                    } 
                }
            }  
        }); 
    }
