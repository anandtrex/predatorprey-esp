/*
 * NetworkContainer.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: anand
 */
#include <sstream>
#include <time.h>

#include "NetworkContainerEsp.h"
#include "FeedForwardNetwork.h"

namespace EspPredPreyHunter
{
    NetworkContainerEsp::NetworkContainerEsp() :
            NetworkContainer(0, 0, 0), networks(0)
    {

    }

    NetworkContainerEsp::NetworkContainerEsp(const vector<Network*> networks) :
            NetworkContainer(networks[0]->numInputs, networks[0]->numOutputs, networks.size()), networks(
                    networks)
    {
        combine = (networks.size() == 1) ? 0 : 1;
    }

    NetworkContainerEsp::NetworkContainerEsp(const uint& nHiddenNeurons, const uint& popSize,
            const uint& netTp, const uint& numNetworks, const uint& numInputsPerNetwork,
            const uint& numOutputsPerNetwork) :
            NetworkContainer(numInputsPerNetwork, numOutputsPerNetwork,
                    numNetworks + (numNetworks == 1 ? 0 : 1))
    {
        combine = (numNetworks == 1) ? 0 : 1;

        LOG(INFO) << "Combine is " << combine;
        LOG(INFO) << "Number of networks (including combiner) is " << this->numNetworks;

        const uint combinerNetworkNumInputs = numNetworks * numOutputsPerNetwork;
        LOG(INFO) << "Number of combiner network inputs is " << combinerNetworkNumInputs;

        // Initialize and create networks
        for (uint i = 0; i < this->numNetworks - combine; i++) {
            networks.push_back(
                    new FeedForwardNetwork(nHiddenNeurons, popSize, numInputsPerNetwork,
                            numOutputsPerNetwork, true));
        }
        for (uint i = this->numNetworks - combine; i < this->numNetworks; i++) {
            networks.push_back(
                    new FeedForwardNetwork(nHiddenNeurons, popSize, combinerNetworkNumInputs,
                            numOutputsPerNetwork, true));
        }
        LOG(INFO)
                << "Initialized and created networks. (subpopulations initialized within the networks)";
    }

    NetworkContainerEsp::~NetworkContainerEsp()
    {

    }

    /*
     void NetworkContainerEsp::setNetwork(const NetworkContainer& networkContainer)
     {
     networks = networkContainer.getNetworks();
     if (networks.size() == 1)
     combine = 0;
     else
     combine = 1;
     }*/

    void NetworkContainerEsp::setFitness(const double& fitness)
    {
        VLOG(5) << "Setting fitness";
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->setFitness(fitness);
        }
    }

    void NetworkContainerEsp::incrementTests()
    {
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->incrementTests();
        }
    }

    void NetworkContainerEsp::average()
    {
        VLOG(5) << "Averaging.";
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->average();
        }
    }

    void NetworkContainerEsp::qsortNeurons()
    {
        VLOG(5) << "Sorting.";
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->qsortNeurons();
        }
    }

    void NetworkContainerEsp::mutate()
    {
        VLOG(5) << "Doing mutate";
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->mutate();
        }
    }

    void NetworkContainerEsp::recombineHallOfFame(NetworkContainer* hallOfFameNetwork)
    {
        VLOG(5) << "Doing recombine hall of fame";
        vector<Network*> hallOfFameNetworks = hallOfFameNetwork->getNetworks();
        for (uint i = 0; i < networks.size(); i++) {
            networks[i]->recombineHallOfFame(hallOfFameNetworks[i]);
        }
    }

    void NetworkContainerEsp::evalReset()
    {
        for (uint i = 0; i < numNetworks; i++) {
            networks[i]->evalReset();
        }
    }

    void NetworkContainerEsp::initializeNetworks()
    {
        VLOG(4) << "Initializing networks";
        for (uint i = 0; i < numNetworks; i++) {
            // Selects random neurons from subpopulation and returns it
            networks[i]->setNeurons();
        }
    }

    vector<Network*> NetworkContainerEsp::getNetworks() const
    {
        return networks;
    }

    void NetworkContainerEsp::activate(const vector<double>& input, vector<double>& output)
    {
        if (networks.size() == 0)
            LOG(FATAL) << "Trying to activate empty container!";

        const int k = networks[0]->numInputs;
        const int l = input.size() / networks.size();
        const int m = networks[0]->numOutputs;

        vector<double> tempSingleOutputs;
        for (uint i = 0; i < networks.size() - combine; i++) {     // sans combiner
            vector<double> tempOutput = vector<double>(m);
            vector<double> tempInput = vector<double>();
            for (int j = 0; j < k; j++) {
                tempInput.push_back(input[l * i + j]);
            }
            networks[i]->activate(tempInput, tempOutput);
            tempSingleOutputs.insert(tempSingleOutputs.end(), tempOutput.begin(), tempOutput.end());
        }

        // for combiner
        if (combine == 1) {
            networks[networks.size() - 1]->activate(tempSingleOutputs, output);
        } else {
            // It comes into else only if there is a single network overall
            output.assign(tempSingleOutputs.begin(), tempSingleOutputs.end());
        }
    }

    using std::ostringstream;
    using std::endl;

    string NetworkContainerEsp::toString()
    {
        ostringstream sout;
        time_t rawtime;
        time(&rawtime);

        sout << "Network Container description" << endl;
        sout << "Timestamp: " << ctime(&rawtime) << endl;

        for (uint i = 0; i < networks.size() - 1; i++) {
            sout << "N" << i << ":" << endl;
            sout << "I:2" << "<-" << endl;
            sout << networks[i]->toString();
            sout << "O:5" << "->C" << networks.size() - 1 << endl;
            sout << endl;
        }

        sout << "C" << networks.size() - 1 << ":" << endl;
        sout << "I:10" << "<-N" << endl;
        sout << networks[networks.size() - 1]->toString();
        sout << "O:5" << "->" << endl;
        sout << endl;

        return sout.str();
    }
}

