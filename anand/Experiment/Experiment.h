/*
 * Experiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include <fstream>
#include <sstream>
#include <time.h>
#include <gflags/gflags.h>

// this is used in EspExperiment.cpp
DECLARE_int32(h);

namespace PredatorPreyHunter
{
    using std::ofstream;
    using std::ostringstream;

    class Experiment
    {
    protected:
        ofstream foutGenAverage;
        ofstream foutGenMax;
        Experiment()
        {
            ostringstream sa;
            sa << "average_fitness." << time(NULL) << ".log";
            foutGenAverage.open(sa.str().c_str());

            ostringstream sm;
            sm << "max_fitness." << time(NULL) << ".log";
            foutGenMax.open(sm.str().c_str());
        }
        ~Experiment()
        {
          foutGenAverage.close();
          foutGenMax.close();
        }
        virtual void start() = 0;
    };
}

#endif /* EXPERIMENT_H_ */
