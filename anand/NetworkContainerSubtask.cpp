/*
 * NetworkContainerSubtask.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "NetworkContainerSubtask.h"

namespace EspPredPreyHunter
{
    NetworkContainerSubtask::NetworkContainerSubtask()
        :networkContainers(0)
    {
    }

    NetworkContainerSubtask::NetworkContainerSubtask(
            const vector<NetworkContainer*> networkContainers)
            : networkContainers(networkContainers)
    {
        VLOG(5) << "Initialized networkContainers with " << this->networkContainers.size() << "containers";
        // NOTE: The networks to be evolved are set through setNetwork
    }

    NetworkContainerSubtask::~NetworkContainerSubtask()
    {
    }

    void NetworkContainerSubtask::setFitness(const double& fitness)
    {
        VLOG(5) << "Setting fitness";
        combinerNetwork->setFitness(fitness);
    }

    void NetworkContainerSubtask::incrementTests()
    {
        combinerNetwork->incrementTests();
    }

    void NetworkContainerSubtask::average()
    {
        VLOG(5) << "Averaging.";
        combinerNetwork->average();
    }

    void NetworkContainerSubtask::qsortNeurons()
    {
        VLOG(5) << "Sorting.";
        combinerNetwork->qsortNeurons();
    }

    void NetworkContainerSubtask::mutate()
    {
        VLOG(5) << "Doing mutate";
        combinerNetwork->mutate();
    }

    void NetworkContainerSubtask::recombineHallOfFame(NetworkContainer* hallOfFameNetwork)
    {
        VLOG(5) << "Doing recombine hall of fame";
        vector<Network*> hallOfFameNetworks = hallOfFameNetwork->getNetworks();
        if (hallOfFameNetworks.size() != 1) {
            LOG(FATAL) << "There can't be more than one hall of fame network!";
        }
        combinerNetwork->recombineHallOfFame(hallOfFameNetworks[0]);
    }

    vector<Network*> NetworkContainerSubtask::getNetworks() const
    {
        vector<Network*> tempNetworks = vector<Network*>();
        tempNetworks.push_back(combinerNetwork);
        return tempNetworks;
    }

    vector<NetworkContainer*> NetworkContainerSubtask::getNetworkContainers() const
    {
        return networkContainers;
    }

    void NetworkContainerSubtask::setNetwork(const NetworkContainer& networkContainer)
    {
        // NOTE: This will be just one network!
        vector<Network*> networks = networkContainer.getNetworks();
        if (networks.size() != 1) {
            LOG(FATAL) << "There can't be more than one hall of fame network!";
        }
        combinerNetwork = networks[0];
    }

    void NetworkContainerSubtask::activate(const vector<double>& input, vector<double>& output)
    {
        if(networkContainers.size() == 0)
            LOG(FATAL) << "Trying to activate empty container!";

        vector<double> tempSingleOutputs;
        int k = input.size() / networkContainers.size();
        for (uint i = 0; i < networkContainers.size(); i++) {
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[k * i + j]);
            }
            VLOG(5) << "Network container size is " << networkContainers.size();
            VLOG(5) << "Network container network size is " << networkContainers[i]->getNetworks().size();
            VLOG(5) << "Temp input size is " << tempInput.size();
            VLOG(5) << "Temp output size is " << tempOutput.size();
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        // Finally there is always one network
        combinerNetwork->activate(tempSingleOutputs, output);
    }
}

