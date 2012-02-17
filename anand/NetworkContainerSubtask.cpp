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
    {

    }

    NetworkContainerSubtask::NetworkContainerSubtask(
            const vector<NetworkContainer*> networkContainers)
            : networkContainers(networkContainers)
    {
        // NOTE: The networks to be evolved are set through setNetwork
    }

    NetworkContainerSubtask::~NetworkContainerSubtask()
    {

    }

    void NetworkContainerSubtask::setNetwork(const NetworkContainer& networkContainer,
            const bool& append)
    {
        // NOTE: This will be just one network!
        networks = networkContainer.getNetworks();
    }
    /*
     void NetworkContainer::setNetworks(const vector<Network*>& networks, const uint& combine)
     {
     this->networks = networks;
     this->combine = combine;
     }*/

    void NetworkContainerSubtask::setFitness(const double& fitness)
    {
        VLOG(5) << "Setting fitness";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->setFitness(fitness);
        }
    }

    void NetworkContainerSubtask::incrementTests()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->incrementTests();
        }
    }

    void NetworkContainerSubtask::average()
    {
        VLOG(5) << "Averaging.";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->average();
        }
    }

    void NetworkContainerSubtask::qsortNeurons()
    {
        VLOG(5) << "Sorting.";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->qsortNeurons();
        }
    }

    void NetworkContainerSubtask::mutate()
    {
        VLOG(5) << "Doing mutate";
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->mutate();
        }
    }

    void NetworkContainerSubtask::recombineHallOfFame(NetworkContainer* hallOfFameNetwork)
    {
        VLOG(5) << "Doing recombine hall of fame";
        vector<Network*> hallOfFameNetworks = hallOfFameNetwork->getNetworks();
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->recombineHallOfFame(hallOfFameNetworks[i]);
        }
    }

    vector<Network*> NetworkContainerSubtask::getNetworks() const
    {
        return networks;
    }

    void NetworkContainerSubtask::activate(const vector<double>& input, vector<double>& output)
    {
        vector<double> tempSingleOutputs;
        for (int i = 0; i < networkContainers.size(); i++) {
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            const double tempInput[] = { input[2 * i], input[2 * i + 1] };
            networkContainers[i]->activate(makeVector(tempInput), tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        // Finally there is always one network
        networks[networks.size() - 1]->activate(tempSingleOutputs, output);
    }
}

