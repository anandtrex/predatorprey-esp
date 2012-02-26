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
    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted()
            : NetworkContainerSubtask()
    {

    }

    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted(const uint& nHiddenNeurons,
            const uint& popSize, const uint& netTp, const uint& numNetworks,
            const uint& numInputsPerNetwork, const uint& numOutputsPerNetwork)
            : NetworkContainerSubtask(nHiddenNeurons, popSize, netTp, numNetworks,
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
        int k = input.size() / networkContainers.size();

        VLOG(5) << " k is " << k;
        VLOG(5) << "Input size is " << input.size();
        VLOG(5) << "Output size is " << output.size();
        VLOG(5) << "Input is " << vecToString(input);

        for (uint i = 0; i < networkContainers.size(); i++) {
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[k * i + j]);
            }
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        VLOG(5) << "tempSingleOutputs size is " << tempSingleOutputs.size();
        VLOG(5) << "tempSingleOutputs is " << vecToString(tempSingleOutputs);

        // Finally there is always one network
        vector<double> tempOutput(2);
        combinerNetwork->activate(input, tempOutput);

        VLOG(5) << "tempOutput is " << vecToString(tempOutput);

        if (output.size() != 5)
            LOG(ERROR) << "Output size wasn't 5!";

        if (tempOutput[0] > tempOutput[1]) {
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[i];
            }
        } else {
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[i + 5];
            }
        }

        VLOG(5) << "Output is " << vecToString(output);
    }
}

