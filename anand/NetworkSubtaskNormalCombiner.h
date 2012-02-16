/*
 * NetworkSubtaskNormalCombiner.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef NETWORKSUBTASKNORMALCOMBINER_H_
#define NETWORKSUBTASKNORMALCOMBINER_H_

#include "NetworkCombiner.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the sub-tasks. This combines it in the "normal" way --
     * feeding the inputs to the subnetworks, and the sub-task combiner network,
     * and getting the final output from the sub-task combiner network
     */
    class NetworkSubtaskNormalCombiner : public NetworkCombiner
    {
    public:
        /**
         * Assume the last network is the combiner network for multi-agent ESP (for a given agent)
         * @param networks
         */
        NetworkSubtaskNormalCombiner(vector<ComboNetwork*> networks);

        ~NetworkSubtaskNormalCombiner();

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        uint getOutput(vector<double>& input, vector<double>& output);
    };
}


#endif /* NETWORKSUBTASKNORMALCOMBINER_H_ */
