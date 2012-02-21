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

    AgentInformation Agent::getAgentInformation()
    {
        AgentInformation agentInformation;
        agentInformation.agentId = agentId;
        agentInformation.agentType = typeAgent;
        agentInformation.position = position;
        return agentInformation;
    }
}
