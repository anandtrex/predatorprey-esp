#include "Predator.h"
#include "nnode.h"
#include <iostream>
#include <cstdlib>

namespace PredatorPreyHunter {
  using std::vector;
  using std::cerr;
  using std::endl;
  using std::abs;
  Predator::Predator( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p, NEAT::Network* ptrNetwork ) : Agent( ptrGridWorld, agentId, p ) {
    this->typeAgent = PREDATOR;
    this->ptrNetwork = ptrNetwork;
  }
  Position Predator::move( const std::vector<AgentInformation>& vAgentInformation ) {
    // prepare inputs for network
    // assuming node order
    // 1 bias node
    // 2 relative hunter position x
    // 3 relative hunter position y
    // 4 hunter type
    double in[4]; // be careful about the starter genome that you feed
    // you can do a check to confirm if the number of input nodes is equal to
    // the number of output nodes
    in[0] = 1.0; // BIAS
    // get the relative hunter position
    Position positionHunter; bool foundHunter = false;
    for ( vector<AgentInformation>::const_iterator it = vAgentInformation.begin(); it != vAgentInformation.end(); ++it ) {
      if ( HUNTER == it->typeAgent ) {
        positionHunter = it->position;
        foundHunter = true; // this is just a last minute check
      }
    }
    if ( !foundHunter ) {
      cerr << "Predator::move() I could not find the hunter" << endl;
      throw 1; // throw something meaningful later
    }
    // borrowed from Aditya and Padmini
    int x_dist, y_dist;
    x_dist = positionHunter.x - this->position.x;
    if ( abs( x_dist ) > ( ptrGridWorld->getWidth() / 2 ) ) {
      int temp = x_dist;
      x_dist = ptrGridWorld->getWidth() - abs( x_dist );
      if ( temp > 0 )
        x_dist = 0 - x_dist;
    }
    y_dist = positionHunter.y - this->position.y;
    if ( abs( y_dist ) > ( ptrGridWorld->getHeight() / 2 ) ) {
      int temp = y_dist;
      y_dist = ptrGridWorld->getHeight() - abs( y_dist );
      if ( temp > 0 )
        y_dist = 0 - y_dist;
    }
    // set the inputs for hunter
    in[1] = static_cast<double> ( x_dist ) / static_cast<double> ( ptrGridWorld->getWidth() ); // relative x position
    in[2] = static_cast<double> ( y_dist ) / static_cast<double> ( ptrGridWorld->getHeight() );// relative y position
    // CAREFUL about in[6]
    in[3] = 1.0; // This is arbitrary. 0.0 for prey; 1.0 for hunter

    // activate network
    ptrNetwork->load_sensors( in ); // feed the input values to the network
    if ( !( ptrNetwork->activate() ) ) {
      cerr << "[CAUTION] Predator::move() Network not activating!" << endl;
      cerr << "CHANGE CODE TO KEEP CONTINUING INSPITE OF FAILURE" << endl;
      // you might want to keep continuing because faulty networks are possible
      throw 1; // throw more meaningful exception here
    }
    vector<NEAT::NNode*>::iterator itPtrNNode;
    // Algorithm
    // get the activation value of the first output node
    // compare it with the rest to determine which has the highest activation
    // value
    // Note the position of the node with the highest activation value and
    // decide your action accordingly
    itPtrNNode = ptrNetwork->outputs.begin(); 
    double greatest = (*itPtrNNode)->activation;
    uint mostActive = 0; 
    int count = 1; ++itPtrNNode; // move to the next output node
    while( itPtrNNode != ptrNetwork->outputs.end() ) {
      if ( greatest < (*itPtrNNode)->activation ) {
        mostActive = count;
        greatest = (*itPtrNNode)->activation;
      }
      count++; ++itPtrNNode;
    }
    Action predatorAction;
    int choice = static_cast<int> ( mostActive ); // there is no need for this variable but I am not changing code now
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
      case 4:
        predatorAction = STAY;
        break;
      default:
        cerr << "Predator::move() predatorAction is not within 0 to 4. Implies more than 5 outputs" << endl;
        cerr << "mostActive: " << mostActive << endl;
        throw 1; // throw something meaningful later
    }
    this->position = ptrGridWorld->move( this->position, predatorAction ); 
    return this->position;
  }
}

