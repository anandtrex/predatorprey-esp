#include "Domain.h"
#include <iostream>
#include <fstream>

namespace PredatorPreyHunter {
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::vector;
  using std::string;
  using std::ofstream;
  Domain::Domain( const int& maxSteps, const int& width, const int& height, NEAT::Network* ptrNetwork ) {
    this->maxSteps = maxSteps;
    ptrGridWorld = new GridWorld( width, height ); 
    cout << "[CREATED] GridWorld of size " << width << ", " << height << endl;
    // initialize predator 
    Position position;
    position.x = static_cast<int> ( fetchRandomNumber() * width );
    position.y = static_cast<int> ( fetchRandomNumber() * height );
    ptrPredator = new Predator( ptrGridWorld, 1, position, ptrNetwork );
    cout << "[CREATED] Predator at " << position.x << ", " << position.y << endl;
    // initialize prey
    position.x = static_cast<int> ( fetchRandomNumber() * width );
    position.y = static_cast<int> ( fetchRandomNumber() * height );
    ptrPrey = new Prey( ptrGridWorld, 2, position );
    cout << "[CREATED] Prey at " << position.x << ", " << position.y << endl;
    // initialize hunter
    position.x = static_cast<int> ( fetchRandomNumber() * width );
    position.y = static_cast<int> ( fetchRandomNumber() * height );
    ptrHunter = new Hunter( ptrGridWorld, 3, position );
    cout << "[CREATED] Hunter at " << position.x << ", " << position.y << endl;
  }
  Domain::~Domain() {
    delete ptrPredator;
    delete ptrPrey;
    delete ptrHunter;
    delete ptrGridWorld;
  }
  double Domain::calculateFitness( const Caught& caught, const uint& stepCurrent ) {
    if ( PREY_CAUGHT == caught ) { // Yay 
      return 10 * ( maxSteps - stepCurrent );
    }
    else if ( PREDATOR_CAUGHT == caught ) { // :-(
      return -1 * 10 * ( maxSteps - stepCurrent );
    }
    else {
      // calculate distance from hunter and prey
      Position positionPredator = ptrPredator->getPosition();
      Position positionPrey = ptrPrey->getPosition();
      Position positionHunter = ptrHunter->getPosition();
      uint distancePrey, distanceHunter;
      distancePrey = ptrGridWorld->distance( positionPrey, positionPredator );
      distanceHunter = ptrGridWorld->distance( positionHunter, positionPredator );
      // reward for being close to prey and far away from hunter
      distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
      // take into account the size of the grid for rewarding later
      return distancePrey + distanceHunter; // if by any chance it reaches here although it won't
    }
  }
  Domain::Caught Domain::step() {
    // check if prey is caught
    // return appropriate caught signal 
    AgentInformation aiPredator, aiPrey, aiHunter; 
    aiPredator = ptrPredator->getAgentInformation();
    aiPrey = ptrPrey->getAgentInformation();
    aiHunter = ptrHunter->getAgentInformation();
    if ( ( aiPrey.position.x == aiPredator.position.x ) 
          &&
         ( aiPrey.position.y == aiPredator.position.y ) ) {
      cout << "Prey caught by Predator" << endl;
      return PREY_CAUGHT;
    }
    // check if predator is caught
    // return appropriate caught signal 
    if ( ( aiHunter.position.x == aiPredator.position.x ) 
          &&
         ( aiHunter.position.y == aiPredator.position.y ) ) {
      cout << "Predator caught by Hunter" << endl;
      return PREDATOR_CAUGHT;
    }
    // build vector<AgentInformation>
    vector<AgentInformation> vAgentInformation;
    vAgentInformation.clear();
    vAgentInformation.push_back( aiPredator );
    vAgentInformation.push_back( aiPrey );
    vAgentInformation.push_back( aiHunter );
    // move prey
    ptrPrey->move( vAgentInformation ); 
    // move hunter
    ptrHunter->move( vAgentInformation ); 
    // move predator
    ptrPredator->move( vAgentInformation ); 
    return NONE_CAUGHT;
  }
  double Domain::run() {
    double fitness = 0.0;
    uint noSteps = 0;
    Caught caught; // for catching caught signal
    while ( noSteps < maxSteps ) {
      cout << "step: " << noSteps << endl;
      caught = step(); 
      switch( caught ) {
        case PREY_CAUGHT:
          // if prey is caught
          // return fitness of predator
          cout << "PREY CAUGHT!!!!" << endl;
          return calculateFitness( caught, noSteps );
          // break;
        case PREDATOR_CAUGHT:
          // if predator is caught
          // return fitness of predator
          cout << "PREDATOR CAUGHT!!!!" << endl;
          return calculateFitness( caught, noSteps );
          // break;
        case NONE_CAUGHT:
          break;
        default:
          cerr << "Domain::run() I should not have reached here" << endl;
          throw 1; // throw something meaningful later
      };
      noSteps++; // NOTE: Very Important. Do not delete.
    }
    fitness = calculateFitness( NONE_CAUGHT, noSteps ); 
    return fitness; // here you can also return how close it is to the prey
  }
  double Domain::run( string pathFile ) {
    cout << "[BEGINS] Domain::run()" << endl;
    // order is pred.x pred.y prey.x prey.y hunter.x hunter.y
    ofstream fout( pathFile.c_str() );
    if ( !fout.is_open() ) {
      cerr << "Could not open file " << pathFile << " for writing!" << endl;
      throw 1; // throw something meaningful later
    }
    double fitness = 0.0;
    uint noSteps = 0;
    Caught caught; // for catching caught signal
    Position positionPredator, positionPrey, positionHunter;
    while ( noSteps < maxSteps ) {
      cout << "step: " << noSteps << endl;
      caught = step(); 
      positionPredator = ptrPredator->getPosition(); 
      positionPrey = ptrPrey->getPosition();
      positionHunter = ptrHunter->getPosition();
      fout << positionPredator.x << " " << positionPredator.y << " ";
      fout << positionPrey.x << " " << positionPrey.y << " ";
      fout << positionHunter.x << " " << positionHunter.y << endl;
      switch( caught ) {
        case PREY_CAUGHT:
          // if prey is caught
          // update fitness of predator
          cout << "PREY CAUGHT!!!!" << endl;
          fout.close(); // use smart pointer later
          return calculateFitness( caught, noSteps );
          // break;
        case PREDATOR_CAUGHT:
          // if predator is caught
          // update fitness of predator
          cout << "PREDATOR CAUGHT!!!!" << endl;
          fout.close(); // use smart pointer later
          return calculateFitness( caught, noSteps );
          // break;
        case NONE_CAUGHT:
          break;
        default:
          cerr << "Domain::run() I should not have reached here" << endl;
          throw 1; // throw something meaningful later
      };
      noSteps++; // NOTE: Very Important. Do not delete.
    }
    fout.close();
    cout << "[ENDS] Domain::run()" << endl;
    fitness = calculateFitness( NONE_CAUGHT, noSteps ); 
    return fitness; // for now returning 0 but you can also return distance from prey
  }
}

