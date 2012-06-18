/*
 * NetworkContainerSubtask.h
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#ifndef NETWORKCONTAINERSUBTASK_H_
#define NETWORKCONTAINERSUBTASK_H_

#include "Network.h"
#include "NetworkContainer.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class NetworkContainerSubtask: public NetworkContainer
    {
    protected:
        vector<NetworkContainer*> networkContainers;
        Network* combinerNetwork;
    public:
        NetworkContainerSubtask();
        NetworkContainerSubtask(const uint& nHiddenNeurons, const uint& popSize,
                    const uint& netTp, const uint& numNetworks, const uint& numInputsPerNetwork,
                    const uint& numOutputsPerNetwork);
        virtual ~NetworkContainerSubtask();

        void initializeNetworks();
        //void setNetwork(const NetworkContainer& networkContainer);
        void setNetworkContainers(const vector<NetworkContainer*>& networkContainers);
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
        vector<NetworkContainer*> getNetworkContainers() const;

        /**
         * Returns the output from the given input and outputs
         * @param input
         * @param output
         * @return
         */
        virtual void activate(const vector<double>& input, vector<double>& output);

        virtual string toString()
        {
            return "None";
        }
    };
}

#endif /* NETWORKCONTAINERSUBTASK_H_ */
