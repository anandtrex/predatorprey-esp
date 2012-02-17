/*
 * Experiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "common.h"
#include "Esp.h"
#include "NetworkContainer.h"

namespace PredatorPreyHunter
{
    class Experiment
    {
    protected:
        Experiment(const char* configFilePath)
        {
        }
        ~Experiment()
        {
        }
        virtual void start() = 0;
    };
}

#endif /* EXPERIMENT_H_ */
