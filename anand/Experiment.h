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
#include "Visualizer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
	using std::vector;

    class Experiment
    { // this is for now a hack due to dearth of time. make it more general later
	    bool displayEnabled;
	    PredPreyHunterVisualizer::Visualizer visualizer;
    protected:
        uint maxSteps;
        GridWorld* ptrGridWorld;
        Predator* ptrPredator;
        Hunter* ptrHunter;
        Prey* ptrPrey;
    public:
        uint numPredators;
        uint numPrey;
        uint numHunters;

        uint numPredCaught;
        uint numPreyCaught;

        vector<uint> preyCaughtIds;
        vector<uint> predatorCaughtIds;
    protected:
        void step();
    public:
        //Experiment(const int& maxSteps, const int& width, const int& height);
        Experiment(const char* configFilePath);
        ~Experiment();
        double run(); // return fitness
        double run(std::string stepsFilePath); // return fitness
    };
}

#endif /* EXPERIMENT_H_ */
