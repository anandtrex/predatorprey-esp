/*
 * Agent.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "GridWorld.h"
#include "common.h"

enum TypeAgent
{
    PREY = 0, PREDATOR = 1, HUNTER = 2
};

struct AgentInformation
{
    uint agentId;
    TypeAgent typeAgent;
    Position position;
};

namespace PredatorPreyHunter
{
    class Agent
    {
    protected:
        const GridWorld* ptrGridWorld;
        uint agentId;
        TypeAgent typeAgent;
        Position position;
    public:
        Agent(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p);
        virtual ~Agent();
        virtual Position move(const std::vector<AgentInformation>& vAgentInformation) = 0;
        AgentInformation getAgentInformation();
        Position getPosition()
        {
            return position;
        }
    };
}

#endif /* AGENT_H_ */
