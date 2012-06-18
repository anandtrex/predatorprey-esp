/*
 * DomainOne.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: anand
 */

#include <iostream>
#include <fstream>

#include <libconfig.h++>

#include "DomainOne.h"
#include "PredatorEsp.h"
#include "../Esp/NetworkContainer.h"

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::map;
    using std::pair;
    using PredPreyHunterVisualizer::Visualizer;

    template<class T>
    DomainOne<T>::DomainOne()
            : Domain()
    {
    }

    template<class T>
    DomainOne<T>::DomainOne(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numAgents, const double& agentMoveProb)
            : Domain(maxSteps, width, height), numPredators(numPredators), numAgents(numAgents), agentMoveProb(
                    agentMoveProb)
    {
        numOtherAgents = numAgents;
    }

    template<class T>
    DomainOne<T>::~DomainOne()
    {
         delete ptrPredator;
         delete ptrOtherAgent;
         delete ptrGridWorld;
    }

    template<class T>
    void DomainOne<T>::init(NetworkContainer* espNetwork)
    {
        Position randomPosition = ptrGridWorld->getRandomPosition();
        ptrPredator = dynamic_cast<Predator*>(new PredatorEsp(ptrGridWorld, 1, randomPosition,
                espNetwork));
        LOG(INFO) << "[CREATED] Predator at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 1 << endl;
        // initialize prey
        randomPosition = ptrGridWorld->getRandomPosition();
        this->ptrOtherAgent = new T(ptrGridWorld, 2, randomPosition, agentMoveProb);
        LOG(INFO) << "Agent move probability is " << agentMoveProb;
        LOG(INFO) << "[CREATED] Agent at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 2 << endl;
    }

    template<class T>
    void DomainOne<T>::enableDisplay(const vector<double>& predatorColour,
            const vector<double>& preyColour)
    {
        this->displayEnabled = true;
        LOG(INFO) << "Display enabled";
        map<int, vector<double> > idColorMapping = map<int, vector<double> >();
        pair<int, vector<double> > p = pair<int, vector<double> >();

        p = pair<int, vector<double> >(1, predatorColour);
        idColorMapping.insert(p);

        p = pair<int, vector<double> >(2, preyColour);
        idColorMapping.insert(p);

        visualizer = Visualizer(idColorMapping, ptrGridWorld->width, ptrGridWorld->height);
    }

    template<class T>
    void DomainOne<T>::step()
    {
        AgentInformation aiPredator, aiOtherAgent;
        aiPredator = ptrPredator->getAgentInformation();
        aiOtherAgent = ptrOtherAgent->getAgentInformation();

        // build vector<AgentInformation>
        vector<AgentInformation> vAgentInformation;
        vAgentInformation.clear();
        vAgentInformation.push_back(aiPredator);
        vAgentInformation.push_back(aiOtherAgent);

        // move prey
        ptrOtherAgent->move(vAgentInformation);
        // move predator
        ptrPredator->move(vAgentInformation);

        VLOG(2) << "Predator at " << aiPredator.position.x << "," << aiPredator.position.y;
        VLOG(2)
                << "Other agent at " << aiOtherAgent.position.x << "," << aiOtherAgent.position.y;

        if ((aiOtherAgent.position.x == aiPredator.position.x)
                && (aiOtherAgent.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Prey caught by predator or predator caught by hunter";
            if (dynamic_cast<T*>(ptrOtherAgent)->getAgentInformation().agentId == PREY) {     // Yay
                agentCaughtIds.push_back(aiOtherAgent.agentId);
                LOG(INFO) << "Prey caught by predator";
            } else if (ptrOtherAgent->getAgentInformation().agentId == HUNTER) {
                agentCaughtIds.push_back(aiPredator.agentId);
                LOG(INFO) << "Predator caught by hunter";
            }
            numAgentsCaught++;
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

    template<class T>
    double DomainOne<T>::run()
    {
        uint noSteps = 0;
        uint prevNumAgentsCaught = 0;
        numAgentsCaught = 0;
        agentCaughtIds = vector<uint>();

        do {
            VLOG(1) << "Step: " << noSteps << endl;
            (void) step();
            if (numAgentsCaught > 0 && numAgentsCaught > prevNumAgentsCaught) {
                LOG(INFO) << "CAUGHT!!!!" << endl;
                prevNumAgentsCaught = numAgentsCaught;
                if (ptrOtherAgent->getAgentInformation().agentId == PREY) {
                    if (numAgentsCaught == numAgents) {
                        LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                        return calculateFitness(noSteps);
                    }
                } else if (ptrOtherAgent->getAgentInformation().agentId == HUNTER) {
                    if (numAgentsCaught == numPredators) {
                        LOG(INFO) << "All predators caught in " << noSteps << " steps!";
                        return calculateFitness(noSteps);
                    }
                }
            }
        } while (noSteps++ < maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return calculateFitness(noSteps);
    }

    template<class T>
    double DomainOne<T>::calculateFitness(const uint& stepCurrent)
    {
        double fitness = 0.0;
        if (numAgentsCaught > 0) {
            if (ptrOtherAgent->getAgentInformation().agentId == PREY) {     // Yay
                fitness = static_cast<double>(10) * (maxSteps - stepCurrent) * numAgentsCaught;
            } else if (ptrOtherAgent->getAgentInformation().agentId == HUNTER) {
                fitness = static_cast<double>(-1) * 10 * (maxSteps - stepCurrent) * numAgentsCaught;
            }
            return fitness;
        } else {
            // calculate distance from hunter and prey
            Position positionPredator = ptrPredator->getPosition();
            Position positionOtherAgent = ptrOtherAgent->getPosition();
            uint distanceOtherAgent;
            distanceOtherAgent = ptrGridWorld->distance(positionOtherAgent, positionPredator);
            // reward for being close to prey and far away from hunter
            distanceOtherAgent = ptrGridWorld->getWidth() + ptrGridWorld->getHeight()
                    - distanceOtherAgent;
            // take into account the size of the grid for rewarding later
            if (ptrOtherAgent->getAgentInformation().agentId == PREY) {
                distanceOtherAgent = ptrGridWorld->getWidth() + ptrGridWorld->getHeight()
                        - distanceOtherAgent;
                fitness = static_cast<double>(distanceOtherAgent);
            } else if (ptrOtherAgent->getAgentInformation().agentId == HUNTER) {
                fitness = static_cast<double>(distanceOtherAgent);
            }
            return fitness;
        }
    }

    template class DomainOne<Prey>;
    template class DomainOne<Hunter>;
}

