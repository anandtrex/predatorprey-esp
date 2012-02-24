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
        std::string vecToString(const vector<double>& vec);
        VLOG(5) << "Input is " << vecToString(input);
        vector<double> tempSingleOutputs(input);
        int k = input.size() / networkContainers.size();
        VLOG(4) << " k is " << k;
        for (uint i = 0; i < networkContainers.size(); i++) {
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[k * i + j]);
                VLOG(5) << "Pushed back " << input[k * i + j];
            }
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        VLOG(5) << "Temp output is " << vecToString(tempSingleOutputs);

        // Finally there is always one network
        vector<double> tempOutput(2);
        combinerNetwork->activate(tempSingleOutputs, tempOutput);

        if (output.size() != 5)
            LOG(ERROR) << "Output size wasn't 5!";

        if (tempOutput[0] > tempOutput[1]) {
            VLOG(5) << "Output 0 is active being " << tempOutput[0];
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[input.size() + i];
            }
            //output.assign(tempSingleOutputs.begin(), tempSingleOutputs.begin() + 5);
        } else {
            VLOG(5) << "Output 1 is active being " << tempOutput[1];
            for (int i = 0; i < 5; i++) {
                output[i] = tempSingleOutputs[input.size() + i + 5];
            }
            //output.assign(tempSingleOutputs.begin() + 5, tempSingleOutputs.end());
        }
        VLOG(5) << "Final output is " << vecToString(output);
    }
}

