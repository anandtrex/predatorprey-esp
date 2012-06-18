/*
 * NetworkContainerSubtaskInverted.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef NETWORKCONTAINERSUBTASKINVERTED_H_
#define NETWORKCONTAINERSUBTASKINVERTED_H_

#include "Network.h"
#include "NetworkContainerCombiner.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the multi-agent ESP case. This is always used, irrespective
     * of the combination mechanism of the sub-tasks
     */
    class NetworkContainerSelection: public NetworkContainerCombiner
    {
        uint networkSelected;
    public:
        NetworkContainerSelection();
        NetworkContainerSelection(const uint& nHiddenNeurons, const uint& popSize,
                const uint& netTp, const uint& numNetworks, const uint& numInputsPerNetwork,
                const uint& numOutputsPerNetwork);
        ~NetworkContainerSelection();
        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        void activate(const vector<double>& input, vector<double>& output);

        uint getNetworkSelected() const
        {
            return networkSelected;
        }
    };
}

#endif /* NETWORKCONTAINERSUBTASKINVERTED_H_ */
