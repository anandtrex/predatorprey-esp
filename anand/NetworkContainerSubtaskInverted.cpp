/*
 * NetworkContainerSubtaskInvertedInverted.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "NetworkContainerSubtaskInverted.h"

namespace EspPredPreyHunter
{
    // TODO Should getNetwork be different for this??
    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted()
            : NetworkContainerSubtask()
    {

    }

    NetworkContainerSubtaskInverted::NetworkContainerSubtaskInverted(
            const vector<NetworkContainer*> networkContainers)
            : NetworkContainerSubtask(networkContainers)
    {
        // NOTE: The networks to be evolved are set through setNetwork
    }

    NetworkContainerSubtaskInverted::~NetworkContainerSubtaskInverted()
    {

    }

    void NetworkContainerSubtaskInverted::activate(const vector<double>& input,
            vector<double>& output)
    {
        vector<double> tempSingleOutputs(input);
        int k = input.size() / networkContainers.size();
        for (uint i = 0; i < networkContainers.size(); i++) {
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[k * i + j]);
            }
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        vector<double> realCombinerInput(input);
        // Finally there is always one network
        vector<double> tempOutput(1);
        combinerNetwork->activate(tempSingleOutputs, tempOutput);

        if (tempOutput[0] < 0.5) {
            output.assign(tempSingleOutputs.begin(), tempSingleOutputs.begin() + 5);
        } else {
            output.assign(tempSingleOutputs.begin() + 5, tempSingleOutputs.end());
        }
    }
}

