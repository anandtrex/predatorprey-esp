/*
 * NetworkContainerSubtaskInvertedInverted.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "NetworkContainerSubtaskInverted.h"

namespace EspPredPreyHunter
{
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
        VLOG(5) << " k is " << k;
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

        // Finally there is always one network
        vector<double> tempOutput(2);
        combinerNetwork->activate(tempSingleOutputs, tempOutput);

        if(output.size()!=5)
            LOG(ERROR) << "Output size wasn't 5!";
        if (tempOutput[0] > tempOutput[1]) {
            VLOG(5) << "Output 0 is active";
            output.assign(tempSingleOutputs.begin(), tempSingleOutputs.begin() + 5);
        } else {
            VLOG(5) << "Output 1 is active";
            output.assign(tempSingleOutputs.begin() + 5, tempSingleOutputs.end());
        }
    }
}

