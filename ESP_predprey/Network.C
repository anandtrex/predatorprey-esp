//////////////////////////////////////////////////////////////////////
//
// Network
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "Environment.h"
#include "Network.h"

using namespace std;

extern int NUM_INPUTS;
extern int NUM_INPUTS_PREY;
extern int NUM_OUTPUTS;
extern int NUM_OUTPUTS_PREY;
extern int NUM_OUTPUT_PRED_COMBINER;
extern int NUM_OUTPUT_PREY_COMBINER;
extern int EVALTRIALS;
extern bool IS_PREY;
extern bool pred_communication;
extern bool prey_communication;
extern int IS_COMBINER_NW;  //Aditya : March 05 - Combiner Network

Network::~Network()
{
    for (int i = 0; i < numNeurons; ++i)
        pop[i] = NULL;
    //pop.clear();
}

//copy a network
void Network::operator=(Network &n)
{
    fitness = n.fitness;
    for (int i = 0; i < numNeurons; ++i)
        *pop[i] = *n.pop[i];

    //  return this;
}

// Places neuron n into this network
void Network::setNeuron(neuron *n, int position)
{
    pop[position] = n;
}

// assigns this to n
void Network::setNetwork(Network *n)
{
    fitness = n->fitness;
    for (int i = 0; i < numNeurons; ++i)
        pop[i] = n->pop[i];
}

// add network fitness to its neurons
void Network::addFitness()
{
    for (int i = 0; i < numNeurons; ++i)
        pop[i]->fitness += fitness;
}

void Network::resetActivation()
{
    for (int i = 0; i < numNeurons; ++i)
        activation[i] = 0.0;
}

// increment net's neurons' tests field
void Network::incrementTests()
{
    for (int i = 0; i < numNeurons; ++i)
        ++pop[i]->tests;
}

SecondOrderRecurrentNetwork::SecondOrderRecurrentNetwork(int size) :
        Network(size), newWeights(size)
{
    int j;

    outOffset = NUM_INPUTS * numNeurons;
    for (int i = 0; i < size; ++i)
        for (j = 0; j < outOffset; ++j)
            newWeights[i].push_back(0.0);
}

inline double sigmoid(double x, double slope = 1.0)
{
    return (1 / (1 + exp(-(slope * x))));
}

// NOTE Does the calculation of the output of the network here
void FeedForwardNetwork::activate(vector<double> &input, vector<double> &output, int num_of_prey,
        int num_of_predators, int num_teams_prey, int num_teams_predator, int inputSize_combiner)
{
    register int i, j, neuron_input_connections, output_weight_index;
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            neuron_input_connections = inputSize_combiner;  //Number of inputs to each combiner neural network
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
            //	  pop[i]->weight.end(),
            //	  input.begin(), 0.0);
            activation[i] = sigmoid(activation[i]);
            //printf("%f\n", activation[i]);
            //}
        }

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

    else if (IS_PREY) {

        if (IS_COMBINER_NW == 1) {
            neuron_input_connections = inputSize_combiner;  //Number of inputs to each combiner neural network
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
            //	  pop[i]->weight.end(),
            //	  input.begin(), 0.0);
            activation[i] = sigmoid(activation[i]);
            //printf("%f\n", activation[i]);
            //}
        }

        output_weight_index = neuron_input_connections;

        if (IS_COMBINER_NW == 1) {
            for (i = 0; i < NUM_OUTPUT_PREY_COMBINER; ++i) {
                output[i] = 0.0;
                for (j = 0; j < numNeurons; ++j) {
                    output[i] += activation[j] * pop[j]->weight[output_weight_index];
                }
                output[i] = sigmoid(output[i]);
                output_weight_index++;
            }
        }

        else {
            for (i = 0; i < NUM_OUTPUTS_PREY; ++i) {
                output[i] = 0.0;
                for (j = 0; j < numNeurons; ++j) {
                    output[i] += activation[j] * pop[j]->weight[output_weight_index];
                }
                output[i] = sigmoid(output[i]);
                output_weight_index++;
            }
        }

    }

}

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
            //	  pop[i]->weight.end(),
            //	  input.begin(), 0.0);
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

void SecondOrderRecurrentNetwork::save(char *filename)
{
    char newname[40];

    strcpy(newname, filename);
    strcat(newname, "_2ndOrder");
    //saveText(newname);
}

void FullyRecurrentNetwork::save(char *filename)
{
    char newname[40];

    strcpy(newname, filename);
    strcat(newname, "_FR");
    //saveText(newname);
}

void FeedForwardNetwork::save(char *filename)
{
    char newname[40];

    strcpy(newname, filename);
    strcat(newname, "_FF");
    //saveText(newname);
}

void SimpleRecurrentNetwork::save(char *filename)
{
    char newname[40];

    strcpy(newname, filename);
    strcat(newname, "_SRN");
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

void FeedForwardNetwork::addNeuron()
{

//  addConnection(NUM_INPUTS-1);
    ++numNeurons;
    pop.push_back(new neuron);
}

void FullyRecurrentNetwork::addNeuron()
{

    addConnection(NUM_INPUTS - 1);
    ++numNeurons;
    pop.push_back(new neuron);
}

void SimpleRecurrentNetwork::addNeuron()
{

    addConnection(NUM_INPUTS - 1);
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

void FullyRecurrentNetwork::removeNeuron(int sp)
{
    removeConnection(NUM_INPUTS + sp);
    --numNeurons;
    delete pop[sp];
    pop.erase(pop.begin() + sp);
}

void SimpleRecurrentNetwork::removeNeuron(int sp)
{
    removeConnection(NUM_INPUTS + sp);
    --numNeurons;
    delete pop[sp];
    pop.erase(pop.begin() + sp);
}

void SecondOrderRecurrentNetwork::removeNeuron(int sp)
{
    --numNeurons;
    for (int i = 1; i < NUM_INPUTS + 1; ++i)  // remove connection to neurons in spops
        removeConnection(numNeurons * i);

    delete pop[sp];
    pop.erase(pop.begin() + sp);
}

#define LESION_THRESHOLD 0.9
#define LESION_EVALTRIALS 20
int Network::lesion(Environment &e, vector<Network*>& team, int num_of_predators, int num_of_prey)
{
    int sp = -1;
    double max = 0;
    vector<double> tempFitness;
    double lesionFitness = 0;
    vector<Network*> team_prey;
    vector<double> temp_return_value;

    for (int j = 0; j < 2; j++) {  //Initializing the vector
        tempFitness.push_back(0.0);
        temp_return_value.push_back(0.0);
    }

    for (int i = 0; i < LESION_EVALTRIALS; i++) {
        //temp_return_value = e.evalNet(team, team_prey);
        for (int j = 0; j < 2; j++) {  //Initializing the vector
            tempFitness[0] = tempFitness[0] + temp_return_value[0];
            tempFitness[1] = tempFitness[1] + temp_return_value[1];
        }
    }
    tempFitness[0] /= LESION_EVALTRIALS;
    tempFitness[1] /= LESION_EVALTRIALS;

    printf("Unlesioned : %f\n", tempFitness[0]);

    for (int i = 0; i < numNeurons; ++i) {
        pop[i]->lesioned = true;

        lesionFitness = 0;
        for (int j = 0; j < LESION_EVALTRIALS; j++)
            //   lesionFitness += e.evalNet(team, team_prey, 1, 0, 0, -1, 0, 2); EVOLVE_PREY - commented temporarily
            lesionFitness /= LESION_EVALTRIALS;

        cout << "Neuron " << i << " lesioned. Remaining fitness: " << lesionFitness << endl;

        pop[i]->lesioned = false;
        if (lesionFitness > max) {
            max = lesionFitness;
            sp = i;
        }
    }

    if (max >= (tempFitness[0] * LESION_THRESHOLD))
        return sp;
    else
        return -1;
}

