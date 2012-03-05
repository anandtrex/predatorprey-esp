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
    using PredPreyHunterVisualizer::Visualizer;

    DomainTotal::DomainTotal() :
            Domain()
    {
    }

    DomainTotal::DomainTotal(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numPrey, const uint& numHunters,
            const double& preyMoveProb, const double& hunterMoveProb) :
            Domain(maxSteps, width, height), numPredators(numPredators), numPrey(numPrey), numHunters(
                    numHunters), preyMoveProb(preyMoveProb), hunterMoveProb(hunterMoveProb)
    {
        numOtherAgents = numPrey + numHunters;
        hunterRoleReversalProbability = 0.0;
    }

    DomainTotal::DomainTotal(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numPrey, const uint& numHunters,
            const double& preyMoveProb, const double& hunterMoveProb, const double& hunterRoleReversalProbability) :
            Domain(maxSteps, width, height), numPredators(numPredators), numPrey(numPrey), numHunters(
                    numHunters), preyMoveProb(preyMoveProb), hunterMoveProb(hunterMoveProb), hunterRoleReversalProbability(hunterRoleReversalProbability)
    {
        numOtherAgents = numPrey + numHunters;
    }

    DomainTotal::~DomainTotal()
    {
        delete ptrPredator;
        delete ptrPrey;
        delete ptrHunter;
    }

    void DomainTotal::init(NetworkContainer* espNetwork)
    {
        Position randomPosition = ptrGridWorld->getRandomPosition();
        ptrPredator = dynamic_cast<Predator*>(new PredatorEsp(ptrGridWorld, 1, randomPosition,
                espNetwork));
        LOG(INFO) << "[CREATED] Predator at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 1 << endl;

        // initialize prey
        randomPosition = ptrGridWorld->getRandomPosition();
        ptrPrey = new Prey(ptrGridWorld, 2, randomPosition, preyMoveProb);
        LOG(INFO) << "Prey move probability is " << preyMoveProb;
        LOG(INFO) << "[CREATED] Prey at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 2 << endl;

        randomPosition = ptrGridWorld->getRandomPosition();
        ptrHunter = new Hunter(ptrGridWorld, 3, randomPosition, hunterMoveProb, hunterRoleReversalProbability);
        LOG(INFO) << "Hunter move probability is " << hunterMoveProb;
        LOG(INFO) << "[CREATED] Hunter at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 3 << endl;
    }

    void DomainTotal::enableDisplay(const vector<double>& predatorColour,
            const vector<double>& preyColour, const vector<double>& hunterColour)
    {
        this->displayEnabled = true;
        LOG(INFO) << "Display enabled";
        map<int, vector<double> > idColorMapping = map<int, vector<double> >();
        pair<int, vector<double> > p = pair<int, vector<double> >();

        p = pair<int, vector<double> >(1, predatorColour);
        idColorMapping.insert(p);

        p = pair<int, vector<double> >(2, preyColour);
        idColorMapping.insert(p);

        p = pair<int, vector<double> >(3, hunterColour);
        idColorMapping.insert(p);

        visualizer = Visualizer(idColorMapping, ptrGridWorld->width, ptrGridWorld->height);
    }

    void DomainTotal::step()
    {
        AgentInformation aiPredator, aiPrey, aiHunter;
        aiPredator = ptrPredator->getAgentInformation();
        aiPrey = ptrPrey->getAgentInformation();
        aiHunter = ptrHunter->getAgentInformation();

        // build vector<AgentInformation>
        vector<AgentInformation> vAgentInformation;
        vAgentInformation.clear();
        vAgentInformation.push_back(aiPredator);
        vAgentInformation.push_back(aiPrey);
        vAgentInformation.push_back(aiHunter);

        // move prey
        ptrPrey->move(vAgentInformation);
        // move hunter
        ptrHunter->move(vAgentInformation);
        // move predator
        ptrPredator->move(vAgentInformation);

        VLOG(2) << "Predator at " << aiPredator.position.x << "," << aiPredator.position.y;
        VLOG(2) << "Prey at " << aiPrey.position.x << "," << aiPrey.position.y;
        VLOG(2) << "Hunter at " << aiHunter.position.x << "," << aiHunter.position.y;

        if ((aiPrey.position.x == aiPredator.position.x)
                && (aiPrey.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Prey caught by Predator";
            preyCaughtIds.push_back(aiPrey.agentId);
            numPreyCaught++;
        }

        if ((aiHunter.position.x == aiPredator.position.x)
                && (aiHunter.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Predator caught by Hunter";
            predatorCaughtIds.push_back(aiPredator.agentId);
            numPredCaught++;
        }

        // Show display
        if (displayEnabled) {
            // FIXME Need to do this in a better way
            static vector<AgentInformation> vAgentInformationPrevious = vector<AgentInformation>();
            if (vAgentInformationPrevious.size() == 0) {
                vAgentInformationPrevious = vAgentInformation;
            }
            visualizer.show(vAgentInformationPrevious, vAgentInformation);
            vAgentInformationPrevious = vAgentInformation;
        }

    }

    double DomainTotal::run()
    {
        uint noSteps = 0;
        uint prevNumPreyCaught = 0;
        uint prevNumPredCaught = 0;
        numPredCaught = 0;
        numPreyCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();

        if (displayEnabled) {
            visualizer.createWindow();
        }

        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void) step();
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
            }
        } while (noSteps++ < maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return calculateFitness(noSteps);
    }

    double DomainTotal::run(string stepsFilePath)
    {
        uint noSteps = 0;
        uint prevNumPreyCaught = 0;
        uint prevNumPredCaught = 0;
        numPredCaught = 0;
        numPreyCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();

        if (displayEnabled) {
            visualizer.createWindow();
        }

        // order is pred.x pred.y prey.x prey.y hunter.x hunter.y
        ofstream fout(stepsFilePath.c_str());
        if (!fout.is_open()) {
            LOG(FATAL) << "Could not open file " << stepsFilePath << " for writing!" << endl;
        }
        double fitness = 0.0;
        Position positionPredator, positionPrey, positionHunter;
        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void) step();
            if (numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught) {
                LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                VLOG(5)
                        << "Number of prey caught is " << numPreyCaught
                                << " and the total number of prey is " << numPrey;
                prevNumPreyCaught = numPreyCaught;
                if (numPreyCaught == numPrey) {
                    LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                    fout.close();
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
            }
            positionPredator = ptrPredator->getPosition();
            positionPrey = ptrPrey->getPosition();
            positionHunter = ptrHunter->getPosition();
            fout << positionPredator.x << " " << positionPredator.y << " ";
            fout << positionPrey.x << " " << positionPrey.y << " ";
            fout << positionHunter.x << " " << positionHunter.y << endl;
        } while (noSteps++ < maxSteps);
        fout.close();
        LOG(INFO) << "[ENDS] Experiment::run()" << endl;
        return fitness;
    }

    double DomainTotal::calculateFitness(const uint& stepCurrent)
    {
        double fitness = 0.0;
        if (numPreyCaught > 0) {     // Yay
            fitness = static_cast<double>(10) * (maxSteps - stepCurrent) * numPreyCaught;
            return fitness;
        } else if (numPredCaught > 0) {     // :-(
            VLOG(2) << "maxSteps - stepCurrent: " << maxSteps - stepCurrent << endl;
            fitness = static_cast<double>(-1) * 10 * (maxSteps - stepCurrent) * numPredCaught;
            LOG(INFO) << "Fitness: " << fitness << endl;
            return fitness;
        } else {
            // calculate distance from hunter and prey
            Position positionPredator = ptrPredator->getPosition();
            Position positionPrey = ptrPrey->getPosition();
            Position positionHunter = ptrHunter->getPosition();
            uint distancePrey, distanceHunter;
            distancePrey = ptrGridWorld->distance(positionPrey, positionPredator);
            distanceHunter = ptrGridWorld->distance(positionHunter, positionPredator);
            // reward for being close to prey and far away from hunter
            distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
            // take into account the size of the grid for rewarding later
            fitness = static_cast<double>(distancePrey) + distanceHunter; // if by any chance it reaches here although it won't
            return fitness;
        }
    }
}
