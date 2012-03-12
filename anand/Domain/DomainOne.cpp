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
            const double& agentMoveProb)
            : Domain(maxSteps, width, height), agentMoveProb(agentMoveProb)
    {
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

        // initialize other agent
        randomPosition = ptrGridWorld->getRandomPosition();
        this->ptrOtherAgent = new T(ptrGridWorld, 2, randomPosition, agentMoveProb);
        LOG(INFO) << "Agent move probability is " << agentMoveProb;
        LOG(INFO) << "[CREATED] Agent at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 2 << endl;
        LOG(INFO) << "Agent type is " << ptrOtherAgent->getAgentInformation().agentType;
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
    void DomainOne<T>::step(const uint& stepNo)
    {
        if (stepNo > maxSteps)
            LOG(FATAL) << "Step no is greater than maxSteps!";
        AgentInformation aiPredator, aiOtherAgent;
        aiPredator = ptrPredator->getAgentInformation();
        aiOtherAgent = ptrOtherAgent->getAgentInformation();

        // build vector<AgentInformation>
        vector<AgentInformation> vAgentInformation;
        vAgentInformation.push_back(aiPredator);
        vAgentInformation.push_back(aiOtherAgent);

        // move prey
        ptrOtherAgent->move(vAgentInformation, stepNo);
        // move predator
        ptrPredator->move(vAgentInformation, stepNo);

        VLOG(2) << "Predator at " << aiPredator.position.x << "," << aiPredator.position.y;
        VLOG(2)
                << "Other agent at " << aiOtherAgent.position.x << "," << aiOtherAgent.position.y;

        if ((aiOtherAgent.position.x == aiPredator.position.x)
                && (aiOtherAgent.position.y == aiPredator.position.y)) {
            if (ptrOtherAgent->getAgentInformation().agentType == PREY) {     // Yay
                agentCaughtId = aiOtherAgent.agentId;
                LOG(INFO) << "Prey caught by predator";
                LOG(INFO) << "Predator was at " << aiPredator.position.x << ", " << aiPredator.position.y;
                LOG(INFO) << "Prey was at " << aiOtherAgent.position.x << ", " << aiOtherAgent.position.y;
            } else if (ptrOtherAgent->getAgentInformation().agentType == HUNTER) {
                agentCaughtId = aiPredator.agentId;
                LOG(INFO) << "Predator caught by hunter";
                LOG(INFO) << "Predator was at " << aiPredator.position.x << ", " << aiPredator.position.y;
                LOG(INFO) << "Hunter was at " << aiOtherAgent.position.x << ", " << aiOtherAgent.position.y;
            }
            agentCaught = true;
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
        uint noSteps = 1;
        agentCaught = false;

        do {
            VLOG(1) << "Step: " << noSteps << endl;
            (void) step(noSteps);
            if (agentCaught) {
                LOG(INFO) << "CAUGHT!!!!" << endl;
                if (ptrOtherAgent->getAgentInformation().agentType == PREY) {
                    LOG(INFO) << "Prey caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                } else if (ptrOtherAgent->getAgentInformation().agentType == HUNTER) {
                    LOG(INFO) << "Predator caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            }
        } while (++noSteps <= maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return calculateFitness(noSteps - 1);
    }

    template<class T>
    double DomainOne<T>::calculateFitness(const uint& stepCurrent)
    {
        if (stepCurrent > maxSteps) {
            LOG(FATAL) << "Step no is greater than maxSteps!" << " stepCurrent is "
                    << stepCurrent << " and maxSteps is " << maxSteps;
        }

        double fitness = 0.0;
        if (agentCaught) {
            if (ptrOtherAgent->getAgentInformation().agentType == PREY) {     // Yay
                fitness = static_cast<double>(10) * (maxSteps - stepCurrent);
            } else if (ptrOtherAgent->getAgentInformation().agentType == HUNTER) {
                fitness = static_cast<double>(-1) * 10 * (maxSteps - stepCurrent);
            }
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        } else {
            // calculate distance from hunter and prey
            Position positionPredator = ptrPredator->getPosition();
            Position positionOtherAgent = ptrOtherAgent->getPosition();
            uint distanceOtherAgent;
            distanceOtherAgent = ptrGridWorld->distance(positionOtherAgent, positionPredator);

            // take into account the size of the grid for rewarding later
            if (ptrOtherAgent->getAgentInformation().agentType == PREY) {
                distanceOtherAgent = ptrGridWorld->getWidth() + ptrGridWorld->getHeight()
                        - distanceOtherAgent;
                fitness = static_cast<double>(distanceOtherAgent);
            } else if (ptrOtherAgent->getAgentInformation().agentType == HUNTER) {
                fitness = static_cast<double>(distanceOtherAgent);
            }
            LOG(INFO) << "Returned fitness is " << fitness;
            return fitness;
        }
    }

    template class DomainOne<Prey> ;
    template class DomainOne<Hunter> ;
}

