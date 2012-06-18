/*
 * SecondOrderRecurrentNetwork.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#ifndef SECONDORDERRECURRENTNETWORK_H_
#define SECONDORDERRECURRENTNETWORK_H_

#include "Network.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class SecondOrderRecurrentNetwork: public Network
        {
        public:
            SecondOrderRecurrentNetwork(int size);

            void activate(vector<double> &input, vector<double> &output);
            void addNeuron();
            void removeNeuron(int sp);
            void save(char *filename);
        private:
            int outOffset;
            vector<vector<double> > newWeights;
        };
}


#endif /* SECONDORDERRECURRENTNETWORK_H_ */
