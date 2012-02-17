/*
 * NetworkContainer.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: anand
 */

#include "NetworkContainerEsp.h"

namespace EspPredPreyHunter
{
    NetworkContainerEsp::NetworkContainerEsp()
    {

    }

    NetworkContainerEsp::NetworkContainerEsp(const vector<Network*> networks)
            : networks(networks)
    {
        if (networks.size() == 1)
            combine = 0;
        else
            combine = 1;
    }

    NetworkContainerEsp::~NetworkContainerEsp()
    {

    }

    void NetworkContainerEsp::setNetwork(const NetworkContainer& networkContainer,
            const bool& append)
    {
        networks = networkContainer.getNetworks();
        if (networks.size() == 1)
            combine = 0;
        else
            combine = 1;
    }
    /*
     void NetworkContainer::setNetworks(const vector<Network*>& networks, const uint& combine)
     {
     this->networks = networks;
     this->combine = combine;
     }*/

    void NetworkContainerEsp::setFitness(const double& fitness)
    {
        VLOG(5) << "Setting fitness";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->setFitness(fitness);
        }
    }

    void NetworkContainerEsp::incrementTests()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->incrementTests();
        }
    }

    void NetworkContainerEsp::average()
    {
        VLOG(5) << "Averaging.";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->average();
        }
    }

    void NetworkContainerEsp::qsortNeurons()
    {
        VLOG(5) << "Sorting.";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->qsortNeurons();
        }
    }

    void NetworkContainerEsp::mutate()
    {
        VLOG(5) << "Doing mutate";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->mutate();
        }
    }

    void NetworkContainerEsp::recombineHallOfFame(NetworkContainer* hallOfFameNetwork)
    {
        VLOG(5) << "Doing recombine hall of fame";
        vector<Network*> hallOfFameNetworks = hallOfFameNetwork->getNetworks();
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->recombineHallOfFame(hallOfFameNetworks[i]);
        }
    }

    vector<Network*> NetworkContainerEsp::getNetworks() const
    {
        return networks;
    }

    void NetworkContainerEsp::activate(const vector<double>& input, vector<double>& output)
    {
        VLOG(5) << "Activating";
        VLOG(5) << "Networks size is " << networks.size();
        VLOG(5) << "Combine is " << combine;
        vector<double> tempSingleOutputs;
        for (int i = 0; i < networks.size() - combine; i++) {     // sans combiner
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            const double tempInput[] = { input[2 * i], input[2 * i + 1] };
            networks[i]->activate(makeVector(tempInput), tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        // for combiner
        if (combine == 1) {
            networks[networks.size() - 1]->activate(tempSingleOutputs, output);
        } else {
            // It comes into else only if there is a single network overall
            output.assign(tempSingleOutputs.begin(), tempSingleOutputs.end());
        }
        VLOG(5) << "Activated";
    }
}

