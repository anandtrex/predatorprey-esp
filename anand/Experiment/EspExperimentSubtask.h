/*
 * EspExperimentSubtask.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENTSUBTASK_H_
#define ESPEXPERIMENTSUBTASK_H_

#include "EspExperiment.h"
#include "../Esp/Esp.h"
#include "../Domain/DomainTotal.h"
#include "../Domain/DomainPrey.h"
#include "../Domain/DomainHunter.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::DomainPrey;
    using PredatorPreyHunter::DomainHunter;

    class EspExperimentSubtask: EspExperiment
    {
        Esp* espDomainTotal;
        Esp* espDomainPrey;
        Esp* espDomainHunter;

        DomainTotal* domainTotal;
        DomainPrey* domainPrey;
        DomainHunter* domainHunter;

    public:
        EspExperimentSubtask(const char* configFilePath);
        ~EspExperimentSubtask()
        {
        }
        void start();
    };
}


#endif /* ESPEXPERIMENTSUBTASK_H_ */
