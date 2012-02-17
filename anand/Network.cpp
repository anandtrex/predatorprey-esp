/*
 * Network.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */
#include "Neuron.h"
#include "common.h"
#include "Network.h"

#include <stdio.h>

namespace EspPredPreyHunter
{
    using std::vector;
    using std::endl;

    Network::Network(const uint& numHiddenNeurons, const uint& geneSize, const uint& subPopSize) :
            subPopSize(subPopSize), networkSubPop(numHiddenNeurons), geneSize(geneSize), numHiddenNeurons(numHiddenNeurons)
    {
        VLOG(4) << "Creating generic network";
        activation = vector<double>(numHiddenNeurons);
        neurons = vector<Neuron*>(numHiddenNeurons);

        if (MIN)
            fitness = 10000000;
        else
            fitness = 0.0;

        evolvable = true;
        create();
    }

    Network::~Network()
    {
        /*
        for (int i = 0; i < numHiddenNeurons; ++i)
            neurons[i] = NULL;
            */
        //pop.clear();
    }

    //copy a network
    void Network::operator=(Network &n)
    {
        fitness = n.fitness;
        for (int i = 0; i < numHiddenNeurons; ++i)
            *neurons[i] = *n.neurons[i];

        //  return this;
    }

    // Places neuron n into this network
    void Network::setNeuron(Neuron *n, int position)
    {
        neurons[position] = n;
    }

    // assigns this to n
    void Network::setNetwork(Network *n)
    {
        fitness = n->fitness;
        for (int i = 0; i < numHiddenNeurons; ++i)
            neurons[i] = n->neurons[i];
    }

    // add network fitness to its neurons
    void Network::addFitness()
    {
        for (int i = 0; i < numHiddenNeurons; ++i)
            neurons[i]->fitness += fitness;
    }

    void Network::resetActivation()
    {
        for (int i = 0; i < numHiddenNeurons; ++i)
            activation[i] = 0.0;
    }

    // increment net's neurons' tests field
    void Network::incrementTests()
    {
        for (int i = 0; i < numHiddenNeurons; ++i)
            ++neurons[i]->tests;
    }

#define LESION_THRESHOLD 0.9
#define LESION_EVALTRIALS 20

    int Network::lesion()
    {
        int sp = -1;
        double max = 0;
        vector<double> tempFitness;
        double lesionFitness = 0;
        vector<Network*> team_prey;
        vector<double> temp_return_value;

        for (int j = 0; j < 2; j++) {         //Initializing the vector
            tempFitness.push_back(0.0);
            temp_return_value.push_back(0.0);
        }

        for (int i = 0; i < LESION_EVALTRIALS; i++) {
            //temp_return_value = e.evalNet(team, team_prey);
            for (int j = 0; j < 2; j++) {         //Initializing the vector
                tempFitness[0] = tempFitness[0] + temp_return_value[0];
                tempFitness[1] = tempFitness[1] + temp_return_value[1];
            }
        }
        tempFitness[0] /= LESION_EVALTRIALS;
        tempFitness[1] /= LESION_EVALTRIALS;

        LOG(INFO) << "Unlesioned: " << tempFitness[0] << endl;

        for (int i = 0; i < numHiddenNeurons; ++i) {
            neurons[i]->lesioned = true;

            lesionFitness = 0;
            for (int j = 0; j < LESION_EVALTRIALS; j++)
                //   lesionFitness += e.evalNet(team, team_prey, 1, 0, 0, -1, 0, 2); EVOLVE_PREY - commented temporarily
                lesionFitness /= LESION_EVALTRIALS;

            LOG(INFO) << "Neuron " << i << " lesioned. Remaining fitness: " << lesionFitness
                    << endl;

            neurons[i]->lesioned = false;
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

    void Network::create()
    {
        VLOG(4) << "Running create in generic network";
        for (int i = 0; i < numHiddenNeurons; i++) {
            networkSubPop[i] = new SubPop(subPopSize, geneSize);
            networkSubPop[i]->create();
        }
    }

    void Network::setNeurons()
    {
        for (int i = 0; i < numHiddenNeurons; i++) {
            setNeuron(networkSubPop[i]->selectNeuron(), i);
        }
    }

    void Network::setFitness(const double& fitness)
    {
        VLOG(5) << "Setting fitness for each hidden neuron";
        for (int i = 0; i < numHiddenNeurons; i++) {
            neurons[i]->fitness += fitness;
        }
    }

    void Network::evalReset()
    {
        for (int i = 0; i < numHiddenNeurons; i++) {
            networkSubPop[i]->evalReset();
        }
    }

    void Network::average()
    {
        VLOG(5) << "Averaging neuron fitness";
        // TODO Check if this has to be done for all the subpop neurons
        for (int i = 0; i < networkSubPop.size(); i++) {
            networkSubPop[i]->average();
        }
    }

    void Network::qsortNeurons()
    {
        VLOG(5) << "Sorting neurons";
        for (int i = 0; i < networkSubPop.size(); i++) {
            networkSubPop[i]->qsortNeurons();
        }
    }

    void Network::mutate()
    {
        VLOG(5) << "Doing mutate in network";
        for (int i = 0; i < networkSubPop.size(); i++) {
            networkSubPop[i]->mutate();
        }
    }

    void Network::recombineHallOfFame(Network* network)
    {
        VLOG(5) << "Doing recombine hall of fame in network";
        for (int i = 0; i < networkSubPop.size(); i++) {
            networkSubPop[i]->recombineHallOfFame(network, i);
        }
    }
}
