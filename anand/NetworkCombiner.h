/*
 * NetworkCombiner.h
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#ifndef NETWORKCOMBINER_H_
#define NETWORKCOMBINER_H_

#include "common.h"
#include "ComboNetwork.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class NetworkCombiner
    {
    public:
        virtual NetworkCombiner(vector<ComboNetwork*> networks) = 0;
        virtual ~NetworkCombiner();
        virtual uint getOutput(vector<double>& input, vector<double>& output) = 0;
    };
}


#endif /* NETWORKCOMBINER_H_ */
