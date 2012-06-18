/*
 * Domain.h
 *
 *  Created on: Feb 16, 2012
 *      Author: anand
 */

#ifndef DOMAIN_H_
#define DOMAIN_H_

#include "../Esp/NetworkContainer.h"
#include "Visualizer.h"

namespace PredatorPreyHunter
{
    using std::vector;
    using EspPredPreyHunter::NetworkContainer;

    class Domain
    {
    protected:
        GridWorld* ptrGridWorld;
        PredPreyHunterVisualizer::Visualizer visualizer;
        const uint maxSteps;

        /**
         * This stores the number of agents that are not predators
         */
        uint numOtherAgents;

        virtual void step(const uint& stepNo) = 0;
        virtual double calculateFitness(const uint& stepCurrent) = 0;

    public:
        Domain()
                : maxSteps(0), numOtherAgents(0)
        {
        }

        Domain(const uint& maxSteps, const uint& width, const uint& height)
                : maxSteps(maxSteps)
        {
            //displayEnabled = true;
            ptrGridWorld = new GridWorld(width, height);
            LOG(INFO) << "[CREATED] GridWorld of size " << width << ", " << height;
            numOtherAgents = -1;
        }

        uint getNumOtherAgents()
        {
            return numOtherAgents;
        }

        virtual ~Domain()
        {
            delete ptrGridWorld;
        }

        virtual void init(NetworkContainer* espNetwork) = 0;
        virtual double run() = 0;     // return fitness
        virtual double run(std::string stepsFilePath, std::string networkSelectionFilePath) = 0;     // return fitness

        const uint getMaxSteps() const
        {
            return maxSteps;
        }
    };
}

#endif /* DOMAIN_H_ */
