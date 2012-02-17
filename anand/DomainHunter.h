/*
 * DomainHunter.h
 *
 *  Created on: Feb 16, 2012
 *      Author: anand
 */

#ifndef DOMAINHUNTER_H_
#define DOMAINHUNTER_H_

#include "GridWorld.h"
#include "Domain.h"
#include "Predator.h"
#include "Hunter.h"
#include "Visualizer.h"
#include "NetworkContainer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

    class DomainHunter: public Domain
    {
        uint numPredators;
        uint numHunters;

        uint numPredCaught;

        vector<uint> predatorCaughtIds;

        double hunterMoveProb;

    protected:
        Predator* ptrPredator;
        Hunter* ptrHunter;
        void step();

    public:
        DomainHunter();
        DomainHunter(const uint& maxSteps, const uint& width, const uint& height,
                const uint& numPredators, const uint& numHunters, const double& hunterMoveProb);
        ~DomainHunter();
        void init(NetworkContainer* espNetwork);
        void enableDisplay(const vector<double>& predatorColour,
                const vector<double>& hunterColour);
        double run();     // return fitness
        double calculateFitness(const uint& stepCurrent);
    };
}

#endif /* DOMAINHUNTER_H_ */
