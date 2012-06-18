/*
 * SimpleRecurrentNetwork.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#ifndef SIMPLERECURRENTNETWORK_H_
#define SIMPLERECURRENTNETWORK_H_

#include "Network.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class SimpleRecurrentNetwork: public Network
    {
    public:
        SimpleRecurrentNetwork(int size)
                : Network(size)
        {
        }

        void activate(vector<double> &input, vector<double> &output);
        void addNeuron();
        void removeNeuron(int sp);
        void save(char *filename);
    };
}


#endif /* SIMPLERECURRENTNETWORK_H_ */
