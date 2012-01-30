#ifndef PPH_PREY_H
#define PPH_PREY_H

#include "Agent.h"

namespace PredatorPreyHunter {
  class Prey : public Agent {
      double moveProbability;
    public:
      Prey( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, const double& moveProbability );
      Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}

#endif // PPH_PREY_H
