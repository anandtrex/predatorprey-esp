/*
 * NetworkContainerSubtaskInverted.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef NETWORKCONTAINERSUBTASKINVERTED_H_
#define NETWORKCONTAINERSUBTASKINVERTED_H_

#include "Network.h"
#include "NetworkContainerSubtask.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the multi-agent ESP case. This is always used, irrespective
     * of the combination mechanism of the sub-tasks
     */
    class NetworkContainerSubtaskInverted: public NetworkContainerSubtask
    {
    public:
        NetworkContainerSubtaskInverted();
        NetworkContainerSubtaskInverted(const uint& nHiddenNeurons, const uint& popSize,
                           const uint& netTp, const uint& numNetworks, const uint& numInputsPerNetwork,
                           const uint& numOutputsPerNetwork);
        ~NetworkContainerSubtaskInverted();
        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        void activate(const vector<double>& input, vector<double>& output);
    };
}

#endif /* NETWORKCONTAINERSUBTASKINVERTED_H_ */
