/*
 * FeedForwardNetwork.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#ifndef FEEDFORWARDNETWORK_H_
#define FEEDFORWARDNETWORK_H_

#include "Network.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class FeedForwardNetwork: public Network
    {
    public:
        FeedForwardNetwork(const uint& numHiddenNeurons, const uint& subPopSize,
                const uint& numInputs, const uint& numOutputs, bool bias)
                : Network(numHiddenNeurons, subPopSize, numInputs, numOutputs, bias)
        {
        }
        void activate(const vector<double> &input, vector<double> &output);
        void addNeuron();
        void removeNeuron(int sp);
        void save(char *filename);
    };
}

#endif /* FEEDFORWARDNETWORK_H_ */
