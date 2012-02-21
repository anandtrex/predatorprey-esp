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
        const vector<NetworkContainer*> networkContainers;
        Network* combinerNetwork;
    public:
        NetworkContainerSubtask();
        NetworkContainerSubtask(const vector<NetworkContainer*> networkContainers);
        virtual ~NetworkContainerSubtask();

        void setNetwork(const NetworkContainer& networkContainer);
        void setFitness(const double& fitness);
        void incrementTests();
        void average();
        // For subpop
        void qsortNeurons();
        // For subpop
        void mutate();
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
    };
}

#endif /* NETWORKCONTAINERSUBTASK_H_ */
