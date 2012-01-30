/*
 * PredatorNaive.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORNAIVE_H_
#define PREDATORNAIVE_H_

#include "Predator.h"

namespace PredatorPreyHunter
{
    /**
     * This predator moves towards the prey with the given probability, the movement being random otherwise
     */
    class PredatorNaive: public Predator
    {
        double moveProbability;
    public:
        PredatorNaive(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, const double& moveProbability )
                : Predator(ptrGridWorld, agentId, p)
        {
            this->moveProbability = moveProbability;
        }
        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif /* PREDATORNAIVE_H_ */
