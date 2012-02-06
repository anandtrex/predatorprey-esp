/*
 * PredatorEsp.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORESP_H_
#define PREDATORESP_H_

#include "Predator.h"

namespace PredatorPreyHunter
{
    /**
     * This predator moves towards the prey with the given probability, the movement being random otherwise
     */
    class PredatorEsp: public Predator
    {
        double moveProbability;
    public:
        PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, const double& moveProbability )
                : Predator(ptrGridWorld, agentId, p)
        {
            this->moveProbability = moveProbability;
        }
        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}


#endif /* PREDATORESP_H_ */
