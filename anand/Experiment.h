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
#include <fstream>

namespace PredatorPreyHunter
{
    using std::ofstream;

    class Experiment
    {
    protected:
        ofstream fout;
        Experiment(const char* configFilePath)
        {
            fout.open("average_fitness.log");
        }
        ~Experiment()
        {
        }
        virtual void start() = 0;
    };
}

#endif /* EXPERIMENT_H_ */
