/*
 * Agent.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "GridWorld.h"

enum TypeAgent
{
    PREY = 0, PREDATOR = 1, HUNTER = 2, HUNTER_WEAK = 3
};

struct AgentInformation
{
    uint agentId;
    TypeAgent agentType;
    Position position;
};

namespace PredatorPreyHunter
{
    class Agent
    {
    protected:
        const GridWorld* ptrGridWorld;
        const uint agentId;
        TypeAgent typeAgent;
        Position position;
    public:
        Agent(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p)
            :ptrGridWorld(ptrGridWorld), agentId(agentId), position(p)
        {
        }
        virtual ~Agent()
        {
            delete ptrGridWorld;
        }

        virtual Position move(const std::vector<AgentInformation>& vAgentInformation, const uint& stepNo) = 0;
        AgentInformation getAgentInformation();
        Position getPosition()
        {
            return position;
        }
    };
}

#endif /* AGENT_H_ */
