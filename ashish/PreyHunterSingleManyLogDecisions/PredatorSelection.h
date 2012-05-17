#ifndef PPH_PREDATOR_SELECTION
#define PPH_PREDATOR_SELECTION

#include "Predator.h"
#include "network.h"
#include <iostream>

namespace PredatorPreyHunter {
  class PredatorSelection : public Predator {
  protected:
    NEAT::Network* ptrNetworkHigher; // higher network which decided to whom to give control to
    NEAT::Network* ptrNetworkPrey;   // network trained only with prey
    NEAT::Network* ptrNetworkHunter; // network trained only with hunter
    uint noOtherAgents;
  protected:
    int computeHigher( NEAT::Network* ptrNetwork, const std::vector<AgentInformation>& vAgentInformation );
    int computePrey( NEAT::Network* ptrNetwork, const AgentInformation& agentInformation );
    int computeHunter( NEAT::Network* ptrNetwork, const AgentInformation& agentInformation );
    struct Input {
      double xRelative;
      double yRelative;
      double type;
    };
    Input prepareInput( const AgentInformation& agentInformation );
  public:
    PredatorSelection( 
        const GridWorld* ptrGridWorld,
        const uint& agentId,
        const Position& p,
        NEAT::Network* ptrNetworkHigher,
        NEAT::Network* ptrNetworkPrey,
        NEAT::Network* ptrNetworkHunter,
        const uint& noOtherAgents
    ); 
    virtual Position move( const std::vector<AgentInformation>& vAgentInformation );
    virtual Position move( const std::vector<AgentInformation>& vAgentInformation, std::ostream& out );
  };
}

#endif // PPH_PREDATOR_SELECTION
