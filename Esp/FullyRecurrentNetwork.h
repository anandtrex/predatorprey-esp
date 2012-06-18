/*
 * FullyRecurrentNetwork.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#ifndef FULLYRECURRENTNETWORK_H_
#define FULLYRECURRENTNETWORK_H_

#include "Network.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class FullyRecurrentNetwork: public Network
    {
    public:
        FullyRecurrentNetwork(int size, int r = 2)
                : Network(size)
        {
            relax = r;
        }

        void activate(vector<double> &input, vector<double> &output);
        void addNeuron();
        void removeNeuron(int sp);
        void save(char *filename);
    private:
        int relax;
    };
}

#endif /* FULLYRECURRENTNETWORK_H_ */
