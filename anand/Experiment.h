/*
 * Experiment.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "GridWorld.h"
#include "Predator.h"
#include "Prey.h"
#include "Hunter.h"
#include <string>

namespace PredatorPreyHunter
{
    class Experiment
    { // this is for now a hack due to dearth of time. make it more general later
    protected:
        uint maxSteps;
        GridWorld* ptrGridWorld;
        Predator* ptrPredator;
        Hunter* ptrHunter;
        Prey* ptrPrey;
    public:
        enum Caught
        {
            NONE_CAUGHT = 0, PREY_CAUGHT = 1, PREDATOR_CAUGHT = 2
        };
    protected:
        Caught step();
    public:
        Experiment(const int& maxSteps, const int& width, const int& height);
        ~Experiment();
        double run(); // return fitness
        double run(std::string pathFile); // return fitness
    };
}

#endif /* EXPERIMENT_H_ */
