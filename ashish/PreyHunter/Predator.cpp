#include "Predator.h"
#include <iostream>
#include <cstdlib>

namespace PredatorPreyHunter {
  using std::vector;
  using std::cerr;
  using std::endl;
  using std::abs;
  Predator::Predator( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p ) : Agent( ptrGridWorld, agentId, p ) {
    this->typeAgent = PREDATOR;
  }
  Position Predator::move( const std::vector<AgentInformation>& vAgentInformation ) {
    Action predatorAction = NORTH;
    this->position = ptrGridWorld->move( this->position, predatorAction ); 
  }
}

