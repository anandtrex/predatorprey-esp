#ifndef PPH_PREDATOR_H
#define PPH_PREDATOR_H

#include "Agent.h"

namespace PredatorPreyHunter {
  class Predator : public Agent {
    public:
      Predator( const GridWorld* ptrGridWorld, const uint& agentId, const TypeAgent& typeAgent, const Position& p ); 
      Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}

#endif // PPH_PREDATOR_H
