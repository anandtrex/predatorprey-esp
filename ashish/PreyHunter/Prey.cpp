#include "Prey.h"
#include <iostream>
#include <stdlib.h> // for drand48(): note unix only
#include <cstdlib>

namespace PredatorPreyHunter {
  using std::vector;
  using std::cerr;
  using std::endl;
  using std::abs;
  Prey::Prey( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p ) : Agent( ptrGridWorld, agentId, p ) {
    this->typeAgent = PREY;
    moveProbability = 0.9; // fetch this using a constructor later
  }
  Position Prey::move( const std::vector<AgentInformation>& vAgentInformation ) {
    typedef vector<AgentInformation>::const_iterator VAICI;
    // if the random number between 0 and 1 is greater than the moveProbability
    // then don't move
    if ( fetchRandomNumber() > moveProbability ) { 
      return this->position;
    }
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
    // the following code has been taken from Padmini and Aditya
    // uses the same conventions
    const int MAP_LENGTH = ptrGridWorld->getWidth();
    const int MAP_HEIGHT = ptrGridWorld->getHeight();
    int x_dist = itPredatorClosest->position.x - this->position.x; // NOTE: Predator - Prey
    int temp;
    if ( ( abs( x_dist ) ) > ( MAP_LENGTH / 2 ) ) { // account for the toroid
        temp = x_dist;
        x_dist = MAP_LENGTH  - abs(x_dist);
        if (temp > 0)
          x_dist = 0 - x_dist;         // account for who is behind who
    }
    // do the same thing for y
    int y_dist = itPredatorClosest->position.y - this->position.y; // NOTE: Predator - Prey
    if ( ( abs( y_dist ) ) > ( MAP_HEIGHT / 2 ) ) {
        temp = y_dist;
        y_dist = MAP_HEIGHT  - abs(y_dist);
        if (temp > 0)
            y_dist = 0 - y_dist;
    }
    Action preyAction;
    if (y_dist<0 && (abs(y_dist)>=abs(x_dist))) {		
        preyAction = NORTH;
    }
    else if (x_dist<0 && (abs(x_dist)>=abs(y_dist))) {
        preyAction = EAST;
    }
    else if (y_dist>0 && (abs(y_dist)>=abs(x_dist))) {
        preyAction = SOUTH;
    }
    else if (x_dist>0 && (abs(x_dist)>=abs(y_dist))) {
        preyAction = WEST;
    }
    else {
        preyAction = STAY;
    }
    this->position = ptrGridWorld->move( this->position, preyAction ); 
  }
}

