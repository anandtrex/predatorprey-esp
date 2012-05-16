#ifndef PPH_DOMAIN_MANY_HUNTERS_H
#define PPH_DOMAIN_MANY_HUNTERS_H

#include "Basic.h"
#include "GridWorld.h"
#include "PredatorSelection.h"
#include "Prey.h"
#include "Hunter.h"
#include "network.h"
#include <string>
#include <vector>

namespace PredatorPreyHunter {
  class DomainManyHunters { // this is for now a hack due to dearth of time. make it more general later
  protected:
    uint maxSteps;
    GridWorld* ptrGridWorld;
    PredatorSelection* ptrPredatorSelection;
    std::vector<Hunter*> vPtrHunter;
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
    DomainManyHunters( const int& maxSteps, const int& width, const int& height, NEAT::Network* ptrNetworkHigher, NEAT::Network* ptrNetworkPrey, NEAT::Network* ptrNetworkHunter, const uint& noHunters );
    ~DomainManyHunters();
    double run(); // return fitness
    double run( std::string pathFile ); // return fitness
  };
}

#endif // PPH_DOMAIN_MANY_HUNTERS_H
