/*
 * Experiment.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#ifndef DOMAINTOTAL_H_
#define DOMAINTOTAL_H_

#include "GridWorld.h"
#include "Domain.h"
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

    class DomainTotal : public Domain
    {
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
        Predator* ptrPredator;
        Hunter* ptrHunter;
        Prey* ptrPrey;
        void step();

    public:
        DomainTotal();
        DomainTotal(const uint& maxSteps, const uint& width, const uint& height,
                const uint& numPredators, const uint& numPrey, const uint& numHunters,
                const double& preyMoveProb, const double& hunterMoveProb);
        ~DomainTotal();
        virtual void init(NetworkContainer* espNetwork);
        virtual void enableDisplay(const vector<double>& predatorColour, const vector<double>& preyColour,
                const vector<double>& hunterColour);
        virtual double run(); // return fitness
        virtual double calculateFitness(const uint& stepCurrent );
    };
}

#endif /* DOMAINTOTAL_H_ */
