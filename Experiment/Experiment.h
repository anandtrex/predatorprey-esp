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
            time_t rawtime;
            char buffer [80];

            time ( &rawtime );
            strftime (buffer,80,"%Y%m%d%H%M",localtime ( &rawtime ));

            ostringstream sa;
            sa << "average_fitness." << buffer << ".log";
            foutGenAverage.open(sa.str().c_str());

            ostringstream sm;
            sm << "max_fitness." << buffer << ".log";
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
