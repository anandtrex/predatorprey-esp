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

namespace EspPredPreyHunter
{
    using std::vector;
    using std::ofstream;

    class Esp
    {
        uint numNetworks;
        uint numHiddenNeurons;

        uint neuronGeneSize;

        uint popSize;

        uint netTp;

        uint numInputs;
        uint numOutputs;

        vector<Network*> networks;

        /**
         * i-th element corresponds to the vector of Subpop, where in the vector of SubPop,
         *  the i-th element corresponds to the i-th neuron in the corresponding network
         */
        vector<vector<SubPop*> > pop;

    public:

        /**
         * Constructor
         * @param numNetworks - total number of networks
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         */
        /**
         * Constructor
         * @param numNetworks - total number of networks
         * @param nHiddenNeurons - number of hidden neurons for all the networks
         * @param popSize - Population size for each neuron in the networks
         * @param netTp - Type of the network. Only FF for now.
         * @param numInputs - Number of inputs for each network (not the combiner network, which is managed internally)
         * @param numOutputs - Number of outputs for each network (not the combiner network, which is managed internally)
         * @param neuronGeneSize
         */
        Esp(const uint& numNetworks, const uint& nHiddenNeurons, const uint& popSize,
                const uint& netTp, const uint& numInputs, const uint& numOutputs);

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

        /*
         // constructor for new Esp
         Esp(int nPreds, int nPreys, int num_teams_predator, int num_teams_prey,
         int num_teams_hunteres, int num_of_hunters, int nPops, int nSize, Environment &e,
         int netTp);

         // for two subtasks, it's two hall_of_fame_pred vectors
         Esp(vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred, int nPreds,
         int nPreys, int num_teams_predator, int num_teams_prey, int num_teams_hunteres,
         int num_of_hunters, int nPops, int nSize, Environment &e, int netTp);

         // constructor to load an old Esp from file
         Esp(char* fname, int nPreds, int nPreys, int num_teams_predator, int num_teams_prey,
         int num_teams_hunters, int num_of_hunters, int nPops, int nSize, Environment &e,
         int netTp);*/

        ~Esp();

        void create(int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey);     // creates a random population of neurons
        void evolve(int cycles, int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey, int num_teams_hunters, int num_of_hunters);
        void evalPop(int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey, int num_teams_hunters, int num_of_hunters);
        void findChampion();
        void loadSeedNet(char *);
        void addSubPop(int pred, int num_of_predators, int num_of_prey);

        void endEvolution();

        static vector<vector<vector<Network*> > > hall_of_fame_pred;     // EVOLVE_PREY vector of vector of best networks
        //static vector<vector<vector<Network*> > > hall_of_fame_prey;  // EVOLVE_PREY vector of vector of best networks

    private:
        uint neuronGeneSize();


        int total_predator_networks_per_team;
        //int total_prey_networks_per_team;
        vector<int> num_hidden_neurons;     // was called nPops

        vector<vector<vector<SubPop*> > > hidden_neuron_populations;     // num teams x num networks x num hidden neurons per network
        //vector<vector<vector<subPop*> > > hidden_neuron_populations_prey;

        // sub networks from previous sub-tasks
        vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred;

        int numTrials;
        int netType;
        //deque<double> perfQ;
        //Environment &Envt;
        vector<vector<Network*> > overall_best_networks;
        vector<double> overall_best_teams_sum_individual_fitness;     //Sum of individual network fitnesses
        //vector<vector<Network*> > overall_best_teams_prey;
        //vector<double> overall_best_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses
        vector<vector<Network*> > bestNetwork;
        //vector<vector<Network*> > bestNetwork_prey;
        int generation;

        void recombine_hall_of_fame(Network* network);     //EVOLVE_PREY

        ofstream fout_champfitness;

        Esp(const Esp &);     // just for safety
        Esp &operator=(const Esp &);

        void save(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey);
        SubPop* load(char *fname);
        void setupNetDimensions(int num_of_predators, int num_of_prey);
        Network* generateNetwork(const uint& networkType, const uint& numHiddenNeurons, const uint& neuronGeneSize);
        void evalReset(int num_teams_predator, int num_teams_prey);
        void performEval(int num_of_predators, int num_of_prey, int num_teams_predator,
                int num_teams_prey, int num_teams_hunters, int num_of_hunters);
        void average(int num_teams_predator, int num_teams_prey);
        void printNeurons();
        void handleStagnation(int num_of_predators, int num_of_prey);
        void newDeltaPhase(int pred);
        int removeSubPop(int sp, int pred, int num_of_predators, int num_of_prey);
        int removeSubPop(vector<Network*>& team, int pred, int num_of_predators, int num_of_prey);
        void addConnections();
        void removeConnections(int sp);
        void printStats();
    };
}

#endif /* ESP_H_ */
