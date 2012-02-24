/*
 * NetworkContainerEsp.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef NETWORKCONTAINERESP_H_
#define NETWORKCONTAINERESP_H_

#include "Network.h"
#include "NetworkContainer.h"

namespace EspPredPreyHunter
{
    using std::vector;

    /**
     * This is the network combiner for the multi-agent ESP case. This is always used, irrespective
     * of the combination mechanism of the sub-tasks
     */
    class NetworkContainerEsp: public NetworkContainer
    {
        uint combine;     // Number of combiner networks
        vector<Network*> networks;
    public:
        NetworkContainerEsp();
        NetworkContainerEsp(const vector<Network*> networks);
        NetworkContainerEsp(const uint& nHiddenNeurons, const uint& popSize, const uint& netTp,
                const uint& numNetworks, const uint& numInputsPerNetwork,
                const uint& numOutputsPerNetwork);
        ~NetworkContainerEsp();

        void initializeNetworks();
        //void setNetwork(const NetworkContainer& networkContainer);
        void setFitness(const double& fitness);
        void incrementTests();
        void average();
        // For subpop
        void qsortNeurons();
        // For subpop
        void mutate();
        void evalReset();
        void recombineHallOfFame(NetworkContainer* hallOfFameNetwork);
        vector<Network*> getNetworks() const;

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        void activate(const vector<double>& input, vector<double>& output);

        string toString();
    };
}
#endif /* NETWORKCONTAINERESP_H_ */
