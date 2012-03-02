/*
 * EspExperimentSubtaskInverted.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENTSUBTASKINVERTED_H_
#define ESPEXPERIMENTSUBTASKINVERTED_H_

#include "EspExperiment.h"
#include "../Domain/Prey.h"
#include "../Domain/Hunter.h"
#include "../Domain/DomainOne.h"
#include "../Domain/DomainTotal.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::DomainOne;
    using PredatorPreyHunter::Prey;
    using PredatorPreyHunter::Hunter;

    class EspExperimentSubtaskInverted: EspExperiment
    {
        NetworkContainer* networkContainerPrey;
        NetworkContainer* networkContainerHunter;

        DomainOne<Prey>* domainPrey;
        DomainOne<Hunter>* domainHunter;

    public:
        EspExperimentSubtaskInverted(const char* configFilePath);
        ~EspExperimentSubtaskInverted()
        {
        }
        void start();
    };
}

#endif /* ESPEXPERIMENTSUBTASKINVERTED_H_ */
