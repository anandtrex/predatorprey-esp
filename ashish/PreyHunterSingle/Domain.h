#ifndef PPH_DOMAIN_H
#define PPH_DOMAIN_H

#include "Basic.h"
#include "GridWorld.h"
#include "PredatorSingleOutput.h"
#include "Prey.h"
#include "Hunter.h"
#include "network.h"
#include <string>

namespace PredatorPreyHunter {
  class Domain { // this is for now a hack due to dearth of time. make it more general later
  protected:
    uint maxSteps;
    GridWorld* ptrGridWorld;
    PredatorSingleOutput* ptrPredatorSingleOutput;
    Hunter* ptrHunter;
    Prey* ptrPrey;
  public:
    enum Caught { 
      NONE_CAUGHT = 0, 
      PREY_CAUGHT = 1,
      PREDATOR_CAUGHT = 2
    };
  protected:
    Caught step();
    double calculateFitness( const Caught& caught, const uint& stepCurrent );
  public:
    Domain( const int& maxSteps, const int& width, const int& height, NEAT::Network* ptrNetwork );
    ~Domain();
    double run(); // return fitness
    double run( std::string pathFile ); // return fitness
  };
}

#endif // PPH_DOMAIN_H
