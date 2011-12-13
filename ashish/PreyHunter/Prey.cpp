#include "Prey.h"
#include <iostream>

namespace PredatorPreyHunter {
  using std::vector;
  using std::cerr;
  using std::endl;
  Prey::Prey( const GridWorld* ptrGridWorld, const uint& agentId, const TypeAgent& typeAgent, const Position& p ) : Agent( ptrGridWorld, agentId, p ) {
    this->typeAgent = typeAgent;
  }
  Position Prey::move( const std::vector<AgentInformation>& vAgentInformation ) {
    typedef vector<AgentInformation>::const_iterator VAICI;
    const int BIG_DISTANCE = ptrGridWorld->getWidth() + ptrGridWorld->getHeight();
    // find the closest predator
    int distMin, dist;
    distMin = BIG_DISTANCE; 
    VAICI itPredatorClosest = vAgentInformation.end();
    for ( VAICI it = vAgentInformation.begin(); it != vAgentInformation.end(); ++it ) {
      if ( PREDATOR == it->typeAgent ) {
        dist = ptrGridWorld->distance( this->position, it->position ); 
        if ( dist < distMin ) {
          distMin = dist;
          itPredatorClosest = it;
        }
      }
    }
    if ( BIG_DISTANCE == distMin || vAgentInformation.end() == itPredatorClosest ) {
      cerr << "Houston, we have a problem" << endl;
      cerr << "Prey::move could not find the nearest predator." << endl;
      throw 1; // throw something meaningful later
    }
    // move away from the closest predator
    
  }
}

