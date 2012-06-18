/*
 * PredatorRandom.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORRANDOM_H_
#define PREDATORRANDOM_H_

#include "Predator.h"

namespace PredatorPreyHunter
{
    /**
     * This predator moves completely randomly
     */
    class PredatorRandom: public Predator
    {
    public:
        PredatorRandom(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p)
            :Predator(ptrGridWorld, agentId, p)
        {

        }

        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif /* PREDATORRANDOM_H_ */
