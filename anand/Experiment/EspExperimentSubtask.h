/*
 * EspExperimentSubtask.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENTSUBTASK_H_
#define ESPEXPERIMENTSUBTASK_H_

#include "EspExperiment.h"
#include "../Domain/DomainOne.h"
#include "../Domain/Prey.h"
#include "../Domain/Hunter.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::DomainOne;
    using PredatorPreyHunter::Prey;
    using PredatorPreyHunter::Hunter;

    template <class T>
    class EspExperimentSubtask: EspExperiment
    {
        NetworkContainer* networkContainerPrey;
        NetworkContainer* networkContainerHunter;

        DomainOne<Prey>* domainPrey;
        DomainOne<Hunter>* domainHunter;

    public:
        EspExperimentSubtask(const string& configFilePath);
        ~EspExperimentSubtask()
        {
        }
        void start();
        NetworkContainer* generateNetworkContainer(const uint& numHiddenNeurons, const uint& netType,
                const uint& numOutputsPerNetwork, const uint& numNonPredAgents, const uint& numInputsPerNetwork, const uint& popSize);
    };
}

#endif /* ESPEXPERIMENTSUBTASK_H_ */
