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
#include "../Esp/NetworkContainer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

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
        virtual ~DomainPrey();
        virtual void init(NetworkContainer* espNetwork);
        virtual void enableDisplay(const vector<double>& predatorColour,
                const vector<double>& preyColour);
        virtual double run();     // return fitness
        virtual double run(std::string stepsFilePath)
        {
            LOG(ERROR) << "Run with steps output not implemented";
            return -1.0;
        }
        virtual double calculateFitness(const uint& stepCurrent);
    };
}

#endif /* DOMAINPREY_H_ */
