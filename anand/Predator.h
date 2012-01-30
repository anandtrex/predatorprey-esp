/*
 * Predator.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATOR_H_
#define PREDATOR_H_

#include "Agent.h"

namespace PredatorPreyHunter
{
    class Predator: public Agent
    {
    public:
        Predator(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p)
                : Agent(ptrGridWorld, agentId, p)
        {
            this->typeAgent = PREDATOR;
        }
        virtual Position move(const std::vector<AgentInformation>& vAgentInformation) = 0;
    };
}

#endif /* PREDATOR_H_ */
