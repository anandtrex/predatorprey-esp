/*
 * EspExperiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENT_H_
#define ESPEXPERIMENT_H_

#include "Experiment.h"
#include "../Domain/DomainTotal.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::Domain;

    class EspExperiment: public Experiment
    {
    protected:
        DomainTotal* domainTotal;
        NetworkContainer* networkContainerTotal;
        uint numGenerations;
        uint numEvalTrials;
        uint numTrialsPerGen;
    public:
        EspExperiment(const char* configFilePath);
        EspExperiment()
                : Experiment()
        {
        }
        virtual ~EspExperiment()
        {
        }
        virtual void start();
        void saveNetworkToFile(const string& fileName, NetworkContainer* networkContainer);
        NetworkContainer* evolve(Domain* domain, NetworkContainer* networkContainer);
    };
}

#endif /* ESPEXPERIMENT_H_ */
