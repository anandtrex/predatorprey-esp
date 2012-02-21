#ifndef PPH_HUNTER_H
#define PPH_HUNTER_H

#include "Agent.h"

namespace PredatorPreyHunter
{
    class Hunter: public Agent
    {
        double moveProbability;
    public:
        Hunter(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
                const double& moveProbability);
        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif // PPH_HUNTER_H
