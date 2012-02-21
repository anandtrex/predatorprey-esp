/*
 * EspExperiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENT_H_
#define ESPEXPERIMENT_H_

#include "Experiment.h"
#include "../Esp/Esp.h"
#include "../Domain/DomainTotal.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::Domain;

    class EspExperiment: public Experiment
    {
        DomainTotal* domain;
        Esp* esp;

    protected:
        uint numGenerations;
        uint popSize;
    public:
        EspExperiment(const char* configFilePath);
        EspExperiment()
            :Experiment()
        {
        }
        virtual ~EspExperiment()
        {
        }
        virtual void start();
        NetworkContainer* evolve(Domain* domain, NetworkContainer* networkContainer, Esp* esp, bool append);
    };
}

#endif /* ESPEXPERIMENT_H_ */
