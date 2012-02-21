/*
 * Esp.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef ESP_H_
#define ESP_H_

#include <fstream>
#include <vector>
#include <math.h>

#include "Network.h"
#include "SubPop.h"
#include "NetworkContainer.h"

namespace EspPredPreyHunter
{
    using std::vector;
    using std::ofstream;

    enum
    {
        NUM_ACTIONS = 5
    };

    /**
     * This class contains functions to evolve networks for a predator.
     */
    class Esp
    {
        uint inputDimensions;

        /**
         * This is equal to ( numNetworks * numInputs + COMBINE * numInputs )
         */
        uint totalNumNetworks;


        /**
         * Number of hidden neurons. Assumed to be the same for all the networks
         * TODO Have different number of hidden neurons for each network
         */
        const uint numHiddenNeurons;

        /**
         * This is the number of weights that the neuron has.
         * 1 to i are the inputs (where i is the num of inputs for the given network, which is 2 in the case of a 2D grid-world)
         * i to n are the outputs (where n is the number of outputs for the given network, which is 5 in all cases)
         */
        uint neuronGeneSize;

        /**
         * This is the number of weights that the combiner neuron has.
         * 1 to i are the inputs (where i is the num of inputs for the given network, which is numOutputs * numOtherAgents in the case of a 2D grid-world)
         * i to n are the outputs (where n is the number of outputs for the given network, which is 5 in all cases)
         */
        uint combinerNeuronGeneSize;
        const uint popSize;
        const uint netType;
        uint numNetworks;
        uint numInputs;

        /**
         * This is equal to numActions
         */
        uint numOutputs;
        uint combine;
        vector<Network*> networks;
        uint generation;
        uint numTrials;

        /**
         * output a new network of the appropriate type.
         * @param networkType
         * @param neuronGeneSize - this parameter is  taken, because the network can either be a combiner network
         * or not
         * @return
         */
        Network* generateNetwork(const uint& networkType, const uint& neuronGeneSize);
        uint getNeuronGeneSize();
        uint getCombinerNeuronGeneSize();

        // NOTE Below are existing private member functions
        Esp &operator=(const Esp &);
        void save(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey);
        SubPop* load(char *fname);
        void average(int num_teams_predator, int num_teams_prey);
        void printNeurons();
        void handleStagnation(int num_of_predators, int num_of_prey);
        void newDeltaPhase(int pred);
        int removeSubPop(int sp, int pred, int num_of_predators, int num_of_prey);
        int removeSubPop(vector<Network*>& team, int pred, int num_of_predators, int num_of_prey);
        void addConnections();
        void removeConnections(int sp);

    public:

        Esp();

        /**
         * Constructor
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         * @param numNetworks - Number of networks that have to be evolved
         * @param numInputsPerNetwork - Number of inputs per network (doesn't include combiner network)
         * @param numOutputsPerNetwork - Number of outputs per network (includes combiner network)
         */
        Esp(const uint& nHiddenNeurons, const uint& popSize, const uint& netTp,
                        const uint& numNetworks, const uint& numInputsPerNetwork, const uint& numOutputsPerNetwork);

        /**
         * Constructor
         * @param fileName
         * @param numNetworks - total number of networks
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         */
        Esp(const char* fileName, const uint& numNetworks, const uint& nHiddenNeurons,
                const uint& popSize, const uint& netTp);

        /**
         * reset fitness values
         */
        void evalReset();

        /**
         * Destructor
         */
        ~Esp()
        {
            for (uint i = 0; i < totalNumNetworks; i++) {
                delete networks[i];
            }
            networks.clear();
        }

        /**
         * Gets the vector of networks to be used for evaluation after
         * setting the neurons appropriately from the subpopulation
         * @return
         */
        NetworkContainer* getNetwork();

        /**
         * evolve is the main genetic function.  The subpopulations are first
         * evaluated in the given task.  Then for each subpop the neurons are
         * ranked (sorted by fitness) and recombined using crossover.
         * Mating is allowed only between the top 'numBreed' members of
         * the same subpop. The neurons are then mutated.
         * @param cycles - number of cycles to run evolve
         */
        void evolve(const uint& cycles);

        /**
         * eval_pop evaluates entire population to get the average fitness of
         * each neuron.  This function is called by evolve and should put the
         * fitness of each neuron in the neuron's fitness field
         * (pop[i]->fitness).
         */
        void evalPop();

        void loadSeedNet(char *);
        void addSubPop(int pred, int num_of_predators, int num_of_prey);
    };
}

#endif /* ESP_H_ */
