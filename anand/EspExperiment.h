/*
 * EspExperiment.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef ESPEXPERIMENT_H_
#define ESPEXPERIMENT_H_

#include "Experiment.h"
#include "Esp.h"
#include "DomainTotal.h"

namespace EspPredPreyHunter
{
    using PredatorPreyHunter::Experiment;
    using PredatorPreyHunter::DomainTotal;

    class EspExperiment: Experiment
    {
        uint numGenerations;
        Esp* esp;
        DomainTotal* domain;

        uint numTrialsPerGen;
        uint numEvalTrials;

        NetworkContainer* generationBestNetwork;
        NetworkContainer* overallBestNetwork;

        vector<NetworkContainer*> hallOfFame;
    public:
        EspExperiment(const char* configFilePath);
        ~EspExperiment()
        {
        }
        void start();
    };
}

#endif /* ESPEXPERIMENT_H_ */
