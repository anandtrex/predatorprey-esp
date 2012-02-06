/*
 * Esp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "Esp.h"
#include "common.h"
#include "Network.h"

#define FF 0
#define SRN 1
#define SCDORDER 2
#define FR 3

//////////////////////////////////////////////////////////////////////
//
// Esp
//
//////////////////////////////////////////////////////////////////////

// Assume combiner network used.
// Assume number of hidden neurons is the same for all networks

namespace EspPredPreyHunter
{
    using std::vector;
    using std::endl;

    Esp::Esp(const uint& numNetworks, const uint& nHiddenNeurons, const uint& popSize,
            const uint& netTp, const uint& numInputs, const uint& numOutputs)
            : numNetworks(numNetworks), numHiddenNeurons(nHiddenNeurons), popSize(popSize), netTp(
                    netTp), numInputs(numInputs), numOutputs(numOutputs)
    {
        generation = 0;

        vector<Network*> temp_overall_best_networks;
        for (uint i = 0; i < numNetworks; i++) {
            temp_overall_best_networks.push_back(generateNetwork(netType, numHiddenNeurons, neuronGeneSize)); // generate new network
            temp_overall_best_networks[i]->create(); // create the network
        }

        overall_best_networks.push_back(temp_overall_best_networks);

        neuronGeneSize = getNeuronGeneSize();
    }

    uint Esp::neuronGeneSize()
    {
        // Assume network type is FF
    }

// Esp constructor -- constructs a new ESP
    /*
     Esp::Esp(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey,
     int num_teams_hunters, int num_of_hunters, int nPops /* num hidden neurons --/,
     int nSize, Environment &e, int netTp = FF) :
     Envt(e)
     {
     hall_of_fame_pred = vector<vector<vector<Network*> > >();
     fout_champfitness.open("champion_fitness.log");
     if (!fout_champfitness.is_open()) {
     LOG(FATAL) << "File not opened";
     } else {
     DLOG(INFO) << "Opened file";
     }

     this->sub_hall_of_fame_pred = vector<vector<vector<vector<Network*> > > >();

     generation = 0;  // start at generation 0

     total_predator_networks_per_team = num_of_predators * (num_of_prey * num_teams_prey /* total no. of prey --/
     + num_of_hunters * num_teams_hunters /* total num of hunters --/
     + num_of_predators * (num_teams_predator - 1) /* total number of predators in the other teams --/
     ) + COMBINE * num_of_predators;  //Total Predator Networks in a team

     LOG(INFO) << "total_predator_networks_per_team is " << total_predator_networks_per_team;

     //    total_prey_networks_per_team = num_of_prey * (num_of_predators * num_teams_predator)
     //            + COMBINE * num_of_prey;  //Total Prey Networks in a team

     if (pred_communication == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1); //Extra NNs for predator direct communication
     }

     //    if (prey_communication == true) {
     //        total_prey_networks_per_team = total_prey_networks_per_team
     //                + num_of_prey * (num_of_prey - 1);  //Extra NNs for prey direct communication
     //    }

     if (pred_messaging == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator messaging
     }

     //    if (prey_messaging == true) {
     //        total_prey_networks_per_team = total_prey_networks_per_team
     //                + num_of_prey * (num_of_prey - 1);  //Extra NNs for prey direct communication
     //    }

     for (int i = 0; i < total_predator_networks_per_team; i++) {
     num_hidden_neurons.push_back(nPops);  // set the number of subpops
     }

     numTrials = nSize * 10;  // # of trials ~10/neuron
     TOTAL_EVALUATIONS = numTrials * EVALTRIALS;  // 6000 for now
     netType = netTp;  // set the network type (FF,SRN,2ndOrder)
     NUM_INPUTS_PRED_COMBINER = Envt.inputSize_pred_combiner;  // network in/out dimensions
     //    NUM_INPUTS_PREY_COMBINER = Envt.inputSize_prey_combiner;  // network in/out dimensions
     NUM_OUTPUT_PRED_COMBINER = Envt.outputSize_pred_combiner;
     //    NUM_OUTPUT_PREY_COMBINER = Envt.outputSize_prey_combiner;
     NUM_INPUTS = Envt.inputSize;  // network in/out dimensions
     //    NUM_INPUTS_PREY = Envt.inputSize_prey;  // network in/out dimensions //EVOLVE_PREY
     NUM_OUTPUTS = Envt.outputSize;  //    specified in the Environment
     //    NUM_OUTPUTS_PREY = Envt.outputSize_prey;  //    specified in the Environment
     setupNetDimensions(num_of_predators, num_of_prey); // calculate the input layer size and gene size

     vector<Network*> temp_overall_best_teams;
     //    vector<Network*> temp_overall_best_teams_prey;

     for (int j = 0; j < num_teams_predator; j++) {
     IS_COMBINER_NW = 0;
     for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
     i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     IS_COMBINER_NW = 1;
     for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
     i < total_predator_networks_per_team; i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     overall_best_teams_sum_individual_fitness.push_back(0.0);
     overall_best_teams.push_back(temp_overall_best_teams);
     temp_overall_best_teams.clear();
     }

     //    for (int j = 0; j < num_teams_prey; j++) {
     //        IS_COMBINER_NW = 0;
     //        for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++) {
     //            temp_overall_best_teams_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));  // generate new net
     //            temp_overall_best_teams_prey[i]->create();  // create net
     //        }
     //        IS_COMBINER_NW = 1;
     //        for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
     //                i < total_prey_networks_per_team; i++) {
     //            temp_overall_best_teams_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));  // generate new net
     //            temp_overall_best_teams_prey[i]->create();  // create net
     //        }
     //        overall_best_teams_prey_sum_individual_fitness.push_back(0.0);
     //        overall_best_teams_prey.push_back(temp_overall_best_teams_prey);
     //        temp_overall_best_teams_prey.clear();
     //    }

     vector<subPop*>* temp;
     vector<vector<subPop*> > team_hidden_neuron_populations;  // num_predators x num_teams

     // construct total_predator_networks_per_team number of subpop vectors\
    // for every team, for each network, for
     for (int k = 0; k < num_teams_predator; k++) {
     team_hidden_neuron_populations.clear();
     for (int i = 0; i < total_predator_networks_per_team; i++) {
     temp = new vector<subPop*>;
     for (int j = 0; j < num_hidden_neurons[i]; j++) {
     temp->push_back(new subPop(nSize));  //nSize = 100
     }
     team_hidden_neuron_populations.push_back(*temp);
     }
     hidden_neuron_populations.push_back(team_hidden_neuron_populations);
     }

     // construct total_predator_networks_per_team number of subpop vectors
     //    for (int k = 0; k < num_teams_prey; k++) {
     //        team_hidden_neuron_populations.clear();
     //        for (int i = 0; i < total_prey_networks_per_team; i++) {
     //            temp = new vector<subPop*>;
     //            for (int j = 0; j < num_hidden_neurons[0]; j++) {
     //                temp->push_back(new subPop(nSize));  //Creating a (pointer to the) population of 100 neurons for each hidden neuron
     //            }
     //            team_hidden_neuron_populations.push_back(*temp);  //Should we clear/delete temp or not?. What is this line doing?
     //        }
     //        hidden_neuron_populations_prey.push_back(team_hidden_neuron_populations);
     //    }

     create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey); // create the neurons.
     LOG(INFO) << " ESP Constructor Done " << endl;

     }*/

// Esp constructor -- constructs a new ESP for a combination of subtasks
    /*
     Esp::Esp(vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred,
     int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey,
     int num_teams_hunters, int num_of_hunters, int nPops /* num hidden neurons --/,
     int nSize, Environment &e, int netTp = FF) :
     Envt(e)
     {
     hall_of_fame_pred = vector<vector<vector<Network*> > >();
     this->sub_hall_of_fame_pred = sub_hall_of_fame_pred;

     fout_champfitness.open("champion_fitness.log");
     if (!fout_champfitness.is_open()) {
     LOG(FATAL) << "File not opened";
     } else {
     DLOG(INFO) << "Opened file";
     }

     generation = 0;  // start at generation 0

     total_predator_networks_per_team = num_of_predators * (num_of_prey * num_teams_prey /* total no. of prey --/
     + num_of_hunters * num_teams_hunters /* total num of hunters --/
     + num_of_predators * (num_teams_predator - 1) /* total number of predators in the other teams --/
     ) + COMBINE * num_of_predators;  //Total Predator Networks in a team

     //    total_prey_networks_per_team = num_of_prey * (num_of_predators * num_teams_predator)
     //            + COMBINE * num_of_prey;  //Total Prey Networks in a team

     if (pred_communication == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1); //Extra NNs for predator direct communication
     }

     //    if (prey_communication == true) {
     //        total_prey_networks_per_team = total_prey_networks_per_team
     //                + num_of_prey * (num_of_prey - 1);  //Extra NNs for prey direct communication
     //    }

     if (pred_messaging == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator messaging
     }

     //    if (prey_messaging == true) {
     //        total_prey_networks_per_team = total_prey_networks_per_team
     //                + num_of_prey * (num_of_prey - 1);  //Extra NNs for prey direct communication
     //    }

     for (int i = 0; i < total_predator_networks_per_team; i++) {
     num_hidden_neurons.push_back(nPops);  // set the number of subpops
     }

     numTrials = nSize * 10;  // # of trials ~10/neuron
     TOTAL_EVALUATIONS = numTrials * EVALTRIALS;  // 6000 for now
     netType = netTp;  // set the network type (FF,SRN,2ndOrder)
     NUM_INPUTS_PRED_COMBINER = Envt.inputSize_pred_combiner;  // network in/out dimensions
     //    NUM_INPUTS_PREY_COMBINER = Envt.inputSize_prey_combiner;  // network in/out dimensions
     NUM_OUTPUT_PRED_COMBINER = Envt.outputSize_pred_combiner;
     //    NUM_OUTPUT_PREY_COMBINER = Envt.outputSize_prey_combiner;
     NUM_INPUTS = Envt.inputSize;  // network in/out dimensions
     //    NUM_INPUTS_PREY = Envt.inputSize_prey;  // network in/out dimensions //EVOLVE_PREY
     NUM_OUTPUTS = Envt.outputSize;  //    specified in the Environment
     //    NUM_OUTPUTS_PREY = Envt.outputSize_prey;  //    specified in the Environment
     setupNetDimensions(num_of_predators, num_of_prey); // calculate the input layer size and gene size

     vector<Network*> temp_overall_best_teams;
     vector<Network*> temp_overall_best_teams_prey;

     for (int j = 0; j < num_teams_predator; j++) {
     IS_COMBINER_NW = 0;
     for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
     i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     IS_COMBINER_NW = 1;
     for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
     i < total_predator_networks_per_team; i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     overall_best_teams_sum_individual_fitness.push_back(0.0);
     overall_best_teams.push_back(temp_overall_best_teams);
     temp_overall_best_teams.clear();
     }

     //    for (int j = 0; j < num_teams_prey; j++) {
     //        IS_COMBINER_NW = 0;
     //        for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++) {
     //            temp_overall_best_teams_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));  // generate new net
     //            temp_overall_best_teams_prey[i]->create();  // create net
     //        }
     //        IS_COMBINER_NW = 1;
     //        for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
     //                i < total_prey_networks_per_team; i++) {
     //            temp_overall_best_teams_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));  // generate new net
     //            temp_overall_best_teams_prey[i]->create();  // create net
     //        }
     //        overall_best_teams_prey_sum_individual_fitness.push_back(0.0);
     //        overall_best_teams_prey.push_back(temp_overall_best_teams_prey);
     //        temp_overall_best_teams_prey.clear();
     //    }

     vector<subPop*>* temp;
     vector<vector<subPop*> > team_hidden_neuron_populations;  // num_predators x num_teams

     // construct total_predator_networks_per_team number of subpop vectors\
    // for every team, for each network, for
     for (int k = 0; k < num_teams_predator; k++) {
     team_hidden_neuron_populations.clear();
     for (int i = 0; i < total_predator_networks_per_team; i++) {
     temp = new vector<subPop*>;
     for (int j = 0; j < num_hidden_neurons[i]; j++) {
     temp->push_back(new subPop(nSize));  //nSize = 100
     }
     team_hidden_neuron_populations.push_back(*temp);
     }
     hidden_neuron_populations.push_back(team_hidden_neuron_populations);
     }

     // construct total_predator_networks_per_team number of subpop vectors
     //    for (int k = 0; k < num_teams_prey; k++) {
     //        team_hidden_neuron_populations.clear();
     //        for (int i = 0; i < total_prey_networks_per_team; i++) {
     //            temp = new vector<subPop*>;
     //            for (int j = 0; j < num_hidden_neurons[0]; j++) {
     //                temp->push_back(new subPop(nSize));  //Creating a (pointer to the) population of 100 neurons for each hidden neuron
     //            }
     //            team_hidden_neuron_populations.push_back(*temp);  //Should we clear/delete temp or not?. What is this line doing?
     //        }
     //        hidden_neuron_populations_prey.push_back(team_hidden_neuron_populations);
     //    }

     create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey); // create the neurons.
     LOG(INFO) << " ESP Constructor Done " << endl;

     }*/

// ----------------------------------------------------------------------
//New Function for Loading weights from file
    /*
     Esp::Esp(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
     int num_teams_prey, int num_teams_hunters, int num_of_hunters, int nPops, int nSize,
     Environment &e, int netTp = FF) :
     Envt(e)

     {

     hall_of_fame_pred = vector<vector<vector<Network*> > >();

     fout_champfitness.open("champion_fitness.log");
     if (!fout_champfitness.is_open()) {
     LOG(FATAL) << "File not opened";
     } else {
     DLOG(INFO) << "Opened file";
     }

     this->sub_hall_of_fame_pred = vector<vector<vector<vector<Network*> > > >();

     vector<subPop*>* tempVec;
     double tempDouble;
     FILE *fptr;

     if ((fptr = fopen(fname, "r")) == NULL) {
     LOG(FATAL) << endl << "Error - cannot open " << fname << endl;
     //exit(1);
     }

     fread(&generation, 4, 1, fptr);  // generation

     total_predator_networks_per_team = num_of_predators
     * (num_of_prey * num_teams_prey + num_teams_hunters * num_of_hunters
     + num_of_predators * (num_teams_predator - 1)) + COMBINE * num_of_predators; //Total Predator Networks in a team

     if (pred_communication == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1); //Extra NNs for predator direct communication
     }

     if (pred_messaging == true) {
     total_predator_networks_per_team = total_predator_networks_per_team
     + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator messaging
     }

     for (int i = 0; i < total_predator_networks_per_team; i++)
     num_hidden_neurons.push_back(nPops);  // set the number of subpops

     numTrials = nSize * 10;  // # of trials ~10/neuron
     TOTAL_EVALUATIONS = numTrials * EVALTRIALS;  // 6000 for now
     netType = netTp;  // set the network type (FF,SRN,2ndOrder)
     NUM_INPUTS_PRED_COMBINER = Envt.inputSize_pred_combiner;  // network in/out dimensions
     NUM_OUTPUT_PRED_COMBINER = Envt.outputSize_pred_combiner;
     NUM_INPUTS = Envt.inputSize;  // network in/out dimensions
     NUM_OUTPUTS = Envt.outputSize;  //    specified in the Environment
     setupNetDimensions(num_of_predators, num_of_prey); // calculate the input layer size and gene size

     vector<Network*> temp_overall_best_teams;
     vector<Network*> temp_overall_best_teams_prey;

     for (int j = 0; j < num_teams_predator; j++) {
     IS_COMBINER_NW = 0;
     for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
     i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     IS_COMBINER_NW = 1;
     for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
     i < total_predator_networks_per_team; i++) {
     temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i])); // generate new net
     temp_overall_best_teams[i]->create();  // create net
     }
     overall_best_teams_sum_individual_fitness.push_back(0.0);
     overall_best_teams.push_back(temp_overall_best_teams);
     temp_overall_best_teams.clear();
     }

     vector<subPop*>* temp;
     vector<vector<subPop*> > total_hidden_neuron_populations_per_team;

     // construct total_predator_networks_per_team number of subpop vectors
     for (int k = 0; k < num_teams_predator; k++) {
     total_hidden_neuron_populations_per_team.clear();
     for (int i = 0; i < total_predator_networks_per_team; i++) {
     temp = new vector<subPop*>;
     for (int j = 0; j < num_hidden_neurons[i]; j++) {
     temp->push_back(new subPop(nSize));
     }
     total_hidden_neuron_populations_per_team.push_back(*temp);
     }
     hidden_neuron_populations.push_back(total_hidden_neuron_populations_per_team);
     }

     create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey); // create the neurons.

     // read neurons' weights from file, and set them
     for (int p = 0; p < num_teams_predator; p++)
     for (int i = 0; i < total_predator_networks_per_team - (COMBINE * num_of_predators);
     i++)
     for (int j = 0; j < num_hidden_neurons[i]; j++)
     for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
     for (int m = 0; m < GENE_SIZE_PRED_NW; m++) {
     fread(&tempDouble, sizeof(double), 1, fptr);
     hidden_neuron_populations[p][i][j]->pop[k]->weight[m] = tempDouble; // weights
     }
     //if (COMBINE == 1) {
     for (int p = 0; p < num_teams_predator; p++)
     for (int i = total_predator_networks_per_team - (COMBINE * num_of_predators);
     i < total_predator_networks_per_team; i++)
     for (int j = 0; j < num_hidden_neurons[i]; j++)
     for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
     for (int m = 0; m < GENE_SIZE_PRED_COMBINER_NW; m++) {
     fread(&tempDouble, sizeof(double), 1, fptr);
     hidden_neuron_populations[p][i][j]->pop[k]->weight[m] = tempDouble; // weights
     }

     fclose(fptr);

     LOG(INFO) << "Loading from file : each predator is a separate neural net" << endl
     << "Total Number of Predator networks: " << total_predator_networks_per_team << endl
     //            << "Total Number of Prey Networks:" << total_prey_networks_per_team << endl
     << "Initial total subpops: " << total_predator_networks_per_team * nPops << endl
     << "Incremental learning: " << INCREMENTAL_LEARNING << endl; << "Burst mutation: "
     << BURST_MUTATE << endl << "EvalTrials: " << EVALTRIALS << endl << "STAG: " << STAG
     << endl;

     }*/

// destructor
    Esp::~Esp()
    {
        //Freeing up memory pointed to by subpop pointers (corresponding to the new statements above)

        int size = hidden_neuron_populations.size();     //This gives num_teams_predator value

        for (int k = 0; k < size; k++) {
            for (int i = 0; i < total_predator_networks_per_team; i++) {
                for (int j = 0; j < num_hidden_neurons[i]; j++) {
                    delete hidden_neuron_populations[k][i][j];
                }
            }
        }
        //Delete the pointers
        hidden_neuron_populations.clear();

        //Freeing up memory pointed to by subpop pointers (corresponding to the new statements above)
//    size = hidden_neuron_populations_prey.size();  //This gives num_teams_prey value
//
//    for (int k = 0; k < size; k++) {
//        for (int i = 0; i < total_prey_networks_per_team; i++) {
//            for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                delete hidden_neuron_populations_prey[k][i][j];
//            }
//        }
//    }
//    //Delete the pointers
//    hidden_neuron_populations_prey.clear();
        fout_champfitness.close();

    }

//----------------------------------------------------------------------
// calculate the input layer size and size of chromosomes (GENE_SIZE_PRED_NW)
//   based on type of network and in/out dimensions, and # of
//   hidden units (i.e. # of subpops)
    void Esp::setupNetDimensions(int num_of_predators, int num_of_prey)
    {
        switch (netType) {
            case FF:
                GENE_SIZE_PRED_NW = (NUM_INPUTS * num_of_predators)
                        / (total_predator_networks_per_team - (COMBINE * num_of_predators))
                        + NUM_OUTPUTS;     // Gene Size per hidden neuron (excepting combiner)
//        if(((total_prey_networks_per_team - (COMBINE * num_of_prey)) + NUM_OUTPUTS_PREY) == 0){
//            GENE_SIZE_PREY_NW = 0;
//        }
//        else {
//        GENE_SIZE_PREY_NW = (NUM_INPUTS_PREY * num_of_prey)
//                / (total_prey_networks_per_team - (COMBINE * num_of_prey)) + NUM_OUTPUTS_PREY;
//        }
                GENE_SIZE_PRED_COMBINER_NW = (NUM_INPUTS_PRED_COMBINER + NUM_OUTPUT_PRED_COMBINER);     // Gene Size per hidden neuron of a combiner network
//        GENE_SIZE_PREY_COMBINER_NW = (NUM_INPUTS_PREY_COMBINER + NUM_OUTPUT_PREY_COMBINER);

                LOG(INFO) << "Gene size per hidden neuron of non-combiner networks: "
                        << " predator:: " << GENE_SIZE_PRED_NW;     //<< "  prey:: " << GENE_SIZE_PREY_NW << endl;
                LOG(INFO) << "Gene size per hidden neuron of combiner networks: "
                        << " predator:: " << GENE_SIZE_PRED_COMBINER_NW;     // << "  prey:: " << GENE_SIZE_PREY_COMBINER_NW << endl;
//        LOG(INFO) << "Total networks in a team  " << " predators:: " << total_predator_networks_per_team
//                << "  prey:: " << total_prey_networks_per_team << endl;
                LOG(INFO) << "FEED FORWARD\n";
                break;

        }
    }

//----------------------------------------------------------------------
// create the subpopulations of neurons, initializing them to
//   random weights
    void Esp::create(int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey)
    {
        IS_COMBINER_NW = 0;
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++)
                for (int j = 0; j < num_hidden_neurons[i]; j++) {
                    hidden_neuron_populations[k][i][j]->create();     // create each subpop
                }
        }

        IS_COMBINER_NW = 1;
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                    i < total_predator_networks_per_team; i++)
                for (int j = 0; j < num_hidden_neurons[i]; j++) {
                    hidden_neuron_populations[k][i][j]->create();     // create each subpop
                }
        }
    }

//////////////////////////////////////////////////////////////////////
//
//  evolve is the main genetic function.  The subpopulations are first
//  evaluated in the given task.  Then for each subpop the neurons are
//  ranked (sorted by fitness) and recombined using crossover.
//  Mating is allowed only between the top 'numBreed' members of
//  the same subpop. The neurons are then mutated.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
//string PLOT_STR("plot ");

    void Esp::evolve(int cycles, int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey, int num_teams_hunters, int num_of_hunters)
// cycles: the number of generations to evolve
    {

        LOG(INFO) << "Starting evolve and running " << cycles << " cycles" << endl;
        while (gInterrupt == false && ++generation <= cycles) {
            evalPop(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                    num_teams_hunters, num_of_hunters);     //evaluate neurons

            IS_COMBINER_NW = 0;
            for (int k = 0; k < num_teams_predator; k++) {
                for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
                        i++) {
                    for (int j = 0; j < num_hidden_neurons[i]; j++) {
                        hidden_neuron_populations[k][i][j]->qsortNeurons();
                        hidden_neuron_populations[k][i][j]->recombine_hall_of_fame(k, i, j);     //(Hall of fame probably doesn't work for Competing agents in a single team)
                    }
                }
            }

            // mutate population
            for (int k = 0; k < num_teams_predator; k++) {
                for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
                        i++) {
                    for (int j = 0; j < num_hidden_neurons[i]; j++) {
                        hidden_neuron_populations[k][i][j]->mutate();
                    }
                }
            }

            IS_COMBINER_NW = 1;
            for (int k = 0; k < num_teams_predator; k++) {
                for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                        i < total_predator_networks_per_team; i++) {
                    for (int j = 0; j < num_hidden_neurons[i]; j++) {
                        hidden_neuron_populations[k][i][j]->qsortNeurons();
                        hidden_neuron_populations[k][i][j]->recombine_hall_of_fame(k, i, j);     //(Hall of fame probably doesn't work for Competing agents in a single team)
                    }
                }
            }

            // mutate population
            for (int k = 0; k < num_teams_predator; k++) {
                for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                        i < total_predator_networks_per_team; i++) {
                    for (int j = 0; j < num_hidden_neurons[i]; j++) {
                        hidden_neuron_populations[k][i][j]->mutate();
                    }
                }
            }
        }     //end while

        endEvolution();

        LOG(INFO) << "Ending evolve" << endl;

    }

//////////////////////////////////////////////////////////////////////
//
//  eval_pop evaluates entire population to get the average fitness of
//  each neuron.  This function is called by evolve and should put the
//  fitness of each neuron in the neuron's fitness field
//  (pop[i]->fitness).
//
//////////////////////////////////////////////////////////////////////

    void Esp::evalPop(int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey, int num_teams_hunters, int num_of_hunters)
    {

        evalReset(num_teams_predator, num_teams_prey);     // reset fitness and test values of each neuron
        performEval(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                num_teams_hunters, num_of_hunters);     // evaluate the networks
        average(num_teams_predator, num_teams_prey);     // normalize neuron fitnesses

    }

//----------------------------------------------------------------------
// reset fitness vals
    void Esp::evalReset(int num_teams_predator, int num_teams_prey)
    {
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < total_predator_networks_per_team; i++) {
                for (int j = 0; j < num_hidden_neurons[i]; j++)
                    hidden_neuron_populations[k][i][j]->evalReset();
            }
        }
    }

//////////////////////////////////////////////////////////////////////
//
//  evaluation stage.  Create numTrials networks, each containing
//  ZETA neurons selected randomly from the population.  Each network
//  is evaluated and each participating neuron receives the fitness
//  evaluations of each network it parcipates in.
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// output a new network of the appropriate type
    Network* Esp::generateNetwork(const uint& networkType, const uint& numHiddenNeurons, const uint& neuronGeneSize)
    {
        switch (networkType) {
            case FF:
                return (new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize));
                //break;
                /*  case FR :
                 return( new FullyRecurrentNetwork(nPops) ); break;
                 case SRN :
                 return( new SimpleRecurrentNetwork(nPops) ); break;
                 case SCDORDER :
                 return( new SecondOrderRecurrentNetwork(nPops) );
                 */

            default:
                return (new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize));
                //break;
        }
    }

//--------------------------------------------------------------------
// evaluate the networks on the task
// TODO Add appropriate addition of sub-task networks
    void Esp::performEval(int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey, int num_teams_hunters, int num_of_hunters)
    {
        //LOG(INFO) << " Starting Perform Eval " <<endl;

        static int evaluations = 0;
        vector<Network*> temp_team;     // vector of current networks
        vector<Network*> temp_bestTeam;     // vector of best networks

        vector<vector<Network*> > current_teams_pred;     // vector of current pred teams
        vector<vector<Network*> > generation_best_teams;     // vector of best pred teams in a given generation

        vector<double> temp_individual_fitness;     //Temporary variable for storing predator/prey team fitness
        vector<vector<double> > temp_team_fitness;     //Temporary variable for storing predator/prey team fitness
        vector<vector<vector<double> > > predator_prey_teams_fitness;     //Vector for all teams fitness together after averaging over 6 trials
        vector<vector<vector<double> > > average_predator_prey_teams_fitness;     //Vector for all teams fitness together after averaging over 6000 trials
        vector<vector<vector<double> > > teams_fitness;     //temporary variable for all teams fitness together per trial

        vector<double> current_teams_sum_individual_fitness;     //Sum of individual network fitnesses
        vector<double> generation_best_teams_sum_individual_fitness;     //Sum of individual network fitnesses
//    vector<double> current_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses
//    vector<double> generation_best_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses

        delay_pred_evolution = 1;

        for (int k = 0; k < num_teams_predator; k++) {
            IS_COMBINER_NW = 0;
            temp_team.clear();
            temp_bestTeam.clear();
            for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
                    i++) {
                temp_team.push_back(generateNetwork(netType, num_hidden_neurons[i]));
                temp_bestTeam.push_back(generateNetwork(netType, num_hidden_neurons[i]));
                temp_bestTeam[i]->create();
            }

            IS_COMBINER_NW = 1;
            for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                    i < total_predator_networks_per_team; i++) {
                temp_team.push_back(generateNetwork(netType, num_hidden_neurons[i]));
                temp_bestTeam.push_back(generateNetwork(netType, num_hidden_neurons[i]));
                temp_bestTeam[i]->create();
            }
            current_teams_pred.push_back(temp_team);
            generation_best_teams.push_back(temp_bestTeam);
        }

        IS_COMBINER_NW = 0;

        //***************START Initialize average fitness (calculated across 1000 trials) to zero********************//
        for (int p = 0; p < num_teams_predator; p++) {
            for (int q = 0; q < num_of_predators; q++) {
                temp_individual_fitness.push_back(0.0);
            }
            temp_team_fitness.push_back(temp_individual_fitness);
            temp_individual_fitness.clear();
        }

        average_predator_prey_teams_fitness.push_back(temp_team_fitness);
        temp_team_fitness.clear();
//    for (int p = 0; p < num_teams_prey; p++) {
//        for (int q = 0; q < num_of_prey; q++) {
//            temp_individual_fitness.push_back(0.0);
//        }
//        temp_team_fitness.push_back(temp_individual_fitness);
//        temp_individual_fitness.clear();
//    }
//    average_predator_prey_teams_fitness.push_back(temp_team_fitness);
        //***************END Initialize average fitness (calculated across 1000 trials) to zero********************//

        //**************START OF TRIALS********************************************//
        for (int i = 0; i < numTrials; ++i) {

            //LOG(INFO) << " PerformEval:: Starting with numTrials" << i << " Pred " << total_hidden_neuron_populations.size()
            //     << "    " << total_hidden_neuron_populations[0].size()
            //     << "    " << total_hidden_neuron_populations[0][0].size()
            //     << "    Prey " << total_hidden_neuron_populations_prey.size()
            //     << endl ;
            // find random subpopulation (only for current team)

            if ((int) sub_hall_of_fame_pred.size() == 2) {
                // LOG(INFO) << "Using previous networks";
                // If there are pre-existing neurons, set neurons only for the combiner network. Get other neurons from networks learnt in the sub task
                // For the subnetwork, there will be a combiner network. So two hidden layers
                // FIXME This will only work when the no. of prey and hunter is 1 !!
                int sub_hall_of_fame_pred_size = sub_hall_of_fame_pred[0].size();
                for (int j = 0; j < num_teams_predator; j++) {
                    int i;
                    int sub_total_predator_networks_per_team =
                            sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j].size();
                    // Takes care of prey sub networks
                    for (i = 0; i < sub_total_predator_networks_per_team;     //num_of_predators * num_teams_prey * num_of_prey /*actually, number of prey networks. Should be num_teams_prey * num_of_prey*/;
                            i++) {
                        if (i < num_of_predators * num_teams_prey * num_of_prey) {
                            current_teams_pred[j][i]->setNetwork(
                                    sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j][i]);
                            current_teams_pred[j][i]->incrementTests();
                        } else {
                            for (int k = 0; k < num_of_predators * num_teams_prey * num_of_prey;
                                    k++) {
                                current_teams_pred[j][k]->addHiddenLayer(
                                        sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j][i]->pop);
                            }
                        }
                    }
                    // Takes care of hunter sub networks
                    int k;
                    i = num_of_predators * num_teams_prey * num_of_prey;
                    sub_total_predator_networks_per_team =
                            sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j].size();
                    for (k = i; k < i + sub_total_predator_networks_per_team;     //num_of_predators * num_teams_hunters * num_of_hunters /*actually, number of hunter networks. Should be num_teams_hunter * num_of_hunter*/;
                            k++) {
                        if (k < i + num_of_predators * num_teams_hunters * num_of_hunters) {
                            current_teams_pred[j][k]->setNetwork(
                                    sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j][k
                                            - i]);
                            current_teams_pred[j][k]->incrementTests();
                        } else {
                            for (int l = 0;
                                    l < num_of_predators * num_teams_hunters * num_of_hunters;
                                    l++) {
                                current_teams_pred[j][l]->addHiddenLayer(
                                        sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j][k
                                                - i]->pop);
                            }
                        }
                    }
                    i = num_of_predators * num_teams_prey * num_of_prey
                            + num_of_predators * num_teams_hunters * num_of_hunters;

                    // Takes care of the actual combiner layer (which is evolved)
                    for (int l = i; l < i + COMBINE * num_of_predators; l++) {
                        for (int m = 0; m < num_hidden_neurons[l]; m++) {
                            current_teams_pred[j][l]->setNeuron(
                                    hidden_neuron_populations[j][l][m]->selectNeuron(), m);
                        }
                        current_teams_pred[j][l]->incrementTests();
                    }
                }

            } else {     // If there are no existing subnetworks already evolved, set neurons for all the hidden layers (including combiner network)

                for (int p = 0; p < num_teams_predator; p++) {
                    for (int j = 0; j < total_predator_networks_per_team; j++) {
                        for (int k = 0; k < num_hidden_neurons[j]; k++) {
                            current_teams_pred[p][j]->setNeuron(
                                    hidden_neuron_populations[p][j][k]->selectNeuron(), k);
                        }
                        current_teams_pred[p][j]->incrementTests();     //Counting the number of trials for a neuron (to compute average later)
                    }
                }
            }

            ++evaluations;

            //************INITIALIZING predator_prey_teams_fitness ********************
            predator_prey_teams_fitness.clear();
            temp_individual_fitness.clear();
            temp_team_fitness.clear();
            current_teams_sum_individual_fitness.clear();
            generation_best_teams_sum_individual_fitness.clear();

            for (int p = 0; p < num_teams_predator; p++) {
                for (int q = 0; q < num_of_predators; q++) {
                    temp_individual_fitness.push_back(0.0);
                }
                temp_team_fitness.push_back(temp_individual_fitness);
                temp_individual_fitness.clear();
                current_teams_sum_individual_fitness.push_back(0.0);
                generation_best_teams_sum_individual_fitness.push_back(0.0);
            }

            predator_prey_teams_fitness.push_back(temp_team_fitness);
            temp_team_fitness.clear();

            //************END INITIALIZING predator_prey_teams_fitness ********************

            //Evalnet returns predator, prey teams fitness in a vector of vectors.
            //Vector 0 consists of fitness for Predator teams and Vector 1 consists
            //of fitness for Prey teams
            for (int p = 0; p < EVALTRIALS; p++) {
                teams_fitness = Envt.evalNet(current_teams_pred, generation);
                /*VLOG(1) << "Team fitness is " << endl;
                 for (int i = 0; i < teams_fitness.size(); i++) {
                 for (int j = 0; j < teams_fitness[i].size(); j++) {
                 LOG(INFO) << teams_fitness[i][j][0] << " ";
                 }
                 VLOG(1) << endl;
                 }*/
//            LOG(INFO) << "Size of fitness from environment is " << teams_fitness.size();
//            LOG(INFO) << "teams_fitness[0][0][0] from environment is " << teams_fitness[0][0][0];
//            LOG(INFO) << "Size of predator_prey_teams_fitness is " << predator_prey_teams_fitness.size();
//            LOG(INFO) << "predator_prey_teams_fitness[0][0][0] is " << predator_prey_teams_fitness[0][0][0];
                for (int q = 0; q < num_teams_predator; q++) {
                    for (int r = 0; r < num_of_predators; r++) {     //Individual predator fitness is used in case of competing predators
                        predator_prey_teams_fitness[0][q][r] = predator_prey_teams_fitness[0][q][r]
                                + teams_fitness[0][q][r];
                    }
                }
//            for (int q = 0; q < num_teams_prey; q++) {
//                for (int r = 0; r < num_of_prey; r++) {
//                    predator_prey_teams_fitness[1][q][r] = predator_prey_teams_fitness[1][q][r]
//                            + teams_fitness[1][q][r];
//                }
//            }
            }

            //Predator - Distribute fitness to each contributing neurons
            for (int q = 0; q < num_teams_predator; q++) {
                for (int r = 0; r < num_of_predators; r++) {
                    predator_prey_teams_fitness[0][q][r] /= EVALTRIALS;
                    average_predator_prey_teams_fitness[0][q][r] +=
                            predator_prey_teams_fitness[0][q][r];

                    for (int j = 0; j < (total_predator_networks_per_team / num_of_predators - 1);
                            j++) {
                        current_teams_pred[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1) + j]->fitness =
                                predator_prey_teams_fitness[0][q][r];     // distribute each predator's fitness to constituent networks
                        current_teams_pred[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1) + j]->addFitness();     // add network fitness to its neurons
                    }

                    //Following two lines are for the combiner network
                    current_teams_pred[q][(total_predator_networks_per_team - num_of_predators) + r]->fitness =
                            predator_prey_teams_fitness[0][q][r];     // distribute each predator's fitness to constituent networks
                    current_teams_pred[q][(total_predator_networks_per_team - num_of_predators) + r]->addFitness();     // add network fitness to its neurons
                }

                //Computing Generation Best - current team with highest total fitness.
                //Note: individual predators in a team can have different fitness in
                //case of competition. However, all networks in a individual have same fitness
                for (int r = 0; r < num_of_predators; r++) {
                    current_teams_sum_individual_fitness[q] =
                            current_teams_sum_individual_fitness[q]
                                    + current_teams_pred[q][r
                                            * (total_predator_networks_per_team / num_of_predators
                                                    - 1)]->fitness;
                }
                current_teams_sum_individual_fitness[q] /= num_of_predators;     //Averaging over team

                // if team outperforms bestTeam, set bestTeam = team (Note: all networks in a individual have same fitness)
                if (current_teams_sum_individual_fitness[q]
                        > generation_best_teams_sum_individual_fitness[q]) {
                    //LOG(INFO) << " Replacing Best Team " <<endl;
                    generation_best_teams_sum_individual_fitness[q] =
                            current_teams_sum_individual_fitness[q];
                    for (int j = 0; j < total_predator_networks_per_team; j++) {
                        generation_best_teams[q][j]->setNetwork(current_teams_pred[q][j]);
                        generation_best_teams[q][j]->fitness = current_teams_pred[q][j]->fitness;
                    }
                }
                //LOG(INFO) << overall_fitness << " " << generation << "\n";
            }
        }     // end for
//    double overall_fitness = 0.0;
//    int i = 0.0;
//    for(i = 0; i < generation_best_teams[0].size(); i++){
//        overall_fitness += generation_best_teams[0][i]->fitness;
//    }
//    overall_fitness /= i;
        //LOG(INFO) << "Fitness " << generation_best_teams[0][1]->fitness;

        //**************END OF TRIALS********************************************//

        //Add the best over 1000 trials to the Hall of Fame
        hall_of_fame_pred.push_back(generation_best_teams);
//    hall_of_fame_prey.push_back(generation_best_teams_prey);

        // if generation best team (over 1000 trials) is better than overall best team (across generations) then replace
        for (int q = 0; q < num_teams_predator; q++) {

            if (generation_best_teams_sum_individual_fitness[q]
                    >= overall_best_teams_sum_individual_fitness[q]) {
                overall_best_teams_sum_individual_fitness[q] =
                        generation_best_teams_sum_individual_fitness[q];
                for (int j = 0; j < total_predator_networks_per_team; j++) {
                    overall_best_networks[q][j]->setNetwork(generation_best_teams[q][j]);
                    overall_best_networks[q][j]->fitness = generation_best_teams[q][j]->fitness;
                }
            }
        }

//    for (int q = 0; q < num_teams_prey; q++) {
//
//        if (generation_best_teams_prey_sum_individual_fitness[q]
//                >= overall_best_teams_prey_sum_individual_fitness[q]) {
//            overall_best_teams_prey_sum_individual_fitness[q] =
//                    generation_best_teams_prey_sum_individual_fitness[q];
//            for (int j = 0; j < total_prey_networks_per_team; j++) {
//                overall_best_teams_prey[q][j]->setNetwork(generation_best_teams_prey[q][j]);
//                overall_best_teams_prey[q][j]->fitness = generation_best_teams_prey[q][j]->fitness;
//            }
//        }
//    }

        /***********************START PRINTING FITNESSES FOR PREDATOR-PREY TEAMS*******************************/
        for (int q = 0; q < num_teams_predator; q++) {
            for (int r = 0; r < num_of_predators; r++) {
                LOG(INFO)
                        << "generation "
                        << generation
                        << ":\tPredator Team number "
                        << q
                        << ":\tPredator Number "
                        << r
                        << "  Generation Best fitness:: "
                        << generation_best_teams[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1)]->fitness
                        << ",  Overall Best fitness:: "
                        << overall_best_networks[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1)]->fitness
                        << ", Generation Average fitness  "
                        << average_predator_prey_teams_fitness[0][q][r] / numTrials << "\n";
            }
        }

        fout_champfitness << generation << " "
                << average_predator_prey_teams_fitness[0][0][0] / numTrials << "\n";

        //********Commenting below because prey does not evolve****************
        // for (int q=0; q<num_teams_prey; q++) {
        //         for (int r=0; r<num_of_prey; r++) {
        //             LOG(INFO) << "generation " << generation << ":\tPrey Team number "<< q << ":\tPrey Number "<< r <<"  Generation Best fitness:: "<< generation_best_teams_prey[q][r*(total_prey_networks_per_team/num_of_prey -1)]-> fitness
        //                     << ",  Overall Best fitness(JUNK):: " << overall_best_teams_prey[q][0]->fitness
        //                     << ", Generation Average fitness  " << average_predator_prey_teams_fitness[1][q][r]/numTrials
        //                     << "\n";
        //         }
        // }
        //********END Commenting below because prey does not evolve****************

        vector<vector<double> > testNet_result;     //number of prey caught by all the predators teams in each of the 20 trials

        //GENERATION BEST
        testNet_result = Envt.testNet(generation_best_teams, 20);

        for (int q = 0; q < num_teams_predator; q++) {
            LOG(INFO) << "\t\tGeneration Best Predator team number " << q << " caught "
                    << testNet_result[q][0] << " prey and killed  ";
            LOG(INFO) << testNet_result[q][1] << " predators on average over 20 trials ";
            LOG(INFO) << endl;
        }

        LOG(INFO) << endl;

        //OVERALL BEST
        testNet_result = Envt.testNet(overall_best_networks, 20);

        for (int q = 0; q < num_teams_predator; q++) {
            LOG(INFO) << "\t\tOverall Best Predator team number " << q << " caught "
                    << testNet_result[q][0] << " prey and killed  ";
            LOG(INFO) << testNet_result[q][1] << " predators on average over 20 trials ";
            LOG(INFO) << endl;
        }
        LOG(INFO) << endl;

        //TYPICAL TEAM (NEURONS RANDOMLY SELECTED FROM THE POPULATION)
        // form a random net for predator teams, and test it for 20 trials
        for (int q = 0; q < num_teams_predator; q++) {
            for (int j = 0; j < total_predator_networks_per_team; j++)
                for (int k = 0; k < num_hidden_neurons[j]; k++)
                    current_teams_pred[q][j]->setNeuron(
                            hidden_neuron_populations[q][j][k]->selectNeuron(), k);
        }

        testNet_result = Envt.testNet(current_teams_pred, 20);

        for (int q = 0; q < num_teams_predator; q++) {
            LOG(INFO) << "\t\tTypical Predator team number " << q << " caught "
                    << testNet_result[q][0] << " prey and killed  ";
            LOG(INFO) << testNet_result[q][1] << " predators on average over 20 trials ";
            LOG(INFO) << endl;
        }
        LOG(INFO) << endl;
        /***********************END PRINTING FITNESSES FOR PREDATOR-PREY TEAMS*******************************/

        /***********************START SIMULATION HERE*******************************/
        if (if_display) {

            LOG(INFO) << endl << "Running benchmark" << endl;
            if (generation % display_frequency == 0) {
                SHOW = 1;
            }

            //************INITIALIZING predator_prey_teams_fitness ********************
            predator_prey_teams_fitness.clear();
            temp_individual_fitness.clear();
            temp_team_fitness.clear();
            for (int p = 0; p < num_teams_predator; p++) {
                for (int q = 0; q < num_of_predators; q++) {
                    temp_individual_fitness.push_back(0.0);
                }
                temp_team_fitness.push_back(temp_individual_fitness);
                temp_individual_fitness.clear();
            }

            predator_prey_teams_fitness.push_back(temp_team_fitness);
            temp_team_fitness.clear();
            //************END INITIALIZING predator_prey_teams_fitness ********************

            for (int p = 0; p < SIMU_TRIALS; p++) {

                teams_fitness = Envt.evalNet(generation_best_teams, generation);
                LOG(INFO) << endl << " Simulation Trial Number:: " << p << endl;
                for (int q = 0; q < num_teams_predator; q++) {
                    for (int r = 0; r < num_of_predators; r++) {
                        predator_prey_teams_fitness[0][q][r] = predator_prey_teams_fitness[0][q][r]
                                + teams_fitness[0][q][r];
                        LOG(INFO) << " Generation Best Predator Team number " << q
                                << "  Predator Number " << r << "  fitness:: "
                                << teams_fitness[0][q][r] << endl;
                    }
                }
            }

            for (int q = 0; q < num_teams_predator; q++) {
                for (int r = 0; r < num_of_predators; r++) {
                    LOG(INFO) << "Average fitness for Predator Team Number " << q
                            << " Predator Number " << r << ":: "
                            << predator_prey_teams_fitness[0][q][r] / SIMU_TRIALS << endl;
                }
            }
        }

        /***********************END SIMULATION HERE*******************************/

        // save state into file
        if (generation == 5)
            save("out_5.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 100)
            save("out_100.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 200)
            save("out_200.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 300)
            save("out_300.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 500)
            save("out_500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 1000)
            save("out_1000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 1500)
            save("out_1500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2000)
            save("out_2000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2100)
            save("out_2100.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2200)
            save("out_2200.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2300)
            save("out_2300.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2400)
            save("out_2400.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 2500)
            save("out_2500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 3000)
            save("out_3000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 5000)
            save("out_5000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 7000)
            save("out_7000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9000)
            save("out_9000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9100)
            save("out_9100.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9200)
            save("out_9200.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9300)
            save("out_9300.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9400)
            save("out_9400.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 9500)
            save("out_9500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
        else if (generation == 10000)
            save("out_10000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 10500)
            save("out_10500.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 11000)
            save("out_11000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 11500)
            save("out_11500.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 12000)
            save("out_12000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 13000)
            save("out_13000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 14000)
            save("out_14000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 15000)
            save("out_15000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 16000)
            save("out_16000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 17000)
            save("out_17000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);
        else if (generation == 18000)
            save("out_18000.bin", num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey);

    }

//----------------------------------------------------------------------
// Make a decision about what to do when performace stagnates.
// bestNetwork is basically the network during the last delta phase
    void Esp::handleStagnation(int num_of_predators, int num_of_prey)
    {

        //perfQ.clear();
        //IS_PREY = false; //EVOLVE_PREY - Clamping to false
        //// Case when there was no improvement at all since last delta phase,
        //// so add or remove subpop
        //if(overall_best_teams[0]->fitness <= bestNetwork[0]->fitness){

        //  // add/remove subpops from ALL eligible predators
        //  for (int i=0; i<total_predator_networks_per_team; i++)

        //    if (removeSubPop(overall_best_teams, i, num_of_predators, num_of_prey)){
        //      bestNetwork[i]->setNetwork(overall_best_teams[i]);
        //      newDeltaPhase(i);
        //    }

        //    else
        //      addSubPop(i, num_of_predators, num_of_prey);

        //  if (INCREMENTAL_LEARNING)
        //    //Envt.simplifyTask();

        //  for (int i=0; i<total_predator_networks_per_team; i++)
        //    overall_best_teams[i]->fitness = 0.0;
        //

        //}

        //else
        //  for (int i=0; i<total_predator_networks_per_team; i++) {
        //    bestNetwork[i]->setNetwork(overall_best_teams[i]);
        //    newDeltaPhase(i);
        //  }

    }

//----------------------------------------------------------------------
// Start a new delta phase
    void Esp::newDeltaPhase(int pred)
    {
        // LOG(INFO) << "DELTA started on predator " << pred << endl;
        // SKIP = 1;

        // // For each subpop, call deltify w.r.t. its best neuron (which is the
        // // one used by bestNetwork)
        // if(!IS_PREY) {
        // for(int k=0; k < numPops[pred]; ++k){
        //   total_hidden_neuron_populations[pred][k]->deltify( bestNetwork[pred]->pop[k] );
        // }
        // }
        // else {
        // for(int k=0; k < numPops[0]; ++k){
        //   total_hidden_neuron_populations_prey[pred][k]->deltify( bestNetwork_prey[pred]->pop[k] );
        // }
        // }

    }

//----------------------------------------------------------------------
// get average fitness level.
    void Esp::average(int num_teams_predator, int num_teams_prey)
    {
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < total_predator_networks_per_team; i++)
                for (int j = 0; j < num_hidden_neurons[i]; j++)
                    hidden_neuron_populations[k][i][j]->average();
        }
    }

/////////////////////////////////////////////////////////////////////
//
// ESP I/O fns
    void Esp::printNeurons()
    {
        LOG(INFO) << "ESP::printNeurons() is commented out" << endl;
        /*  char filename[50];
         sprintf(filename, "./%u_%d_%d.pop",     //3-6-00: %u used to be %lu
         getpid(), numPops, generation);
         FILE *fptr;

         if ((fptr = fopen(filename,"w")) == NULL) {
         printf("\n Error - cannot open %s",filename);
         exit(1);
         }
         for(int i=0; i < numPops; ++i){
         total_hidden_neuron_populations[i]->printWeight(fptr);
         }
         fclose(fptr);
         */
    }

//----------------------------------------------------------------------
//
    void Esp::loadSeedNet(char *filename)
    {
        LOG(INFO) << "ESP::loadSeedNet (char* filename) is commented out" << endl;
        /*  netType = SCDORDER;  //change this to read the type from the filename;
         numPops = bestNetwork->load(filename);
         printf("NUM_POPS %d\n", numPops);
         printf("GENE_SIZE_PRED_NW %d\n", GENE_SIZE_PRED_NW);
         */
    }

    void Esp::save(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey)
    {
        FILE* fptr;
        fname = (char*) file_prefix.append(fname).c_str();
        if ((fptr = fopen(fname, "wb")) == NULL) {
            LOG(FATAL) << endl << "Error - cannot open " << fname << endl;
            //exit(1);
        }

        fwrite(&generation, 4, 1, fptr);     // int generation

        for (int p = 0; p < num_teams_predator; p++)
            for (int i = 0; i < total_predator_networks_per_team - (COMBINE * num_of_predators);
                    i++)
                for (int j = 0; j < num_hidden_neurons[i]; j++)
                    for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                        for (int m = 0; m < GENE_SIZE_PRED_NW; m++) {     // weights
                            fwrite(&(hidden_neuron_populations[p][i][j]->pop[k]->weight[m]),
                                    sizeof(double), 1, fptr);
                        }

        for (int p = 0; p < num_teams_predator; p++)
            for (int i = total_predator_networks_per_team - (COMBINE * num_of_predators);
                    i < total_predator_networks_per_team; i++)
                for (int j = 0; j < num_hidden_neurons[i]; j++)
                    for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                        for (int m = 0; m < GENE_SIZE_PRED_COMBINER_NW; m++) {     // weights
                            fwrite(&(hidden_neuron_populations[p][i][j]->pop[k]->weight[m]),
                                    sizeof(double), 1, fptr);
                        }

        fclose(fptr);

        LOG(INFO) << endl << "Saved to " << fname << endl << endl;
    }

    void Esp::findChampion()
    {
        LOG(INFO) << "ESP::findChampion() is commented out" << endl;
        /*
         int j,l;
         double fitness, best=-999999;
         char subname[15];
         FILE *fptr;
         subPop *net;

         strcpy(subname,"000best.bin");
         for(l=0;l<1000;++l) {
         j = l/100;
         subname[0] = j + 48;
         j = l%100;
         subname[1] = j/10 + 48;
         subname[2] = j%10 + 48;
         if ((fptr = fopen(subname,"r")) != NULL) {
         fclose(fptr);
         net = load(subname);
         fitness = 0;

         // evaluate network
         //ccfitness += state.apply(net);
         if (fitness > best)
         best = fitness;
         fptr = fopen("report.txt","a");
         fprintf(fptr,"%d ",l);

         fclose(fptr);
         }
         }

         fptr=fopen("analyze.out","a");
         fprintf(fptr,"%f\n",1/best);
         fclose(fptr);
         printf("%f\n",1/best);
         */

    }

    void Esp::endEvolution()
    {
        //save ("end.bin", num_of_predators, num_of_prey);
        printStats();
        fout_champfitness.close();
        LOG(INFO) << "Done with evolution";
        //exit(0);
    }

    void Esp::printStats()
    {
        LOG(INFO) << "ESP::printStats() is commented out" << endl;
//  printf("\nTotal number of network evaluations : %d\n", generation*numTrials);
    }

//----------------------------------------------------------------------
// add the appropriate connection wieghts that are needed to add
// a unit to each type of network.
    void Esp::addConnections()
    {
        switch (netType) {
            case FF:
                break;
                /*  case FR:
                 case SRN:
                 for(int i=0; i < numPops; ++i)  // add connection to neurons in spops
                 total_hidden_neuron_populations[i]->addConnection(NUM_INPUTS);
                 break;
                 case SCDORDER:
                 for(int i=0; i < numPops; ++i)  // add connection to neurons in spops
                 for(int j=1; j < NUM_INPUTS+1; ++j)  // add connection to neurons in spops
                 total_hidden_neuron_populations[i]->addConnection(numPops*j+j-1);
                 break;
                 */
        }
    }

//----------------------------------------------------------------------
// opposite of addConnections.
    void Esp::removeConnections(int sp)
    {
        switch (netType) {
            case FF:
                break;
                /*  case FR:
                 case SRN:
                 for(int i=0; i < numPops; ++i)  // remove connection to neurons in spops
                 total_hidden_neuron_populations[i]->removeConnection(NUM_INPUTS-1);
                 break;
                 case SCDORDER:
                 for(int i=0; i < numPops; ++i)
                 for(int j=1; j < NUM_IN+1; ++j)  // remove connection to neurons in spops
                 total_hidden_neuron_populations[i]->removeConnection((numPops-1)*j);
                 */
        }
    }

//----------------------------------------------------------------------
// add a new subbpop. In effect: add a unit to the networks.
    void Esp::addSubPop(int pred, int num_of_predators, int num_of_prey)
    {
        // subPop *newSp;    //pointer to new subpop.
        // addConnections();

        // for(int i=0; i < numPops[pred]; ++i)  //freeze best network? false=yes
        //   total_hidden_neuron_populations[pred][i]->evolvable = true;    // true = no.

        // ++(numPops[pred]);             // inc # subpops
        // setupNetDimensions(num_of_predators, num_of_prey);  //adjust NUM_INPUTS, GENE_SIZE_PRED_NW
        // overall_best_teams[pred]->addNeuron();  // add a neuron to the overall_best_teams and best net.
        // bestNetwork[pred]->addNeuron();

        // // construct and create new subpop
        // newSp = new subPop(total_hidden_neuron_populations[pred][0]->numNeurons);
        // newSp->create();

        // // add that subpop to pred
        // total_hidden_neuron_populations[pred].push_back( newSp ); // put its pointer in Esp's vector
        //                                //  of sp pointers.

        // LOG(INFO) << "Adding SUBPOP " << numPops[pred]
        //      << " to predator " << pred << endl;
    }

//----------------------------------------------------------------------
// opposite of addSubPop.
    int Esp::removeSubPop(vector<Network*>& team, int pred, int num_of_predators, int num_of_prey)
    {
        int sp;

        // see which (if any) subpop can be removed from pred
        sp = team[pred]->lesion(Envt, team, num_of_predators, num_of_prey);
        return (removeSubPop(sp, pred, num_of_predators, num_of_prey));     // remove it.
    }

//----------------------------------------------------------------------
    int Esp::removeSubPop(int sp, int pred, int num_of_predators, int num_of_prey)
    {
//  if (sp >= 0)
//    {
//      removeConnections(sp);
//      --(numPops[pred]);
//      setupNetDimensions(num_of_predators, num_of_prey);
//
//      overall_best_teams[pred]->removeNeuron(sp);
//      bestNetwork[pred]->removeNeuron(sp);
//
//
////      delete total_hidden_neuron_populations[pred][sp];
//      total_hidden_neuron_populations[pred].erase(total_hidden_neuron_populations[pred].begin()+sp);
//
//      LOG(INFO) << "Remove SUBPOP " << sp << " from predator " << pred << endl;
//      LOG(INFO) << "Now " << numPops[pred] << " subpops\n";
//      return 1;
//    }
//  else
        return 0;
    }

    /**
     * recombine neurons with members of their subpop using crossover. EVOLVE_PREY
     * @param network
     */
    void Esp::recombine_hall_of_fame(Network* network)
    {
        int i = 0;
        for (; i < numBreed - 5; ++i) {
            crossover(neurons[i]->weight, neurons[findMate(i)]->weight,
                    neurons[numHiddenNeurons - (1 + i * 2)]->weight, neurons[numHiddenNeurons - (2 + i * 2)]->weight);
        }

        // NOTE: Need to make sure this is number of neurons
        for (int j = 0; j < numHiddenNeurons; j++) {
            for (; i < numBreed; ++i) {
                crossover(neurons[i]->weight, network->neurons[j]->weight,
                        neurons[numHiddenNeurons - (1 + i * 2)]->weight,
                        neurons[numHiddenNeurons - (2 + i * 2)]->weight);
            }
        }
    }

//////////////////////////////////////////////////////////////////////
//
// Other functions
//
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
// reseed random fns.
    void reseed(int val)
    {
        unsigned short seed[3];

        seed[0] = val;
        seed[1] = val + 1;
        seed[2] = val + 2;
        seed48(seed);
        srand48(val);
    }

//----------------------------------------------------------------------
// generate a random number form a cauchy distribution centered on zero.
#define PI 3.1415926535897931
    double rndCauchy(double wtrange)
    {
        double u = 0.5, Cauchy_cut = 10.0;

        while (u == 0.5)
            u = drand48();
        u = wtrange * tan(u * PI);
        if (fabs(u) > Cauchy_cut)
            return rndCauchy(wtrange);
        else
            return u;
    }

//----------------------------------------------------------------------
// is it a number?
    int isnumber(char *str)
    {
        int i, res = 1;

        for (i = 0; i < (int) strlen(str); ++i)
            if (!isdigit(str[i]))
                res = 0;

        return res;
    }

//----------------------------------------------------------------------
// Parse command-line arguments.
    /*
     static void parseArgs(int argc, char *argv[], int *numPops, int *popSize, int *seed,
     int *netType, bool *analyze, int *seedNet)
     {
     // Command-line defaults.
     const int DEFAULT_NUM_POPS = 10;
     const int DEFAULT_POP_SIZE = 100;

     // Set defaults.
     *numPops = DEFAULT_NUM_POPS;
     *popSize = DEFAULT_POP_SIZE;
     *netType = FF;
     *seed = getpid();

     //int this_option_optind = optind ? optind : 1;
     int option_index = 0;
     static struct option long_options[] = {
     //  {"plot", 0, &PLOTTING, 1},
     //  {"delta", 0, &DELTA, 1},
     { "minimize", 0, &MIN, 1 }, { "stag", 1, 0, 0 }, { "mutation", 1, 0, 0 } };

     // Parse arguments.
     int c;
     while ((c = getopt_long(argc, argv, "z:n:s:t:h", long_options, &option_index)) != EOF) {
     switch (c) {
     case 0:
     if (!strcmp("stag", long_options[option_index].name)) {
     STAG = atoi(optarg);
     printf("STAG : %d\n", STAG);
     } else if (!strcmp("mutation", long_options[option_index].name)) {
     MUT_RATE = atof(optarg);
     printf("MUT_RATE : %f\n", MUT_RATE);
     }
     break;  //undocumented
     case 'z':
     *numPops = atoi(optarg);
     break;
     case 'n':
     *popSize = atoi(optarg);
     break;
     case 's':
     *seed = atoi(optarg);
     break;
     case 't':
     *netType = atoi(optarg);
     break;
     case 'x':
     *analyze = true;
     break;
     case 'f':
     *seedNet = optind - 1;
     break;
     case 'h':
     LOG(INFO) << "Usage: " << argv[0] << " <options>\n";
     LOG(INFO) << "options:\n";
     LOG(INFO) << "  -z Z : Z number of hidden units.\n";
     LOG(INFO) << "  -n N : N neurons in each subpop\n";
     LOG(INFO) << "  -s S : Initial integer seed S.\n";
     LOG(INFO) << "  -t T : Type of Network.\n";
     LOG(INFO) << "         0 = Feedforward, 1 = SRN, 2 = 2ndOrder Recurrent\n";
     LOG(INFO) << "         3 = Fully Recurrent\n";
     //LOG(INFO) << "  -f F : Seed network. \n";
     LOG(INFO) << "  --mutation M : Mutation rate in percent (0,1). \n";
     LOG(INFO) << "  --stag STAG : Stagnation threshold, number of generations. \n";
     LOG(INFO) << "  --minimize : Minimize fitness. \n";
     //      LOG(INFO) << "  -x   : analyze nets.\n";
     LOG(INFO) << "  -h   : this message.\n";
     LOG(INFO) << "Default: " << argv[0] << " -z " << DEFAULT_NUM_POPS << " -n "
     << DEFAULT_POP_SIZE << " -s getpid()" << " -t " << FF
     << " --mutation 0.4" << " --stag 20";
     LOG(INFO) << "\n" << flush;
     exit(0);
     }
     }
     }*/

//----------------------------------------------------------------------
// Check command-line arguments.
    static void checkArgs(int numPops, int popSize)
    {
        if (MIN)
            LOG(INFO) << "MINIMIZING\n";
    }
}
