/*
 * Domain.h
 *
 *  Created on: Feb 16, 2012
 *      Author: anand
 */

#ifndef DOMAIN_H_
#define DOMAIN_H_

#include "NetworkContainer.h"
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
        bool displayEnabled;

        /**
         * This stores the number of agents that are not predators
         */
        uint numOtherAgents;

        virtual void step() = 0;
        virtual double calculateFitness(const uint& stepCurrent) = 0;

    public:
        Domain()
                : maxSteps(0), displayEnabled(false), numOtherAgents(0)
        {
        }

        Domain(const uint& maxSteps, const uint& width, const uint& height)
                : maxSteps(maxSteps), displayEnabled(false)
        {
            //displayEnabled = true;
            ptrGridWorld = new GridWorld(width, height);
            LOG(INFO) << "[CREATED] GridWorld of size " << width << ", " << height;
        }

        uint getNumOtherAgents()
        {
            return numOtherAgents;
        }

        void disableDisplay()
        {
            displayEnabled = false;
        }

        virtual ~Domain()
        {
        }

        virtual void init(NetworkContainer* espNetwork) = 0;
        virtual double run() = 0;     // return fitness
        //virtual double run(std::string stepsFilePath) = 0;     // return fitness
    };
}

#endif /* DOMAIN_H_ */
