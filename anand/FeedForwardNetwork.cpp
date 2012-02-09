/*
 * FeedForwardNetwork.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "FeedForwardNetwork.h"

namespace EspPredPreyHunter
{
    // NOTE Does the calculation of the output of the network here
    void FeedForwardNetwork::activate(vector<double> &input, vector<double> &output, /*int num_of_prey,
     int num_of_predators, int num_teams_prey, int num_teams_predator,*/int inputSize_combiner)
    {
        register int i, j, neuron_input_connections, output_weight_index;
        if (IS_COMBINER_NW == 1) {
            neuron_input_connections = inputSize_combiner; //Number of inputs to each combiner neural network
        } else {
            neuron_input_connections = 2;  //Number of inputs to each sensory neural network
        }
        // evaluate hidden/output layer
        for (i = 0; i < numNeurons; ++i) {  //for each hidden unit
            //if(!pop[i]->lesioned){
            activation[i] = 0.0;
            for (j = 0; j < neuron_input_connections; ++j) {
                activation[i] += pop[i]->weight[j] * input[j];
                //printf("%f\n", activation[i]);
            }
            //inner_product(pop[i]->weight.begin(),
            //    pop[i]->weight.end(),
            //    input.begin(), 0.0);
            activation[i] = sigmoid(activation[i]);
            //printf("%f\n", activation[i]);
            //}
        }

        // If there is a second hidden layer
        // NOTE FIXME Will work only if the size of both the hidden layers are the same!
        vector<double> oldActivation = vector<double>(activation);
        /*
         if (numNeurons2 > 0) {
         if (IS_COMBINER_NW) {
         LOG(ERROR) << "Second layer of combiner network!!";
         }
         //DLOG(INFO) << "pop2 size is " << pop2.size();
         //DLOG(INFO) << "numNeurons2 is " << numNeurons2;
         for (int i = 0; i < numNeurons2; i++) {
         activation[i] = 0.0;
         // take the activation of each of the first layer of hidden neurons
         //DLOG(INFO) << "pop2[" << i <<"]->weight size is " << pop2[i]->weight.size();
         for (int j = 0; j < numNeurons2; j++) {
         //DLOG(INFO) << "pop2[i]->weight[j] is " << pop2[i]->weight[j];
         activation[i] += pop2[i]->weight[j] * oldActivation[i];
         }
         activation[i] = sigmoid(activation[i]);
         }
         }*/

        output_weight_index = neuron_input_connections;

        if (IS_COMBINER_NW == 1) {
            for (i = 0; i < NUM_OUTPUT_PRED_COMBINER; ++i) {
                output[i] = 0.0;
                for (j = 0; j < numNeurons; ++j) {
                    output[i] += activation[j] * pop[j]->weight[output_weight_index];
                }
                output_weight_index++;
                output[i] = sigmoid(output[i]);
                //printf("%f\n", output[0]); //->weight[j]);
            }
        } else {
            for (i = 0; i < NUM_OUTPUTS; ++i) {
                output[i] = 0.0;
                for (j = 0; j < numNeurons; ++j) {
                    output[i] += activation[j] * pop[j]->weight[output_weight_index];
                }
                output_weight_index++;
                output[i] = sigmoid(output[i]);
                //printf("%f\n", output[0]); //->weight[j]);
            }
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
        ++numNeurons;
        pop.push_back(new neuron);
    }

    void FeedForwardNetwork::removeNeuron(int sp)
    {
//  removeConnection(NUM_INPUTS+sp);
        --numNeurons;
//  delete pop[sp];
        pop.erase(pop.begin() + sp);
    }
}

