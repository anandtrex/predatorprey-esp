#ifndef PPH_HUNTER_H
#define PPH_HUNTER_H

#include "Agent.h"

namespace PredatorPreyHunter
{
    class Hunter: public Agent
    {
        double moveProbability;
        double roleReversalProbability;
        Position moveHunter(const std::vector<AgentInformation>& vAgentInformation);
        Position movePrey(const std::vector<AgentInformation>& vAgentInformation);
    public:
        Hunter(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
                const double& moveProbability);
        Hunter(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
                        const double& moveProbability, const double& roleReversalProbability);
        Position move(const std::vector<AgentInformation>& vAgentInformation);
    };
}

#endif // PPH_HUNTER_H
