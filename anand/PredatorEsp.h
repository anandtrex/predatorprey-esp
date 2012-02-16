/*
 * PredatorEsp.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORESP_H_
#define PREDATORESP_H_

#include "Predator.h"
#include "Esp.h"
#include "NetworkContainer.h"

namespace PredatorPreyHunter
{
    using EspPredPreyHunter::NetworkContainer;
    using std::vector;

    /**
     * This predator moves towards the prey with the given probability, the movement being random otherwise
     */
    class PredatorEsp: public Predator
    {
        NetworkContainer* networkContainer;
        // TODO Take this as a parameter to Predator class
        uint getMaxIndex(const vector<double>& vec);
    public:
        PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, NetworkContainer *network);
        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif /* PREDATORESP_H_ */
