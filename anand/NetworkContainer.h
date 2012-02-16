/*
 * NetworkEspCombiner.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef NETWORKESPCOMBINER_H_
#define NETWORKESPCOMBINER_H_


#include "Network.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the multi-agent ESP case. This is always used, irrespective
     * of the combination mechanism of the sub-tasks
     */
    class NetworkContainer
    {
        vector<Network*> networks;
    public:
        /**
         * Assume the last network is the combiner network for multi-agent ESP (for a given agent)
         * @param networks
         */
        NetworkContainer();

        ~NetworkContainer();

        void setNetworks(const vector<Network*>& networks);

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        void getOutput(const vector<double>& input, vector<double>& output);

        void setFitness(const double& fitness);

        void incrementTests();

        void average();

        // For subpop
        void qsortNeurons();

        // For subpop
        void mutate();

        void recombineHallOfFame(NetworkContainer* hallOfFameNetwork);

        Network* getNetwork(uint i);

        void activate(const vector<double>& input, vector<double>& output);
    };
}


#endif /* NETWORKESPCOMBINER_H_ */
