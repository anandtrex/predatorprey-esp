/*
 * FeedForwardNetwork.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "FeedForwardNetwork.h"
#include "Neuron.h"

namespace EspPredPreyHunter
{
    // NOTE Does the calculation of the output of the network here
    void FeedForwardNetwork::activate(const vector<double> &input, vector<double> &output)
    {
        register int i, j, neuron_input_connections, output_weight_index;
        neuron_input_connections = input.size() - 1;
        //neuron_input_connections = 2;  //Number of inputs to each sensory neural network
        // evaluate hidden/output layer
        for (i = 0; i < numHiddenNeurons; ++i) {  //for each hidden unit
            //if(!pop[i]->lesioned){
            activation[i] = 0.0;
            for (j = 0; j < input.size() - 1; ++j) {
                activation[i] += neurons[i]->weight[j] * input[j];
            }
            //inner_product(pop[i]->weight.begin(),
            //    pop[i]->weight.end(),
            //    input.begin(), 0.0);
            activation[i] = sigmoid(activation[i]);
            //}
        }

        output_weight_index = neuron_input_connections;

        for (i = 0; i < output.size() - 1; ++i) {
            output[i] = 0.0;
            for (j = 0; j < numHiddenNeurons; ++j) {
                output[i] += activation[j] * neurons[j]->weight[output_weight_index];
            }
            output_weight_index++;
        }
    }

    void FeedForwardNetwork::save(char *filename)
    {
        char newname[40];

        strcpy(newname, filename);
        strcat(newname, "_FF");
        //saveText(newname);
    }

    void FeedForwardNetwork::addNeuron()
    {

        //  addConnection(NUM_INPUTS-1);
        ++numHiddenNeurons;
        neurons.push_back(new Neuron(geneSize));
    }

    void FeedForwardNetwork::removeNeuron(int sp)
    {
//  removeConnection(NUM_INPUTS+sp);
        --numHiddenNeurons;
//  delete pop[sp];
        neurons.erase(neurons.begin() + sp);
    }
}

