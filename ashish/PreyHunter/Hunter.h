#ifndef PPH_HUNTER_H
#define PPH_HUNTER_H

#include "Agent.h"

namespace PredatorPreyHunter {
  class Hunter : public Agent {
    public:
      Hunter( const GridWorld* ptrGridWorld, const uint& agentId, const TypeAgent& typeAgent, const Position& p ); 
      Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}

#endif // PPH_HUNTER_H
