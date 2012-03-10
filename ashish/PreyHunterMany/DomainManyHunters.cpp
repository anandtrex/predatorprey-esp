#include "DomainManyHunters.h"
#include <iostream>
#include <fstream>
#include <utility>

namespace PredatorPreyHunter {
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::vector;
  using std::string;
  using std::ofstream;

  Position getNewPosition( const int& width, const int& height, vector< Position >& vPositionsAllocated ) {
    Position position;
    bool existsValue;
    uint count = 1;
    do {
      existsValue = false;
      position.x = static_cast<int> ( fetchRandomNumber() * width );
      position.y = static_cast<int> ( fetchRandomNumber() * height );
      for ( vector< Position >::const_iterator it = vPositionsAllocated.begin(); it != vPositionsAllocated.end(); ++it ) {
        if ( it->x == position.x && it->y == position.y ) {
          existsValue = true; 
          break;
        }
      }
      //cout << "Count: " << count << " Position( " << position.x << ", " << position.y << " ) " << endl;
      //cout << "Vector: ";
      //for ( vector< Position >::const_iterator it = vPositionsAllocated.begin(); it != vPositionsAllocated.end(); ++it ) {
        //cout << " ( " << it->x << ", " << it->y << " ) ";
      //}
      //cout << endl;
      count++;
    } while ( existsValue );
    vPositionsAllocated.push_back( position );
    return position;
  }
      
  DomainManyHunters::DomainManyHunters( const int& maxSteps, const int& width, const int& height, NEAT::Network* ptrNetwork, const uint& noHunters ) {
    this->maxSteps = maxSteps;
    ptrGridWorld = new GridWorld( width, height ); 
    cout << "[CREATED] GridWorld of size " << width << ", " << height << endl;
    // set of positions where agents have already been created
    vector< Position > vPositionsAllocated; // ideally you should use a set
    // initialize predator 
    Position position;
    position = getNewPosition( width, height, vPositionsAllocated );
    ptrPredator = new Predator( ptrGridWorld, 1, position, ptrNetwork, noHunters + 1 );
    cout << "[CREATED] Predator at " << position.x << ", " << position.y << endl;
    // initialize prey
    position = getNewPosition( width, height, vPositionsAllocated );
    ptrPrey = new Prey( ptrGridWorld, 2, position );
    cout << "[CREATED] Prey at " << position.x << ", " << position.y << endl;
    // initialize hunters
    for ( uint i = 1; i <= noHunters; i++ ) {
      position = getNewPosition( width, height, vPositionsAllocated );
      Hunter* ptrHunter = new Hunter( ptrGridWorld, 2 + i, position );
      vPtrHunter.push_back( ptrHunter ); 
      cout << "[CREATED] Hunter at " << position.x << ", " << position.y << endl;
    }
  }
  DomainManyHunters::~DomainManyHunters() {
    delete ptrPredator;
    delete ptrPrey;
    for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
      delete vPtrHunter[i];
    }
    delete ptrGridWorld;
  }
  double DomainManyHunters::calculateFitness( const Caught& caught, const uint& stepCurrent ) {
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
      Position positionPredator = ptrPredator->getPosition();
      Position positionPrey = ptrPrey->getPosition();
      vector<Position> vPositionHunter;
      for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
        vPositionHunter.push_back( ( vPtrHunter[i] )->getPosition() );
      }
      uint distancePrey;
      double distanceHunter;
      distancePrey = ptrGridWorld->distance( positionPrey, positionPredator );
      // calculating the average (harmonic mean) distance away from the hunters
      distanceHunter = 0; // calculating the denominator first
      for ( uint i = 0; i < vPositionHunter.size(); i++ ) {
        distanceHunter += 1 / static_cast<double> ( ptrGridWorld->distance( vPositionHunter[i], positionPredator ) );
      }
      distanceHunter = vPositionHunter.size() / distanceHunter; // n / ( 1/x1 + 1/x2 + ... + 1/xn )
      // reward for being close to prey and far away from hunter
      distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
      // take into account the size of the grid for rewarding later
      fitness = static_cast<double>(distancePrey) + distanceHunter; // if by any chance it reaches here although it won't
      cout << "Calculating Fitness: " << fitness << endl;
      cout << "DistancePrey  : " << distancePrey << endl;
      cout << "DistanceHunter: " << distanceHunter << endl;
      return fitness;
    }
  }
  DomainManyHunters::Caught DomainManyHunters::step() {
    // check if prey is caught
    // return appropriate caught signal 
    AgentInformation aiPredator, aiPrey, aiHunter; 
    aiPredator = ptrPredator->getAgentInformation();
    aiPrey = ptrPrey->getAgentInformation();
    if ( ( aiPrey.position.x == aiPredator.position.x ) 
          &&
         ( aiPrey.position.y == aiPredator.position.y ) ) {
      cout << "Prey caught by Predator" << endl;
      return PREY_CAUGHT;
    }
    // check if predator is caught
    // return appropriate caught signal 
    for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
      aiHunter = ( vPtrHunter[i] )->getAgentInformation();
      if ( ( aiHunter.position.x == aiPredator.position.x ) 
            &&
          ( aiHunter.position.y == aiPredator.position.y ) ) {
        cout << "Predator caught by Hunter" << endl;
        return PREDATOR_CAUGHT;
      }
    }
    // build vector<AgentInformation>
    vector<AgentInformation> vAgentInformation;
    vAgentInformation.clear();
    vAgentInformation.push_back( aiPredator );
    vAgentInformation.push_back( aiPrey );
    for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
      vAgentInformation.push_back( ( vPtrHunter[i] )->getAgentInformation() );
    }
    // move prey
    ptrPrey->move( vAgentInformation ); 
    // move hunters
    for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
      ( vPtrHunter[i] )->move( vAgentInformation ); 
    }
    // move predator
    ptrPredator->move( vAgentInformation ); 
    return NONE_CAUGHT;
  }
  double DomainManyHunters::run() {
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
          fitness = calculateFitness( caught, noSteps );
          cout << "Fitness: " << fitness << endl; 
          return fitness; 
          // break;
        case PREDATOR_CAUGHT:
          // if predator is caught
          // return fitness of predator
          cout << "PREDATOR CAUGHT!!!!" << endl;
          cout << "step: " << noSteps << endl;
          fitness = calculateFitness( caught, noSteps );
          cout << "Fitness: " << fitness << endl; 
          return fitness;
          // break;
        case NONE_CAUGHT:
          break;
        default:
          cerr << "DomainManyHunters::run() I should not have reached here" << endl;
          throw 1; // throw something meaningful later
      };
      noSteps++; // NOTE: Very Important. Do not delete.
    }
    fitness = calculateFitness( NONE_CAUGHT, noSteps ); 
    cout << "step: " << noSteps << endl;
    cout << "Fitness: " << fitness << endl;
    return fitness; // here you can also return how close it is to the prey
  }
  double DomainManyHunters::run( string pathFile ) {
    cout << "[BEGINS] DomainManyHunters::run(pathFile)" << endl;
    cout << "Saving performance to file: " << pathFile << endl;
    // order is type pred.x pred.y type prey.x prey.y ... type[i] hunter[i].x hunter[i].y
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
      positionPredator = ptrPredator->getPosition(); 
      positionPrey = ptrPrey->getPosition();
      fout << ptrPredator->getType() << " " << positionPredator.x << " " << positionPredator.y << " ";
      fout << ptrPrey->getType() << " " << positionPrey.x << " " << positionPrey.y << " ";
      for ( uint i = 0; i < vPtrHunter.size(); i++ ) {
        positionHunter = ( vPtrHunter[i] )->getPosition();
        fout << ( vPtrHunter[i] )->getType() << " " << positionHunter.x << " " << positionHunter.y << " ";
      }
      fout << endl;
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
          cerr << "DomainManyHunters::run() I should not have reached here" << endl;
          throw 1; // throw something meaningful later
      };
      noSteps++; // NOTE: Very Important. Do not delete.
    }
    fout.close();
    cout << "step: " << noSteps << endl;
    cout << "[ENDS] DomainManyHunters::run(pathFile)" << endl;
    fitness = calculateFitness( NONE_CAUGHT, noSteps ); 
    return fitness;
  }
}

