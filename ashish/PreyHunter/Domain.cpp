#include "Domain.h"

namespace PredatorPreyHunter {
  Domain::Domain( const int& maxSteps, const int& width, const int& height ) {
    this->maxSteps = maxSteps;
    ptrGridWorld = new GridWorld( width, height ); 
    // initialize predator
    // initialize prey
    // initialize hunter
  }
  Domain::~Domain() {
    //delete ptrPredator;
    //delete ptrPrey;
    //delete ptrHunter;
    delete ptrGridWorld;
  }
  Domain::Caught Domain::step() {
    // check if prey is caught
    // return appropriate caught signal 
    // check if predator is caught
    // return appropriate caught signal 

    // move prey
    // move hunter
    // move predator
  }
  double Domain::run() {
    int noSteps = 0;
    Caught caught; // for catching caught signal
    while ( noSteps < maxSteps ) {
      caught = step(); 
      // if prey is caught
      // update fitness of predator
      // if predator is caught
      // update fitness of predator
    }
  }
}

