/*
 * NetworkContainerSubtaskInvertedInverted.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "NetworkContainerSelection.h"
#include <sstream>

namespace EspPredPreyHunter
{

    using PredatorPreyHunter::getMaxIndex;
    using std::endl;

    NetworkContainerSelection::NetworkContainerSelection()
            : NetworkContainerCombiner()
    {

    }

    NetworkContainerSelection::NetworkContainerSelection(const uint& nHiddenNeurons,
            const uint& popSize, const uint& netTp, const uint& numNetworks,
            const uint& numInputsPerNetwork, const uint& numOutputsPerNetwork)
            : NetworkContainerCombiner(nHiddenNeurons, popSize, netTp, numNetworks,
                    numInputsPerNetwork, numOutputsPerNetwork)
    {
    }

    NetworkContainerSelection::~NetworkContainerSelection()
    {

    }

    void NetworkContainerSelection::activate(const vector<double>& input, vector<double>& output)
    {
        const uint indepInputs = totalNumInputs;
        vector<double> newInput;

        newInput.assign(input.begin(), input.begin() + indepInputs);
        //LOG(ERROR) << endl;
        //LOG(ERROR) << "Input is " << vecToString(input);

        vector<double> tempSingleOutputs = vector<double>();
        const uint k = networkContainers[0]->getInputsPerNetwork();
        const uint l = input.size() / networkContainers.size();
        const uint m = networkContainers[0]->getOutputsPerNetwork();

        if(k + 1 != l){
            LOG(ERROR) << "Input size doesn't match!";
        }

        for (uint i = 0; i < networkContainers.size(); i++) {
            vector<double> tempOutput = vector<double>(m);
            vector<double> tempInput = vector<double>();
            for (uint j = 0; j < k; j++) {
                tempInput.push_back(input[l * i + j]);
            }
            //LOG(ERROR) << vecToString(tempInput);
            networkContainers[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        vector<double> tempOutput(outputsPerNetwork);
        combinerNetwork->activate(newInput, tempOutput);
        //LOG(ERROR) << "tempSingleOutputs is " << vecToString(tempSingleOutputs);

        if (output.size() != m)
            LOG(FATAL) << "Output size wasn't " << m << "!";

        if (tempSingleOutputs.size() != m * networkContainers.size())
            LOG(FATAL) << "tempSingleOutputs size wasn't 10!!";

        //LOG(ERROR) << "tempOutput is " << vecToString(tempOutput);

        const uint maxIndex = getMaxIndex(tempOutput);
        //LOG(ERROR) << "maxIndex is " << maxIndex;

        output.assign(tempSingleOutputs.begin() + maxIndex * m,
                tempSingleOutputs.begin() + maxIndex * m + m);
        //LOG(ERROR) << "Output is " << vecToString(output);
    }
}

