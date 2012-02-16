/*
 * NetworkSubtaskSeclectionCombiner.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef NETWORKSUBTASKSECLECTIONCOMBINER_H_
#define NETWORKSUBTASKSECLECTIONCOMBINER_H_

#include "NetworkCombiner.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the sub-tasks. This combines it by using
     * the top level network to select between the sub-task networks
     */
    class NetworkSubtaskSelectionCombiner : public NetworkCombiner
    {
    public:
        /**
         * Assume that the final NetworkCombo is the combiner NetworkCombo
         * @param networks
         */
        NetworkSubtaskSelectionCombiner(vector<ComboNetwork*> networks);

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


#endif /* NETWORKSUBTASKSECLECTIONCOMBINER_H_ */
