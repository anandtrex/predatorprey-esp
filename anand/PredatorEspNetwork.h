/*
 * PredatorEsp.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORESP_H_
#define PREDATORESP_H_

#include "Predator.h"
#include "EspExperiment.h"

namespace PredatorPreyHunter
{
    using EspPredPreyHunter::EspExperiment;

    /**
     * This predator moves towards the prey with the given probability, the movement being random otherwise
     */
    class PredatorEspNetwork: public Predator
    {
        double moveProbability;
        Experiment experiment;
        EspExperiment esp;
    public:
        PredatorEspNetwork(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
                const double& moveProbability, const Experiment& experiment, const uint& numTeamAgents,
                const uint& numHiddenNeurons, const uint& popSize, const uint& netTp,
                const uint& numOtherAgents, const uint& numActions);

        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif /* PREDATORESP_H_ */
