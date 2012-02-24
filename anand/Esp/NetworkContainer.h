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
    protected:
        const uint inputsPerNetwork;     // Doesn't include combiner network (3 in the case of Pred-Prey-Hunter)
        const uint outputsPerNetwork;     // 5 in the case of pred-prey-hunter
        uint totalNumInputs;     // For the overall container -- calculated value
        uint totalNumOutputs;     // For the overall container -- calculated value
        uint numNetworks;     // Includes combiner network
    public:
        NetworkContainer(const uint& inputsPerNetwork, const uint& outputsPerNetwork,
                const uint& numNetworks)
                : inputsPerNetwork(inputsPerNetwork), outputsPerNetwork(outputsPerNetwork), totalNumInputs(
                        inputsPerNetwork * numNetworks), totalNumOutputs(outputsPerNetwork), numNetworks(
                        numNetworks)
        {
        }

        /**
         * Assume the last network is the combiner network for multi-agent ESP (for a given agent)
         * @param networks
         */
        virtual ~NetworkContainer()
        {
        }

        virtual void initializeNetworks() = 0;
        //virtual void setNetwork(const NetworkContainer& networkContainer) = 0;
        virtual void setFitness(const double& fitness) = 0;
        virtual void incrementTests() = 0;
        virtual void average() = 0;
        // For subpop
        virtual void qsortNeurons() = 0;
        // For subpop
        virtual void mutate() = 0;
        virtual void evalReset() = 0;
        virtual void recombineHallOfFame(NetworkContainer* hallOfFameNetwork) = 0;
        virtual vector<Network*> getNetworks() const = 0;

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        virtual void activate(const vector<double>& input, vector<double>& output) = 0;

        virtual string toString() = 0;
    };
}
#endif /* NETWORKESPCOMBINER_H_ */
