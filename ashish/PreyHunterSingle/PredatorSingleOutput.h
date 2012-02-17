#ifndef PPH_PREDATOR_SINGLE_OUTPUT_H
#define PPH_PREDATOR_SINGLE_OUTPUT_H

#include "Predator.h"
#include "network.h"

namespace PredatorPreyHunter {
  class PredatorSingleOutput : public Predator {
  protected:
    NEAT::Network* ptrNetworkHigher; // higher network which decided to whom to give control to
    NEAT::Network* ptrNetworkPrey; // network trained only with prey
    NEAT::Network* ptrNetworkHunter; // network trained only with hunter
  protected:
    int compute( NEAT::Network* ptrNetwork, const std::vector<AgentInformation>& vAgentInformation );
  public:
    PredatorSingleOutput( 
        const GridWorld* ptrGridWorld,
        const uint& agentId,
        const Position& p,
        NEAT::Network* ptrNetworkHigher,
        NEAT::Network* ptrNetworkPrey,
        NEAT::Network* ptrNetworkHunter
    ); 
    virtual Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}

#endif // PPH_PREDATOR_SINGLE_OUTPUT_H  
