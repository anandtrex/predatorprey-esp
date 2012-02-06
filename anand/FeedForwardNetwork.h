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
            FeedForwardNetwork(const uint& numHiddenNeurons, const uint& neuronGeneSize)
                    : Network(numHiddenNeurons, neuronGeneSize)
            {
            }
            void activate(vector<double> &input, vector<double> &output, int inputSize_combiner);
            void addNeuron();
            void removeNeuron(int sp);
            void save(char *filename);
        };
}


#endif /* FEEDFORWARDNETWORK_H_ */
