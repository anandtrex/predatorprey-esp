/*
 * Domain.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#include <iostream>
#include <fstream>

#include <libconfig.h++>

#include "DomainTotal.h"
#include "PredatorEsp.h"
#include "../Esp/NetworkContainer.h"

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::map;
    using std::pair;
    using std::ofstream;
    using std::string;

    DomainTotal::DomainTotal()
            : Domain()
    {
    }

    DomainTotal::DomainTotal(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numPrey, const uint& numHunters,
            const double& preyMoveProb, const double& hunterMoveProb)
            : Domain(maxSteps, width, height), numPredators(numPredators), numPrey(numPrey), numHunters(
                    numHunters), preyMoveProb(preyMoveProb), hunterMoveProb(hunterMoveProb), hunterRoleReversalProbability(0.0)
    {
        numOtherAgents = numPrey + numHunters;
        LOG(INFO) << "Initialized DomainTotal with number of predators " << numPredators
                << ", num of prey " << numPrey << ", number of hunters " << numHunters;
    }

    DomainTotal::DomainTotal(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numPrey, const uint& numHunters,
            const double& preyMoveProb, const double& hunterMoveProb,
            const double& hunterRoleReversalProbability)
            : Domain(maxSteps, width, height), numPredators(numPredators), numPrey(numPrey), numHunters(
                    numHunters), preyMoveProb(preyMoveProb), hunterMoveProb(hunterMoveProb), hunterRoleReversalProbability(
                    hunterRoleReversalProbability)
    {
        numOtherAgents = numPrey + numHunters;
        LOG(INFO) << "Initialized DomainTotal with number of predators " << numPredators
                << ", num of prey " << numPrey << ", number of hunters " << numHunters;
    }

    DomainTotal::~DomainTotal()
    {
        for (uint i = 0; i < numPredators; i++) {
            delete vPredators[i];
        }
        vPredators.clear();
        for (uint i = 0; i < numPrey; i++) {
            delete vPreys[i];
        }
        vPreys.clear();
        for (uint i = 0; i < numHunters; i++) {
            delete vHunters[i];
        }
        vHunters.clear();
    }

    void DomainTotal::init(NetworkContainer* espNetwork)
    {
        Position randomPosition;
        uint id = 1;
        vPredators.clear();
        vPreys.clear();
        vHunters.clear();
        for (uint i = 0; i < numPredators; i++) {
            randomPosition = ptrGridWorld->getRandomPosition();
            vPredators.push_back(
                    dynamic_cast<Predator*>(new PredatorEsp(ptrGridWorld, id++, randomPosition,
                            espNetwork)));
            LOG(INFO) << "[CREATED] Predator at " << randomPosition.x << ", "
                    << randomPosition.y << " with id " << id << endl;
        }

        // initialize prey
        for (uint i = 0; i < numPrey; i++) {
            randomPosition = ptrGridWorld->getRandomPosition();
            vPreys.push_back(new Prey(ptrGridWorld, id++, randomPosition, preyMoveProb));
            LOG(INFO) << "Prey move probability is " << preyMoveProb;
            LOG(INFO) << "[CREATED] Prey at " << randomPosition.x << ", " << randomPosition.y
                    << " with id " << id << endl;
        }

        for (uint i = 0; i < numHunters; i++) {
            randomPosition = ptrGridWorld->getRandomPosition();
            vHunters.push_back(
                    new Hunter(ptrGridWorld, id++, randomPosition, hunterMoveProb,
                            hunterRoleReversalProbability));
            LOG(INFO) << "Hunter move probability is " << hunterMoveProb;
            LOG(INFO) << "[CREATED] Hunter at " << randomPosition.x << ", " << randomPosition.y
                    << " with id " << id << endl;
        }
    }

    void DomainTotal::step(const uint& stepNo)
    {
        if (stepNo > maxSteps)
            LOG(FATAL) << "Step no is greater than maxSteps!";

        vector<AgentInformation> vAiPredator, vAiPrey, vAiHunter;

        for (uint i = 0; i < numPredators; i++) {
            vAiPredator.push_back(vPredators[i]->getAgentInformation());
        }
        for (uint i = 0; i < numPrey; i++) {
            vAiPrey.push_back(vPreys[i]->getAgentInformation());
        }
        for (uint i = 0; i < numHunters; i++) {
            vAiHunter.push_back(vHunters[i]->getAgentInformation());
        }

        // build vector<AgentInformation>
        vector<AgentInformation> vAgentInformation;
        vAgentInformation.clear();
        vAgentInformation.insert(vAgentInformation.begin(), vAiPredator.begin(), vAiPredator.end());
        vAgentInformation.insert(vAgentInformation.begin(), vAiPrey.begin(), vAiPrey.end());
        vAgentInformation.insert(vAgentInformation.begin(), vAiHunter.begin(), vAiHunter.end());

        // move prey
        for (uint i = 0; i < numPrey; i++) {
            vPreys[i]->move(vAgentInformation);
        }
        // move hunter
        for (uint i = 0; i < numHunters; i++) {
            vHunters[i]->move(vAgentInformation);
        }
        // move predator
        for (uint i = 0; i < numPredators; i++) {
            vPredators[i]->move(vAgentInformation, stepNo);
        }

        for (uint i = 0; i < numPrey; i++) {
            for (uint j = 0; j < numPredators; j++) {
                if ((vAiPrey[i].position.x == vAiPredator[j].position.x)
                        && (vAiPrey[i].position.y == vAiPredator[j].position.y)) {
                    LOG(INFO) << "Prey caught by Predator";
                    preyCaughtIds.push_back(vAiPrey[i].agentId);
                    if (numPreyCaught < numPrey) {
                        numPreyCaught++;
                    }
                }
            }
        }

        for (uint i = 0; i < numPredators; i++) {
            for (uint j = 0; j < numHunters; j++) {
                if ((vAiHunter[j].position.x == vAiPredator[i].position.x)
                        && (vAiHunter[j].position.y == vAiPredator[i].position.y)) {
                    if (vAiHunter[j].agentType == HUNTER) {
                        LOG(INFO) << "Predator caught by Hunter";
                        LOG(INFO) << "Predator at " << vAiPredator[i].position.x << ", "
                                << vAiPredator[i].position.y;
                        LOG(INFO) << "Hunter at " << vAiHunter[j].position.x << ", "
                                << vAiHunter[j].position.y;
                        predatorCaughtIds.push_back(vAiPredator[i].agentId);
                        if (numPredCaught < numPredators) {
                            numPredCaught++;
                        }
                    } else if (vAiHunter[j].agentType == HUNTER_WEAK) {
                        LOG(INFO) << "Weak hunter caught by predator!";
                        hunterCaughtIds.push_back(vAiHunter[j].agentId);
                        if (numHunterCaught < numHunters) {
                            numHunterCaught++;
                        }
                    }
                }
            }
        }
    }

    double DomainTotal::run()
    {
        uint noSteps = 1;
        uint prevNumPreyCaught = 0;
        uint prevNumPredCaught = 0;
        uint prevNumHunterCaught = 0;
        numPredCaught = 0;
        numPreyCaught = 0;
        numHunterCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();
        hunterCaughtIds = vector<uint>();

        do {
            LOG(INFO) << "step: " << noSteps << endl;
            (void) step(noSteps);
            if (numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught) {
                LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                VLOG(5)
                        << "Number of prey caught is " << numPreyCaught
                                << " and the total number of prey is " << numPrey;
                prevNumPreyCaught = numPreyCaught;
                if (numPreyCaught == numPrey) {
                    LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            } else if (numPredCaught > 0 && numPredCaught > prevNumPredCaught) {
                LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
                prevNumPredCaught = numPredCaught;
                VLOG(5)
                        << "Number of predators caught is " << numPredCaught
                                << " and the total number of predators is " << numPredators;
                if (numPredCaught == numPredators) {
                    LOG(INFO) << "All predators caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            } else if (numHunterCaught > 0 && numHunterCaught > prevNumHunterCaught) {
                LOG(INFO) << "WEAK HUNTER CAUGHT!!!!" << endl;
                prevNumHunterCaught = numHunterCaught;
                VLOG(5)
                        << "Number of weak hunters caught is " << numHunterCaught
                                << " and the total number of hunters is " << numHunters;
                if (numHunterCaught == numHunters) {
                    LOG(INFO) << "All hunters caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            }
        } while (++noSteps <= maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return calculateFitness(noSteps - 1);
    }

    double DomainTotal::run(string stepsFilePath)
    {
        LOG(INFO) << "Running with output to file";

        uint noSteps = 1;
        uint prevNumPreyCaught = 0;
        uint prevNumPredCaught = 0;
        uint prevNumHunterCaught = 0;
        numPredCaught = 0;
        numPreyCaught = 0;
        numHunterCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();
        hunterCaughtIds = vector<uint>();

        // order is pred.x pred.y prey.x prey.y hunter.x hunter.y
        ofstream fout(stepsFilePath.c_str());
        if (!fout.is_open()) {
            LOG(FATAL) << "Could not open file " << stepsFilePath << " for writing!" << endl;
        }
        Position positionPredator, positionPrey, positionHunter;
        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void) step(noSteps);
            if (numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught) {
                LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                VLOG(5)
                        << "Number of prey caught is " << numPreyCaught
                                << " and the total number of prey is " << numPrey;
                prevNumPreyCaught = numPreyCaught;
                if (numPreyCaught == numPrey) {
                    LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            } else if (numPredCaught > 0 && numPredCaught > prevNumPredCaught) {
                LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
                prevNumPredCaught = numPredCaught;
                VLOG(5)
                        << "Number of prey caught is " << numPredCaught
                                << " and the total number of prey is " << numPredators;
                if (numPredCaught == numPredators) {
                    LOG(INFO) << "All predators caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            } else if (numHunterCaught > 0 && numHunterCaught > prevNumHunterCaught) {
                LOG(INFO) << "WEAK HUNTER CAUGHT!!!!" << endl;
                prevNumHunterCaught = numHunterCaught;
                VLOG(5)
                        << "Number of weak hunters caught is " << numHunterCaught
                                << " and the total number of hunters is " << numHunters;
                if (numHunterCaught == numHunters) {
                    LOG(INFO) << "All hunters caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            }
            for (uint i = 0; i < numPredators; i++) {
                positionPredator = vPredators[i]->getPosition();
                fout << positionPredator.x << " " << positionPredator.y << " ";
            }
            for (uint i = 0; i < numPrey; i++) {
                positionPrey = vPreys[i]->getPosition();
                fout << positionPrey.x << " " << positionPrey.y << " ";
            }
            for (uint i = 0; i < numHunters; i++) {
                positionHunter = vHunters[i]->getPosition();
                fout << positionHunter.x << " " << positionHunter.y;
            }
            fout << endl;
        } while (++noSteps <= maxSteps);
        fout.close();

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        LOG(INFO) << "[ENDS] Experiment::run()" << endl;
        return calculateFitness(noSteps - 1);
    }

    double DomainTotal::calculateFitness(const uint& stepCurrent)
    {
        if (stepCurrent > maxSteps) {
            LOG(FATAL) << "Step no is greater than maxSteps!" << " stepCurrent is "
                    << stepCurrent << " and maxSteps is " << maxSteps;
        }

        double fitness = 0.0;
        LOG(INFO) << "In calculateFitness";
        LOG(INFO) << "Num prey caught is " << numPreyCaught;
        LOG(INFO) << "Num predators caught is " << numPredCaught;
        LOG(INFO) << "Num hunters caught is " << numHunterCaught;
        LOG(INFO) << "stepCurrent is " << stepCurrent;
        if (numPreyCaught > numPrey || numPredCaught > numPredators
                || numHunterCaught > numHunters) {
            LOG(FATAL) << "Num caught is too high!!";
        }
        if (numPreyCaught > 0) {     // Yay!
            LOG(INFO) << "Yay!";
            fitness = static_cast<double>(10) * (maxSteps - stepCurrent) * numPreyCaught;
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        } else if (numPredCaught > 0) {     // :-(
            LOG(INFO) << ":-(";
            LOG(INFO) << "maxSteps is " << maxSteps;
            fitness = static_cast<double>(10) * -1 * (maxSteps - stepCurrent) * numPredCaught;
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        } else if (numHunterCaught > 0) {     // Double Yay!
            LOG(INFO) << "WTF!!!!!";
            LOG(FATAL) << "Can't be here!!!!!!!";
            fitness = static_cast<double>(100) * (maxSteps - stepCurrent) * numHunterCaught;
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        } else {
            LOG(INFO) << "Meh";
            // calculate distance from hunter and prey
            uint distancePrey = 0, distanceHunter = 0;
            for (uint i = 0; i < numPredators; i++) {
                Position positionPredator = vPredators[i]->getPosition();
                for (uint j = 0; j < numPrey; j++) {
                    Position positionPrey = vPreys[j]->getPosition();
                    distancePrey += ptrGridWorld->distance(positionPrey, positionPredator);
                }
                for (uint j = 0; j < numHunters; j++) {
                    Position positionHunter = vHunters[j]->getPosition();
                    distanceHunter += ptrGridWorld->distance(positionHunter, positionPredator);
                }
            }

            // reward for being close to prey and far away from hunter
            distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
            // take into account the size of the grid for rewarding later
            fitness = static_cast<double>(distancePrey) + distanceHunter;
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        }
    }
}
