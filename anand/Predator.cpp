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
    // random predator
    Action predatorAction;
    double moveProbability = 0.5;
    if ( fetchRandomNumber() > moveProbability ) {
      predatorAction = NORTH;
    } else {
      int choice = static_cast<int> ( fetchRandomNumber() * 5 );
      switch( choice ) {
        case 0:
          predatorAction = NORTH;
          break;
        case 1:
          predatorAction = EAST;
          break;
        case 2:
          predatorAction = SOUTH;
          break;
        case 3:
          predatorAction = WEST;
          break;
        default:
          predatorAction = STAY;
      }
    }
    this->position = ptrGridWorld->move( this->position, predatorAction ); 
    return this->position;
  }
}

