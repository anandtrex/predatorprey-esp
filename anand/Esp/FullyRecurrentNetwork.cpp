/*
 * FullyRecurrentNetwork.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "FullyRecurrentNetwork.h"

namespace EspPredPreyHunter
{
    void FullyRecurrentNetwork::activate(vector<double> &input, vector<double> &output)
    {
        register int i, j, r;

        /* evaluate hidden/output layer */
        for (r = 0; r < 1; ++r) {
            for (i = 0; i < numNeurons; ++i)
                input[NUM_INPUTS + i] = activation[i];
            for (i = 0; i < numNeurons; ++i) { /*for each hidden unit*/
                activation[i] = 0.0;
                if (!pop[i]->lesioned) {
                    for (j = 0; j < NUM_INPUTS - 1; ++j)
                        activation[i] += pop[i]->weight[j] * input[j];
                    activation[i] = sigmoid(activation[i]);
                }
            }
            for (i = 0; i < NUM_OUTPUTS; ++i)
                output[i] = activation[i];
        }
    }

    void FullyRecurrentNetwork::save(char *filename)
    {
        char newname[40];

        strcpy(newname, filename);
        strcat(newname, "_FR");
        //saveText(newname);
    }

    void FullyRecurrentNetwork::addNeuron()
    {

        addConnection(NUM_INPUTS - 1);
        ++numNeurons;
        pop.push_back(new neuron);
    }

    void FullyRecurrentNetwork::removeNeuron(int sp)
    {
        removeConnection(NUM_INPUTS + sp);
        --numNeurons;
        delete pop[sp];
        pop.erase(pop.begin() + sp);
    }
}

