#ifndef PPH_AGENT_H
#define PPH_AGENT_H

#include "Basic.h"
#include "GridWorld.h"
#include <vector>

namespace PredatorPreyHunter {
  class Agent {
  protected:
    const GridWorld* ptrGridWorld;
    uint agentId;
    TypeAgent typeAgent;
    Position position;
  public:
    Agent( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p );
    virtual Position move( const std::vector<AgentInformation>& vAgentInformation ) = 0;
    AgentInformation getAgentInformation();
    Position getPosition() { return position; }
    TypeAgent getType() { return typeAgent; }
  };
}

#endif // PPH_AGENT_H

