/*
 * DomainPrey.h
 *
 *  Created on: Feb 16, 2012
 *      Author: anand
 */

#ifndef DOMAINPREY_H_
#define DOMAINPREY_H_

#include "GridWorld.h"
#include "Domain.h"
#include "Predator.h"
#include "Prey.h"
#include "Visualizer.h"
#include "NetworkContainer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

    // TODO Split this up for each subtask
    class DomainPrey: public Domain
    {
        uint numPredators;
        uint numPrey;

        uint numPreyCaught;

        vector<uint> preyCaughtIds;

        double preyMoveProb;

    protected:
        Predator* ptrPredator;
        Prey* ptrPrey;
        void step();

    public:
        DomainPrey();
        DomainPrey(const uint& maxSteps, const uint& width, const uint& height,
                const uint& numPredators, const uint& numPrey, const double& preyMoveProb);
        ~DomainPrey();
        virtual void init(NetworkContainer* espNetwork);
        virtual void enableDisplay(const vector<double>& predatorColour,
                const vector<double>& preyColour);
        virtual double run();     // return fitness
        virtual double calculateFitness(const uint& stepCurrent);
    };
}

#endif /* DOMAINPREY_H_ */
