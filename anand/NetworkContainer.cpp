/*
 * NetworkContainer.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: anand
 */

#include "NetworkContainer.h"

namespace EspPredPreyHunter
{
    NetworkContainer::NetworkContainer()
    {

    }

    NetworkContainer::~NetworkContainer()
    {

    }

    void NetworkContainer::setNetworks(const vector<Network*>& networks)
    {
        this->networks = networks;
    }

    void NetworkContainer::getOutput(const vector<double>& input, vector<double>& output)
    {
        // TODO Write this
    }

    void NetworkContainer::setFitness(const double& fitness)
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->setFitness(fitness);
        }
    }

    void NetworkContainer::incrementTests()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->incrementTests();
        }
    }

    void NetworkContainer::average()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->average();
        }
    }

    void NetworkContainer::qsortNeurons()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->qsortNeurons();
        }
    }

    void NetworkContainer::mutate()
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->mutate();
        }
    }

    void NetworkContainer::recombineHallOfFame(NetworkContainer* hallOfFameNetwork)
    {
        for (int i = 0; i < networks.size(); i++) {
            networks[i]->recombineHallOfFame(hallOfFameNetwork->getNetwork(i));
        }
    }

    Network* NetworkContainer::getNetwork(uint i)
    {
        return networks[i];
    }

    void NetworkContainer::activate(const vector<double>& input, vector<double>& output)
    {
        vector<double> tempSingleOutputs;
        for(int i = 0; i < networks.size() - 1; i++){
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            const double tempInput[] = {input[2 * i], input[2 * i + 1]};
            networks[i]->activate(makeVector(tempInput), tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }
        networks[networks.size() - 1]->activate(tempSingleOutputs, output);
    }
}

