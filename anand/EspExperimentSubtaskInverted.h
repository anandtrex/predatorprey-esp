/*
 * EspExperimentSubtaskInverted.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENTSUBTASKINVERTED_H_
#define ESPEXPERIMENTSUBTASKINVERTED_H_

#include "EspExperiment.h"
#include "Esp.h"
#include "DomainTotal.h"
#include "DomainPrey.h"
#include "DomainHunter.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;
    using PredatorPreyHunter::DomainPrey;
    using PredatorPreyHunter::DomainHunter;

    class EspExperimentSubtaskInverted: EspExperiment
    {
        Esp* espDomainTotal;
        Esp* espDomainPrey;
        Esp* espDomainHunter;

        DomainTotal* domainTotal;
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
