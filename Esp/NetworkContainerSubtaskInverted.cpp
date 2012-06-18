/*
 * NetworkContainerSubtaskInvertedInverted.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "NetworkContainerSubtaskInverted.h"
#include <sstream>

namespace EspPredPreyHunter
{
    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted() :
            NetworkContainerSubtask()
    {

    }

    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted(const uint& nHiddenNeurons,
            const uint& popSize, const uint& netTp, const uint& numNetworks,
            const uint& numInputsPerNetwork, const uint& numOutputsPerNetwork) :
            NetworkContainerSubtask(nHiddenNeurons, popSize, netTp, numNetworks,
                    numInputsPerNetwork, numOutputsPerNetwork)
    {
    }

    NetworkContainerSubtaskInverted::~NetworkContainerSubtaskInverted()
    {

    }

    void NetworkContainerSubtaskInverted::activate(const vector<double>& input,
            vector<double>& output)
    {
        vector<double> tempSingleOutputs = vector<double>();
        const int k = networkContainers[0]->getInputsPerNetwork();
        const int l = input.size() / networkContainers.size();
        const int m = networkContainers[0]->getOutputsPerNetwork();

        for (uint i = 0; i < networkContainers.size(); i++) {
            vector<double> tempOutput = vector<double>(m);
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[l * i + j]);
            }
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        vector<double> tempOutput(outputsPerNetwork);
        combinerNetwork->activate(input, tempOutput);

        if (output.size() != 5)
            LOG(FATAL) << "Output size wasn't 5!";

        if(tempSingleOutputs.size() != 10)
            LOG(FATAL) << "tempSingleOutputs size wasn't 10!!";

        if (tempOutput[0] > tempOutput[1]) {
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[i];
            }
        } else {
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[i + 5];
            }
        }
    }
}

