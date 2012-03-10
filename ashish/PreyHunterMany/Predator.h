#ifndef PPH_PREDATOR_H
#define PPH_PREDATOR_H

#include "Agent.h"
#include "network.h"

namespace PredatorPreyHunter {
  class Predator : public Agent {
    NEAT::Network* ptrNetwork;
    uint noOtherAgents;
    protected:
      struct Input {
        double xRelative;
        double yRelative;
        double type;
      };
      Input prepareInput( const AgentInformation& agentInformation );
    public:
      Predator( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, NEAT::Network* ptrNetwork, const uint& noOtherAgents ); 
      Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}

#endif // PPH_PREDATOR_H
