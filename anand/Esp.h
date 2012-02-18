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

#include "common.h"
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
         * This is equal to the total number of EVOLVING agents for which networks have to be evolved using ESP (excluding combiner networks)
         * For predator-prey-hunter, this is equal to (num_predators)
         */
        //const uint numAgents;
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

        /**
         * This is equal to the number of all the other agents in the task excluding this team of agents
         * For predator-prey-hunter this is equal to
         * ( num_predators * (num_teams_predators - 1) + num_prey * num_teams_prey + num_hunters * num_teams_hunters )
         * for a team of predators
         */
        //const uint numOtherAgents;
        //const uint numActions;
        uint numNetworks;

        // These two store redundant information (based on numOtherAgents and numActions)
        // This is for convenience, and clearer code
        /**
         * This is equal to numOtherAgents * 2 NOTE: TIMES 2
         */
        uint numInputs;

        /**
         * This is equal to numActions
         */
        uint numOutputs;

        uint combine;

        vector<Network*> networks;

        /**
         * i-th element corresponds to the vector of Subpop for the i-th network, where in the vector of SubPop,
         *  the j-th element corresponds to the j-th neuron in the corresponding network
         */
//        vector<vector<SubPop*> > hidden_neuron_populations; // num networks x num hidden neurons per network
        uint generation;

        vector<Network*> overall_best_networks;

        uint numTrials;

        NetworkContainer* networkContainer;

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

        /**
         * evaluation stage.  Create numTrials networks, each containing
         * ZETA neurons selected randomly from the population.  Each network
         * is evaluated and each participating neuron receives the fitness
         * evaluations of each network it parcipates in.
         */
        void performEval();

        // NOTE Below are existing private member functions
        //Esp(const Esp &);     // just for safety
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
        void printStats();

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
        ~Esp();

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

        void findChampion();
        void loadSeedNet(char *);
        void addSubPop(int pred, int num_of_predators, int num_of_prey);
        void endEvolution();
        static vector<vector<vector<Network*> > > hall_of_fame_pred;     // EVOLVE_PREY vector of vector of best networks
    };
}

#endif /* ESP_H_ */
