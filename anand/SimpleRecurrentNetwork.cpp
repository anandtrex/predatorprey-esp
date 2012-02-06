/*
 * SimpleRecurrentNetwork.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "SimpleRecurrentNetwork.h"

namespace EspPredPreyHunter
{
    void SimpleRecurrentNetwork::activate(vector<double> &input, vector<double> &output)
    {
        register int i, j;

        /* evaluate hidden/output layer */
        for (i = 0; i < numNeurons; ++i) {
            input[NUM_INPUTS + i] = activation[i];
            /* printf("%f \n", net[i]->activation);   */
        }
        for (i = 0; i < numNeurons; ++i) { /*for each hidden unit*/
            activation[i] = 0.0;
            if (!pop[i]->lesioned) {
                for (j = 0; j < NUM_INPUTS; ++j) {
                    activation[i] += pop[i]->weight[j] * input[j];
                    //printf("%f\n", activation[i]);
                }
                //inner_product(pop[i]->weight.begin(),
                //    pop[i]->weight.end(),
                //    input.begin(), 0.0);
                activation[i] = sigmoid(activation[i]);
                // fabs(pop[i]->weight[NUM_INPUTS+1]/6.0)) * 2 - 1.0;
                //printf("%f\n", activation[i]);
            }
        }
        for (i = 0; i < NUM_OUTPUTS; ++i) {
            output[i] = 0.0;
            for (j = 0; j < numNeurons; ++j) {
                output[i] += activation[j] * pop[j]->weight[NUM_INPUTS + i];
            }
            output[i] = sigmoid(output[i]);
            //printf("%f\n", output[0]); //->weight[j]);
        }
    }

    void SimpleRecurrentNetwork::save(char *filename)
    {
        char newname[40];

        strcpy(newname, filename);
        strcat(newname, "_SRN");
        //saveText(newname);
    }

    void SimpleRecurrentNetwork::addNeuron()
    {

        addConnection(NUM_INPUTS - 1);
        ++numNeurons;
        pop.push_back(new neuron);
    }

    void SimpleRecurrentNetwork::removeNeuron(int sp)
    {
        removeConnection(NUM_INPUTS + sp);
        --numNeurons;
        delete pop[sp];
        pop.erase(pop.begin() + sp);
    }
}

