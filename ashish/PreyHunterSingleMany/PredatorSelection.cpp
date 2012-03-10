#include "Basic.h"
#include "PredatorSelection.h"
#include "nnode.h"
#include <iostream>
#include <cstdlib>

namespace PredatorPreyHunter {
  using std::vector;
  using std::cerr;
  using std::endl;
  using std::abs;
  PredatorSelection::PredatorSelection (
      const GridWorld* ptrGridWorld,
      const uint& agentId,
      const Position& p,
      NEAT::Network* ptrNetworkHigher,
      NEAT::Network* ptrNetworkPrey,
      NEAT::Network* ptrNetworkHunter
    ) : Predator( ptrGridWorld, agentId, p, NULL ) {
    this->typeAgent = PREDATOR;
    this->ptrNetworkHigher = ptrNetworkHigher;
    this->ptrNetworkPrey = ptrNetworkPrey;
    this->ptrNetworkHunter = ptrNetworkHunter;
  }

  PredatorSelection::Input PredatorSelection::prepareInput( const AgentInformation& agentInformation ) {
    // borrowed from Aditya and Padmini
    int x_dist, y_dist;
    x_dist = agentInformation.position.x - this->position.x;
    if ( abs( x_dist ) > ( ptrGridWorld->getWidth() / 2 ) ) {
      int temp = x_dist;
      x_dist = ptrGridWorld->getWidth() - abs( x_dist );
      if ( temp > 0 )
        x_dist = 0 - x_dist;
    }
    y_dist = agentInformation.position.y - this->position.y;
    if ( abs( y_dist ) > ( ptrGridWorld->getHeight() / 2 ) ) {
      int temp = y_dist;
      y_dist = ptrGridWorld->getHeight() - abs( y_dist );
      if ( temp > 0 )
        y_dist = 0 - y_dist;
    }
    // set the inputs for prey
    Input in;
    in.xRelative = static_cast<double> ( x_dist ) / static_cast<double> ( ptrGridWorld->getWidth() ); // relative x position
    in.yRelative = static_cast<double> ( y_dist ) / static_cast<double> ( ptrGridWorld->getHeight() );// relative y position
    switch( agentInformation.typeAgent ) {
      case PREY:
        in.type = 0.0; // This is arbitrary. 0.0 for prey; 1.0 for hunter
        break;
      case HUNTER:
        in.type = 1.0; // This is arbitrary. 0.0 for prey; 1.0 for hunter
        break;
    }
    return in;
  }

  int PredatorSelection::computeHigher( NEAT::Network* ptrNetworkCurrent, const std::vector<AgentInformation>& vAgentInformation ) {
    // Note: variable name ptrNetworkCurrent has been chosen over ptrNetwork
    // because ptrNetwork already exists (derived from Predator). I know
    // ptrNetwork in this context is an automatic variable and takes precedence
    // but we do not want strange undebuggable errors now do we.
    // prepare inputs for network
    // assuming node order
    // 1 bias node
    // 2 relative agent position x
    // 3 relative agent position y
    // 4 type
    // . relative agent position x
    // . relative agent position y
    // . type
    double* in = new double[ 1 + noOtherAgents * 3 ]; // be careful about the starter genome that you feed
    // you can do a check to confirm if the number of input nodes is equal to
    // the number of output nodes
    in[0] = 1.0; // BIAS
    // get the relative prey position
    vector<Input> vInput;
    for ( vector<AgentInformation>::const_iterator it = vAgentInformation.begin(); it != vAgentInformation.end(); ++it ) {
      if ( PREDATOR != it->typeAgent ) {
        Input input = prepareInput( *it ); 
        vInput.push_back( input );
      }
    }
    if ( noOtherAgents != vInput.size() ) {
      cerr << "Number of agents is not as specified!" << endl;
      throw 1; // throw something meaningful later
    }
    uint index = 1;
    for ( vector<Input>::const_iterator it = vInput.begin(); it != vInput.end(); ++it ) {
      in[ index ] = it->xRelative; index++;
      in[ index ] = it->yRelative; index++;
      in[ index ] = it->type; index++;
    }
    // activate network
    ptrNetworkCurrent->load_sensors( in ); // feed the input values to the network
    if ( !( ptrNetworkCurrent->activate() ) ) {
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
    itPtrNNode = ptrNetworkCurrent->outputs.begin(); 
    double greatest = (*itPtrNNode)->activation;
    uint mostActive = 0; 
    int count = 1; ++itPtrNNode; // move to the next output node
    while( itPtrNNode != ptrNetworkCurrent->outputs.end() ) {
      if ( greatest < (*itPtrNNode)->activation ) {
        mostActive = count;
        greatest = (*itPtrNNode)->activation;
      }
      count++; ++itPtrNNode;
    }
    int choice = static_cast<int> ( mostActive ); // there is no need for this variable but I am not changing code now
    return choice;
  }
  int PredatorSelection::computePrey( NEAT::Network* ptrNetwork, const std::vector<AgentInformation>& vAgentInformation ) {
    // prepare inputs for network
    // assuming node order
    // 1 bias node
    // 2 relative prey position x
    // 3 relative prey position y
    // 4 prey type
    double in[4]; // be careful about the starter genome that you feed
    // you can do a check to confirm if the number of input nodes is equal to
    // the number of output nodes
    in[0] = 1.0; // BIAS
    // get the relative prey position
    Position positionPrey; bool foundPrey = false;
    for ( vector<AgentInformation>::const_iterator it = vAgentInformation.begin(); it != vAgentInformation.end(); ++it ) {
      if ( PREY == it->typeAgent ) {
        positionPrey = it->position;
        foundPrey = true; // this is just a last minute check
      }
    }
    if ( !foundPrey ) {
      cerr << "PredatorSelection::computePrey() I could not find the prey" << endl;
      throw 1; // throw something meaningful later
    }
    // borrowed from Aditya and Padmini
    int x_dist, y_dist;
    x_dist = positionPrey.x - this->position.x;
    if ( abs( x_dist ) > ( ptrGridWorld->getWidth() / 2 ) ) {
      int temp = x_dist;
      x_dist = ptrGridWorld->getWidth() - abs( x_dist );
      if ( temp > 0 )
        x_dist = 0 - x_dist;
    }
    y_dist = positionPrey.y - this->position.y;
    if ( abs( y_dist ) > ( ptrGridWorld->getHeight() / 2 ) ) {
      int temp = y_dist;
      y_dist = ptrGridWorld->getHeight() - abs( y_dist );
      if ( temp > 0 )
        y_dist = 0 - y_dist;
    }
    // set the inputs for prey
    in[1] = static_cast<double> ( x_dist ) / static_cast<double> ( ptrGridWorld->getWidth() ); // relative x position
    in[2] = static_cast<double> ( y_dist ) / static_cast<double> ( ptrGridWorld->getHeight() );// relative y position
    // CAREFUL about in[3]
    in[3] = 0.0; // This is arbitrary. 0.0 for prey; 1.0 for hunter
  
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
    return choice;
  }
  int PredatorSelection::computeHunter( NEAT::Network* ptrNetwork, const std::vector<AgentInformation>& vAgentInformation ) {
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
    return choice;
  }
  Position PredatorSelection::move( const std::vector<AgentInformation>& vAgentInformation ) {
    Action predatorAction;
    int choiceHigher = computeHigher( ptrNetworkHigher, vAgentInformation ); 
    int choice = -1;
    switch ( choiceHigher ) {
      case 0:
        choice = computePrey( ptrNetworkPrey, vAgentInformation );
        break;
      case 1:
        choice = computeHunter( ptrNetworkHunter, vAgentInformation );
        break;
      default:
        cerr << "Value computed by ptrNetworkHigher is NOT 0 or 1." << endl;
        cerr << "mostActive: " << choice << endl;
        throw 1; // throw something meaningful later
    }
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
        cerr << "mostActive: " << choice << endl;
        throw 1; // throw something meaningful later
    }
    this->position = ptrGridWorld->move( this->position, predatorAction ); 
    return this->position;
  }
}

