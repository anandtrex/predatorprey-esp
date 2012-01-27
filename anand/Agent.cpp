/*
 * Agent.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include "Agent.h"
#include <iostream>

namespace PredatorPreyHunter
{
    using std::endl;
    Agent::Agent(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p)
    {
        if (ptrGridWorld == NULL) {
            LOG(ERROR) << "Agent::constructor received a NULL grid world pointer" << endl;
        }
        this->ptrGridWorld = ptrGridWorld;
        this->agentId = agentId;
    }
    AgentInformation Agent::getAgentInformation()
    {
        AgentInformation agentInformation;
        agentInformation.agentId = this->agentId;
        agentInformation.typeAgent = this->typeAgent;
        agentInformation.position = this->position;
        return agentInformation;
    }
    Agent::~Agent()
    {

    }
}
