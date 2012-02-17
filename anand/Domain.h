/*
 * Experiment.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#ifndef DOMAIN_H_
#define DOMAIN_H_

#include "GridWorld.h"
#include "Predator.h"
#include "Prey.h"
#include "Hunter.h"
#include "Visualizer.h"
#include "NetworkContainer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

    // TODO Split this up for each subtask
    class Domain
    {
        bool displayEnabled;
        PredPreyHunterVisualizer::Visualizer visualizer;

        uint maxSteps;

        uint numPredators;
        uint numPrey;
        uint numHunters;

        uint numPredCaught;
        uint numPreyCaught;

        vector<uint> preyCaughtIds;
        vector<uint> predatorCaughtIds;

        double preyMoveProb;
        double hunterMoveProb;

    protected:
        GridWorld* ptrGridWorld;
        Predator* ptrPredator;
        Hunter* ptrHunter;
        Prey* ptrPrey;
        void step();

    public:
        Domain();
        Domain(const uint& maxSteps, const uint& width, const uint& height,
                const uint& numPredators, const uint& numPrey, const uint& numHunters,
                const double& preyMoveProb, const double& hunterMoveProb);
        //Domain(const char* configFilePath);
        ~Domain();
        void init(NetworkContainer* espNetwork);
        void enableDisplay(const vector<double>& predatorColour, const vector<double>& preyColour,
                const vector<double>& hunterColour);
        void disableDisplay();
        double run(); // return fitness
        double run(std::string stepsFilePath); // return fitness
        double calculateFitness(const uint& stepCurrent );
    };
}

#endif /* DOMAIN_H_ */
