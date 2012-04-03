/*
 * PredatorEsp.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef PREDATORESP_H_
#define PREDATORESP_H_

#include "Predator.h"
#include "../Esp/NetworkContainer.h"

namespace PredatorPreyHunter
{
    using EspPredPreyHunter::NetworkContainer;
    using std::vector;
    using std::string;

    /**
     * This predator moves towards the prey with the given probability, the movement being random otherwise
     */
    class PredatorEsp: public Predator
    {
        NetworkContainer* networkContainer;
        string lastSelection;

    public:
        PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, NetworkContainer *network);
        Position move(const std::vector<AgentInformation>& vAgentInformation, const uint& stepNo = -1);
        string getLastNetworkSelection()
        {
            return lastSelection;
        }
    };
}

#endif /* PREDATORESP_H_ */
