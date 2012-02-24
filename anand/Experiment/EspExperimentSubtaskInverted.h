/*
 * EspExperimentSubtaskInverted.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENTSUBTASKINVERTED_H_
#define ESPEXPERIMENTSUBTASKINVERTED_H_

#include "EspExperiment.h"
#include "../Domain/DomainTotal.h"
#include "../Domain/DomainPrey.h"
#include "../Domain/DomainHunter.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::DomainPrey;
    using PredatorPreyHunter::DomainHunter;

    class EspExperimentSubtaskInverted: EspExperiment
    {
        NetworkContainer* networkContainerPrey;
        NetworkContainer* networkContainerHunter;

        DomainPrey* domainPrey;
        DomainHunter* domainHunter;

    public:
        EspExperimentSubtaskInverted(const char* configFilePath);
        ~EspExperimentSubtaskInverted()
        {
        }
        void start();
    };
}

#endif /* ESPEXPERIMENTSUBTASKINVERTED_H_ */
