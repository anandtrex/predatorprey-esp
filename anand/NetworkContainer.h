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
     * This is a general Network container interface that can be used both for multi-agent esp case
     * and combining subtasks
     */
    class NetworkContainer
    {
    public:
        /**
         * Assume the last network is the combiner network for multi-agent ESP (for a given agent)
         * @param networks
         */
        NetworkContainer()
        {
        }

        virtual ~NetworkContainer()
        {
        }

        virtual void setNetwork(const NetworkContainer& networkContainer, const bool& append) = 0;

        virtual void setFitness(const double& fitness) = 0;

        virtual void incrementTests() = 0;

        virtual void average() = 0;

        // For subpop
        virtual void qsortNeurons() = 0;

        // For subpop
        virtual void mutate() = 0;

        virtual void recombineHallOfFame(NetworkContainer* hallOfFameNetwork) = 0;

        //virtual Network* getNetwork(uint i) = 0;

        virtual vector<Network*> getNetworks() const = 0;

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        virtual void activate(const vector<double>& input, vector<double>& output) = 0;
    };
}

#endif /* NETWORKESPCOMBINER_H_ */
