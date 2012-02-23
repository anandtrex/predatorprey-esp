/*
 * NetworkContainer.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: anand
 */
#include <sstream>
#include <time.h>

#include "NetworkContainerEsp.h"

namespace EspPredPreyHunter
{
    NetworkContainerEsp::NetworkContainerEsp()
            : networks(0)
    {
    }

    NetworkContainerEsp::NetworkContainerEsp(const vector<Network*> networks)
            : networks(networks)
    {
        if (networks.size() == 1)
            combine = 0;
        else
            combine = 1;
    }

    NetworkContainerEsp::~NetworkContainerEsp()
    {

    }

    void NetworkContainerEsp::setNetwork(const NetworkContainer& networkContainer)
    {
        networks = networkContainer.getNetworks();
        if (networks.size() == 1)
            combine = 0;
        else
            combine = 1;
    }

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

    vector<Network*> NetworkContainerEsp::getNetworks() const
    {
        return networks;
    }

    void NetworkContainerEsp::activate(const vector<double>& input, vector<double>& output)
    {
        if (networks.size() == 0)
            LOG(FATAL) << "Trying to activate empty container!";

        vector<double> tempSingleOutputs;
        for (uint i = 0; i < networks.size() - combine; i++) {     // sans combiner
            // FIXME Assuming that the number of outputs is 5
            vector<double> tempOutput = vector<double>(5);
            // FIXME assuming number of inputs is 2
            const double tempInput[] = { input[2 * i], input[2 * i + 1] };
            networks[i]->activate(makeVector(tempInput), tempOutput);
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

        for(uint i = 0; i < networks.size() - 1; i++){
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

