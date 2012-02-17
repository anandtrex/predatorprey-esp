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
    ptrPredatorSingleOutput = new PredatorSingleOutput( ptrGridWorld, 1, position, ptrNetwork, NULL, NULL );
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
    delete ptrPredatorSingleOutput;
    delete ptrPrey;
    delete ptrHunter;
    delete ptrGridWorld;
  }
  double Domain::calculateFitness( const Caught& caught, const uint& stepCurrent ) {
    double fitness = 0.0;
    if ( PREY_CAUGHT == caught ) { // Yay 
      fitness = static_cast<double>(10) * ( maxSteps - stepCurrent );
      return fitness;
    }
    else if ( PREDATOR_CAUGHT == caught ) { // :-(
      cout << "maxSteps - stepCurrent: " << maxSteps - stepCurrent << endl; 
      fitness = static_cast<double>(-1) * 10 * ( maxSteps - stepCurrent );
      cout << "FiTness: " << fitness << endl;
      return fitness; 
    }
    else {
      // calculate distance from hunter and prey
      Position positionPredator = ptrPredatorSingleOutput->getPosition();
      Position positionPrey = ptrPrey->getPosition();
      Position positionHunter = ptrHunter->getPosition();
      uint distancePrey, distanceHunter;
      distancePrey = ptrGridWorld->distance( positionPrey, positionPredator );
      distanceHunter = ptrGridWorld->distance( positionHunter, positionPredator );
      // reward for being close to prey and far away from hunter
      distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
      // take into account the size of the grid for rewarding later
      fitness = static_cast<double>(distancePrey) + distanceHunter; // if by any chance it reaches here although it won't
      return fitness;
    }
  }
  Domain::Caught Domain::step() {
    // check if prey is caught
    // return appropriate caught signal 
    AgentInformation aiPredator, aiPrey, aiHunter; 
    aiPredator = ptrPredatorSingleOutput->getAgentInformation();
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
    ptrPredatorSingleOutput->move( vAgentInformation ); 
    return NONE_CAUGHT;
  }
  double Domain::run() {
    double fitness = 0.0;
    uint noSteps = 0;
    Caught caught; // for catching caught signal
    while ( noSteps < maxSteps ) {
      //cout << "step: " << noSteps << endl;
      caught = step(); 
      switch( caught ) {
        case PREY_CAUGHT:
          // if prey is caught
          // return fitness of predator
          cout << "PREY CAUGHT!!!!" << endl;
          cout << "step: " << noSteps << endl;
          return calculateFitness( caught, noSteps );
          // break;
        case PREDATOR_CAUGHT:
          // if predator is caught
          // return fitness of predator
          cout << "PREDATOR CAUGHT!!!!" << endl;
          cout << "step: " << noSteps << endl;
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
    cout << "step: " << noSteps << endl;
    return fitness; // here you can also return how close it is to the prey
  }
  double Domain::run( string pathFile ) {
    cout << "[BEGINS] Domain::run(pathFile)" << endl;
    cout << "Saving performance to file: " << pathFile << endl;
    // order is pred.x pred.y prey.x prey.y hunter.x hunter.y
    ofstream fout( pathFile.c_str() );
    if ( !fout.is_open() ) {
      cerr << "Could not open file " << pathFile << " for writing!" << endl;
      throw 1; // throw something meaningful later
    } else {
      cout << "File " << pathFile << " opened successfully for writing" << endl;
    }
    double fitness = 0.0;
    uint noSteps = 0;
    Caught caught; // for catching caught signal
    Position positionPredator, positionPrey, positionHunter;
    while ( noSteps < maxSteps ) {
      //cout << "step: " << noSteps << endl;
      positionPredator = ptrPredatorSingleOutput->getPosition(); 
      positionPrey = ptrPrey->getPosition();
      positionHunter = ptrHunter->getPosition();
      fout << positionPredator.x << " " << positionPredator.y << " ";
      fout << positionPrey.x << " " << positionPrey.y << " ";
      fout << positionHunter.x << " " << positionHunter.y << endl;
      caught = step(); 
      switch( caught ) {
        case PREY_CAUGHT:
          // if prey is caught
          // update fitness of predator
          cout << "PREY CAUGHT!!!!" << endl;
          cout << "step: " << noSteps << endl;
          fout.close(); // use smart pointer later
          return calculateFitness( caught, noSteps );
          // break;
        case PREDATOR_CAUGHT:
          // if predator is caught
          // update fitness of predator
          cout << "PREDATOR CAUGHT!!!!" << endl;
          cout << "step: " << noSteps << endl;
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
    cout << "step: " << noSteps << endl;
    cout << "[ENDS] Domain::run(pathFile)" << endl;
    fitness = calculateFitness( NONE_CAUGHT, noSteps ); 
    return fitness; // for now returning 0 but you can also return distance from prey
  }
}

