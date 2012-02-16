/*
 * NetworkEspCombiner.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef NETWORKESPCOMBINER_H_
#define NETWORKESPCOMBINER_H_

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the multi-agent ESP case. This is always used, irrespective
     * of the combination mechanism of the sub-tasks
     */
    class ComboNetwork
    {
    public:
        /**
         * Assume the last network is the combiner network for multi-agent ESP (for a given agent)
         * @param networks
         */
        ComboNetwork(vector<Network*> networks);

        ~NetworkEspCombiner();

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        uint getOutput(vector<double>& input, vector<double>& output);
    };
}


#endif /* NETWORKESPCOMBINER_H_ */
