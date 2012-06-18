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
    void FeedForwardNetwork::activate(const vector<double> &input, vector<double> &output)
    {
        VLOG(4) << "Activating feed forward network";
        register uint i, j, neuron_input_connections, output_weight_index;

        vector<double> biasedInput(input);

        if(bias)
            biasedInput.push_back(1.0); // Bias

        if(( biasedInput.size() + output.size() ) != geneSize){
            LOG(ERROR) << "Mismatch between genesize and input + output size";
            LOG(ERROR) << "Input size is " << biasedInput.size();
            LOG(ERROR) << "Output size is " << output.size();
            LOG(ERROR) << "Gene size is " << geneSize;
        }

        VLOG(5) << "Input is " << vecToString(biasedInput);

        // evaluate hidden/output layer
        for (i = 0; i < numHiddenNeurons; ++i) {  //for each hidden unit
            //if(!pop[i]->lesioned){
            activation[i] = 0.0;
            for (j = 0; j < biasedInput.size(); ++j) {
                activation[i] += neurons[i]->weight[j] * biasedInput[j];
            }
            //inner_product(pop[i]->weight.begin(),
            //    pop[i]->weight.end(),
            //    input.begin(), 0.0);
            VLOG(5) << "Activation is " << vecToString(activation);
            //LOG(INFO) << "Weights are " << vecToString(neurons[i]->weight);
            activation[i] = sigmoid(activation[i]);
            //}
        }



        VLOG(4) << "Inputs evaluated";

        neuron_input_connections = biasedInput.size();
        output_weight_index = neuron_input_connections;

        for (i = 0; i < output.size(); ++i) {
            output[i] = 0.0;
            for (j = 0; j < numHiddenNeurons; ++j) {
                output[i] += activation[j] * neurons[j]->weight[output_weight_index];
            }
            output_weight_index++;
        }

        VLOG(5) << "Output is " << vecToString(output);

        VLOG(4) << "Done activating feed forward network";
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

