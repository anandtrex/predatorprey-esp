/*
 * Experiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include <fstream>

namespace PredatorPreyHunter
{
    using std::ofstream;

    class Experiment
    {
    protected:
        ofstream foutGenAverage;
        ofstream foutGenMax;
        Experiment()
        {
            foutGenAverage.open("average_fitness.log");
            foutGenMax.open("max_fitness.log");
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
