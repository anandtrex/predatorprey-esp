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

#include "common.h"
#include "Network.h"
#include "SubPop.h"
#include "Experiment.h"

namespace EspPredPreyHunter
{
    using std::vector;
    using std::ofstream;
    using PredatorPreyHunter::Experiment;

    /**
     * This class contains functions to evolve networks for a whole team of predators. So this should be shared across all agents that
     * belong to a given team. This assumes that this whole thing is used only for a single team.
     */
    class EspExperiment: Experiment
    {
        uint inputDimensions;

        /**
         * This is equal to the total number of EVOLVING agents for which networks have to be evolved using ESP (excluding combiner networks)
         * For predator-prey-hunter, this is equal to (num_predators)
         */
        const uint numAgents;

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

        const uint netTp;

        /**
         * This is equal to the number of all the other agents in the task excluding this team of agents
         * For predator-prey-hunter this is equal to
         * ( num_predators * (num_teams_predators - 1) + num_prey * num_teams_prey + num_hunters * num_teams_hunters )
         * for a team of predators
         */
        const uint numOtherAgents;
        const uint numActions;

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

        enum
        {
            COMBINE = 1
        };     // is 0 or 1 depending on whether a combiner network is used or not

        vector<Network*> networks;

        /**
         * i-th element corresponds to the vector of Subpop, where in the vector of SubPop,
         *  the i-th element corresponds to the i-th neuron in the corresponding network
         */
        vector<vector<SubPop*> > hidden_neuron_populations; // num networks x num hidden neurons per network

        uint generation;

        vector<Network*> overall_best_networks;

        uint numTrials;

        /**
         * Initializes the esp code
         */
        void init();

        /**
         * output a new network of the appropriate type
         * @param networkType
         * @param numHiddenNeurons
         * @param neuronGeneSize
         * @return
         */
        Network* generateNetwork(const uint& networkType, const uint& numHiddenNeurons,
                const uint& neuronGeneSize);
        uint getNeuronGeneSize();
        uint getCombinerNeuronGeneSize();

        /**
         * reset fitness vals
         */
        void evalReset();

        /**
         * evaluation stage.  Create numTrials networks, each containing
         * ZETA neurons selected randomly from the population.  Each network
         * is evaluated and each participating neuron receives the fitness
         * evaluations of each network it parcipates in.
         */
        void performEval();

        // NOTE Below are existing private member functions
        void recombine_hall_of_fame(Network* network);     //EVOLVE_PREY
        //Esp(const Esp &);     // just for safety
        EspExperiment &operator=(const EspExperiment &);
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
        static vector<vector<vector<Network*> > > hall_of_fame_pred; // EVOLVE_PREY vector of vector of best networks

    public:

        /**
         * Constructor
         * This sets all the esp-specific parameters read from the configuration file. Rest of the parameters are
         * set by the parent class
         */
        EspExperiment(const char* configFilePath);

        /**
         * Constructor
         * @param numAgents - total number of networks
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         * @param numInputs - Number of inputs for each network (not the combiner network, which is managed internally)
         * @param numOutputs - Number of outputs for each network (not the combiner network, which is managed internally)
         * @param neuronGeneSize
         */
        EspExperiment(const uint& numAgents, const uint& nHiddenNeurons, const uint& popSize,
                const uint& netTp, const uint& numOtherAgents, const uint& numActions,
                const Experiment& experiment);

        /**
         * Constructor
         * @param fileName
         * @param numNetworks - total number of networks
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         */
        EspExperiment(const char* fileName, const uint& numNetworks, const uint& nHiddenNeurons,
                const uint& popSize, const uint& netTp);

        EspExperiment(const EspExperiment& espExperiment);

        /**
         * Destructor
         */
        ~EspExperiment();

        // NOTE: The below functions are implementations of the Experiment interface
        /**
         * Starts running the experiment -- running the episode n number of times.
         */
        double start(const uint& maxSteps = -1, const uint& numGenerations = -1);

        /**
         * Runs the domain for one episode, and returns the fitness.
         */
        double run(); // return fitness
        double run(std::string stepsFilePath); // return fitness
    };
}

#endif /* ESP_H_ */
