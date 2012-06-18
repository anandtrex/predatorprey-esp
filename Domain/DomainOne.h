/*
 * DomainOne.h
 *
 *  Created on: Mar 1, 2012
 *      Author: anand
 */

#ifndef DOMAINONE_H_
#define DOMAINONE_H_

#include "GridWorld.h"
#include "Domain.h"
#include "Agent.h"
#include "Predator.h"
#include "Prey.h"
#include "Hunter.h"
#include "Visualizer.h"
#include "../Esp/NetworkContainer.h"

#include <string>
#include <vector>

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

    template<class T>
    class DomainOne: public Domain
    {
        bool agentCaught;

        uint agentCaughtId;

        double agentMoveProb;

    protected:
        Predator* ptrPredator;
        Agent* ptrOtherAgent;
        void step(const uint& stepNo);

    public:
        DomainOne();
        DomainOne(const uint& maxSteps, const uint& width, const uint& height,
                const double& agentMoveProb);
        virtual ~DomainOne();
        virtual void init(NetworkContainer* espNetwork);
        virtual double run();     // return fitness
        virtual double run(std::string stepsFilePath, std::string networkSelectionFilePath)
        {
            LOG(ERROR) << "Run with steps output not implemented";
            return -1.0;
        }
        virtual double calculateFitness(const uint& stepCurrent);
    };
}

#endif /* DOMAINONE_H_ */
