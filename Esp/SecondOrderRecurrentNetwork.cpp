/*
 * SecondOrderRecurrentNetwork.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "SecondOrderRecurrentNetwork.h"

namespace EspPredPreyHunter
{
    void SecondOrderRecurrentNetwork::save(char *filename)
    {
        char newname[40];

        strcpy(newname, filename);
        strcat(newname, "_2ndOrder");
        //saveText(newname);
    }

    void SecondOrderRecurrentNetwork::addNeuron()
    {
        int i;

        for (i = 1; i < NUM_INPUTS + 1; ++i)  // add connection to neurons in spops
            addConnection(numNeurons * i + i - 1);
        ++numNeurons;
        pop.push_back(new neuron);
    }

    void SecondOrderRecurrentNetwork::removeNeuron(int sp)
    {
        --numNeurons;
        for (int i = 1; i < NUM_INPUTS + 1; ++i)  // remove connection to neurons in spops
            removeConnection(numNeurons * i);

        delete pop[sp];
        pop.erase(pop.begin() + sp);
    }

    void SecondOrderRecurrentNetwork::activate(vector<double> &input, vector<double> &output)

    {
        register int i, j, k;

        // calculate new weights wij
        for (i = 0; i < numNeurons; ++i)
            if (!pop[i]->lesioned) {
                for (j = 0; j < NUM_INPUTS; ++j) {
                    (newWeights[i])[j] = 0.0;
                    for (k = 0; k < numNeurons; ++k) {
                        (newWeights[i])[j] += activation[k] * pop[i]->weight[j * numNeurons + k];
                    }
                }
            }
        // activate hidden layer
        for (i = 0; i < numNeurons; ++i) {
            activation[i] = 0.0;
            if (!pop[i]->lesioned) {
                for (j = 0; j < NUM_INPUTS; ++j) {
                    activation[i] += (newWeights[i])[j] * input[j];
                    //printf("%f\n", activation[i]);
                }
                activation[i] = sigmoid(activation[i]);  //, fabs(pop[i]->weight[0]/6.0));
            }
        }
        // for (i=0;i<NUM_OUTPUTS;++i)
        //output[i] = activation[i];
        for (i = 0; i < NUM_OUTPUTS; ++i) {
            output[i] = 0.0;
            for (j = 0; j < numNeurons; ++j) {
                output[i] += activation[j] * pop[j]->weight[outOffset + i];
            }
            output[i] = sigmoid(output[i]);
            //printf("%f\n", output[0]); //->weight[j]);
        }
    }

    SecondOrderRecurrentNetwork::SecondOrderRecurrentNetwork(int size)
            : Network(size), newWeights(size)
    {
        int j;

        outOffset = NUM_INPUTS * numNeurons;
        for (int i = 0; i < size; ++i)
            for (j = 0; j < outOffset; ++j)
                newWeights[i].push_back(0.0);
    }
}

