/*
 * Esp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "EspExperiment.h"
#include "common.h"
#include "Network.h"
#include "FeedForwardNetwork.h"
#include "SubPop.h"

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

    EspExperiment::EspExperiment(const char* configFilePath) :
            Experiment(configFilePath)
    {
        libconfig::Config cfg;

        LOG(INFO) << "Reading from esp parameters from the config file " << configFilePath;
        cfg.readFile(configFilePath);

        numAgents = numPredators;

        numHiddenNeurons = cfg.lookup("experiment:esp:num_hidden_neurons");
        LOG(INFO) << "Number of hidden neurons set to " << numHiddenNeurons;

        popSize = cfg.lookup("experiment:esp:population_size");
        LOG(INFO) << "ESP population size set to " << popSize;

        netTp = (cfg.lookup("experiment:esp:net_type") == "FF") ? FF : LOG(ERROR) << "Unknown net type";
        LOG(INFO) << "Network type set to " << cfg.lookup("experiment:esp:net_type");

        // TODO Make this valid for teams of predators, prey and hunters too
        numOtherAgents = numPredators + numPrey + numHunters;

        // NOTE Hardcoding it right now. Will be 5 for the grid world, and will change only for a different domain.
        // TODO Take this as a parameter
        numActions = 5;

        init();
    }

    EspExperiment::init() :
    {
        generation = 0;
        inputDimensions = 2;
        numInputs = numOtherAgents * inputDimensions; // NOTE: Assuming (x,y) gives position of agent.
        // NOTE: (contd.) Should change it for other problems/ problems with more than 2 dimensions
        numOutputs = numActions;
        totalNumNetworks = numOtherAgents * numAgents + COMBINE * numAgents;
        neuronGeneSize = getNeuronGeneSize();
        combinerNeuronGeneSize = getCombinerNeuronGeneSize();

        LOG(INFO) << endl << "number of inputs is " << numInputs << endl
                << "number of outputs is " << numOutputs << endl << " total number of networks is "
                << totalNumNetworks << endl << " neuron gene size for a normal neuron is "
                << neuronGeneSize << endl << " neuron gene size for a combiner neuron is "
                << combinerNeuronGeneSize << endl;

        // Initialize and create overall_best_networks
        // Only one team here
        for (uint i = 0; i < totalNumNetworks - COMBINE * numAgents; i++) {
            overall_best_networks.push_back(
                    generateNetwork(netTp, numHiddenNeurons, neuronGeneSize)); // generate new network
            overall_best_networks[i]->create();     // create the network
        }
        for (uint i = totalNumNetworks - COMBINE * numAgents; i < totalNumNetworks; i++) {
            overall_best_networks.push_back(
                    generateNetwork(netTp, numHiddenNeurons, combinerNeuronGeneSize)); // generate new network
            overall_best_networks[i]->create();     // create the network
        }
        LOG(INFO) << "Initialized and created overall_best_networks";

        // Initialize and create hidden_neuron_populations
        vector<SubPop*>* temp;
        for (uint i = 0; i < totalNumNetworks - COMBINE * numAgents; i++) {
            temp = new vector<SubPop*>;
            for (uint j = 0; j < numHiddenNeurons; j++) {
                SubPop* tempSubPop = new SubPop(popSize, neuronGeneSize);
                tempSubPop->create();
                temp->push_back(tempSubPop);
            }
            hidden_neuron_populations.push_back(*temp);
        }
        for (uint i = totalNumNetworks - COMBINE * numAgents; i < totalNumNetworks; i++) {
            temp = new vector<SubPop*>;
            for (uint j = 0; j < numHiddenNeurons; j++) {
                SubPop* tempSubPop = new SubPop(popSize, combinerNeuronGeneSize);
                tempSubPop->create();
                temp->push_back(tempSubPop);
            }
            hidden_neuron_populations.push_back(*temp);
        }
        LOG(INFO) << "Initialized and created hidden_neuron_populations";

    }

    EspExperiment::~EspExperiment()
    {
        for (uint i = 0; i < totalNumNetworks; i++) {
            for (uint j = 0; j < numHiddenNeurons; j++) {
                delete hidden_neuron_populations[i][j];
            }
            delete overall_best_networks[i];
        }
        hidden_neuron_populations.clear();
        overall_best_networks.clear();
    }

    uint EspExperiment::getNeuronGeneSize()
    {
        // Assume network type is FF
        return inputDimensions + numOutputs;
    }

    uint EspExperiment::getCombinerNeuronGeneSize()
    {
        // Assume network type is FF
        return (numOutputs * numOtherAgents + numOutputs);
    }

    Network* EspExperiment::generateNetwork(const uint& networkType, const uint& numHiddenNeurons,
            const uint& neuronGeneSize)
    {
        switch (networkType) {
            case FF:
                return new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize);
                //break;
                /*  case FR :
                 return( new FullyRecurrentNetwork(nPops) ); break;
                 case SRN :
                 return( new SimpleRecurrentNetwork(nPops) ); break;
                 case SCDORDER :
                 return( new SecondOrderRecurrentNetwork(nPops) );
                 */

            default:
                return new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize);
                //break;
        }
    }

    void EspExperiment::evolve(const uint& cycles)
    {
        LOG(INFO) << "Starting evolve and running " << cycles << " cycles" << endl;
        while (/*gInterrupt == false && */++generation <= cycles) {
            evalPop();     //evaluate neurons

            for (uint i = 0; i < totalNumNetworks; i++) {
                for (uint j = 0; j < numHiddenNeurons; j++) {
                    hidden_neuron_populations[i][j]->qsortNeurons();
                    hidden_neuron_populations[i][j]->recombine_hall_of_fame();
                }
            }

            for (uint i = 0; i < totalNumNetworks; i++) {
                for (uint j = 0; j < numHiddenNeurons; j++) {
                    hidden_neuron_populations[i][j]->mutate();
                }
            }
        }     //end while

        endEvolution();
        LOG(INFO) << "Ending evolve" << endl;
    }

    void EspExperiment::evalPop()
    {

        evalReset();            // reset fitness and test values of each neuron
        performEval();     // evaluate the networks
        average(num_teams_predator, num_teams_prey);     // normalize neuron fitnesses

    }

    void EspExperiment::evalReset()
    {
        for (uint i = 0; i < totalNumNetworks; i++) {
            for (uint j = 0; j < numHiddenNeurons; j++) {
                hidden_neuron_populations[i][j]->evalReset();
            }
        }
    }

    void EspExperiment::performEval()
    {
        static int evaluations = 0;
        vector<Network*> temp_networks;     // vector of current networks
        vector<Network*> temp_bestNetworks;     // vector of best networks

        vector<Network*> current_networks;     // current pred teams
        vector<Network*> generation_best_networks; // vector of best pred teams in a given generation

        vector<double> temp_individual_fitness; //Temporary variable for storing predator/prey team fitness
        vector<double> temp_team_fitness; //Temporary variable for storing predator/prey team fitness
        vector<vector<double> > predator_prey_teams_fitness; //Vector for all teams fitness together after averaging over 6 trials
        vector<vector<double> > average_predator_prey_teams_fitness; //Vector for all teams fitness together after averaging over 6000 trials
        vector<vector<vector<double> > > teams_fitness; //temporary variable for all teams fitness together per trial

        vector<double> current_teams_sum_individual_fitness;   //Sum of individual network fitnesses
        vector<double> generation_best_teams_sum_individual_fitness; //Sum of individual network fitnesses

        temp_networks.clear();
        temp_bestNetworks.clear();
        for (uint i = 0; i < totalNumNetworks - COMBINE * numAgents; i++) {
            temp_networks.push_back(generateNetwork(netTp, numHiddenNeurons, neuronGeneSize));
            temp_bestNetworks.push_back(generateNetwork(netTp, numHiddenNeurons, neuronGeneSize));
            temp_bestNetworks[i]->create();
        }
        for (uint i = totalNumNetworks - COMBINE * numAgents; i < totalNumNetworks; i++) {
            temp_networks.push_back(generateNetwork(netTp, numHiddenNeurons, neuronGeneSize));
            temp_bestNetworks.push_back(generateNetwork(netTp, numHiddenNeurons, neuronGeneSize));
            temp_bestNetworks[i]->create();
        }
        current_networks = temp_networks;
        generation_best_networks = temp_bestNetworks;

        //***************START Initialize average fitness (calculated across 1000 trials) to zero********************//
        for (uint i = 0; i < numAgents; i++) {
            temp_individual_fitness.push_back(0.0);
        }
        temp_team_fitness = temp_individual_fitness;
        temp_individual_fitness.clear();

        average_predator_prey_teams_fitness.push_back(temp_team_fitness);
        temp_team_fitness.clear();
        //***************END Initialize average fitness (calculated across 1000 trials) to zero********************//

        //**************START OF TRIALS********************************************//
        for (uint i = 0; i < numTrials; ++i) {
            if ((int) sub_hall_of_fame_pred.size() == 2) {
                // If there are pre-existing neurons, set neurons only for the combiner network. Get other neurons from networks learnt in the sub task
                // For the subnetwork, there will be a combiner network. So two hidden layers
                // FIXME This will only work when the no. of prey and hunter is 1 !!
                int sub_hall_of_fame_pred_size = sub_hall_of_fame_pred[0].size();
                for (int j = 0; j < num_teams_predator; j++) {
                    int i;
                    int sub_total_predator_networks_per_team =
                            sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j].size();
                    // Takes care of prey sub networks
                    for (i = 0; i < sub_total_predator_networks_per_team; //num_of_predators * num_teams_prey * num_of_prey /*actually, number of prey networks. Should be num_teams_prey * num_of_prey*/;
                            i++) {
                        if (i < num_of_predators * num_teams_prey * num_of_prey) {
                            current_networks[j][i]->setNetwork(
                                    sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j][i]);
                            current_networks[j][i]->incrementTests();
                        } else {
                            for (int k = 0; k < num_of_predators * num_teams_prey * num_of_prey;
                                    k++) {
                                current_networks[j][k]->addHiddenLayer(
                                        sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j][i]->pop);
                            }
                        }
                    }
                    // Takes care of hunter sub networks
                    int k;
                    i = num_of_predators * num_teams_prey * num_of_prey;
                    sub_total_predator_networks_per_team =
                            sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j].size();
                    for (k = i; k < i + sub_total_predator_networks_per_team; //num_of_predators * num_teams_hunters * num_of_hunters /*actually, number of hunter networks. Should be num_teams_hunter * num_of_hunter*/;
                            k++) {
                        if (k < i + num_of_predators * num_teams_hunters * num_of_hunters) {
                            current_networks[j][k]->setNetwork(
                                    sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j][k
                                            - i]);
                            current_networks[j][k]->incrementTests();
                        } else {
                            for (int l = 0;
                                    l < num_of_predators * num_teams_hunters * num_of_hunters;
                                    l++) {
                                current_networks[j][l]->addHiddenLayer(
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
                            current_networks[j][l]->setNeuron(
                                    hidden_neuron_populations[j][l][m]->selectNeuron(), m);
                        }
                        current_networks[j][l]->incrementTests();
                    }
                }

            } else { // If there are no existing subnetworks already evolved, set neurons for all the hidden layers (including combiner network)

                for (int p = 0; p < num_teams_predator; p++) {
                    for (int j = 0; j < total_predator_networks_per_team; j++) {
                        for (int k = 0; k < num_hidden_neurons[j]; k++) {
                            current_networks[p][j]->setNeuron(
                                    hidden_neuron_populations[p][j][k]->selectNeuron(), k);
                        }
                        current_networks[p][j]->incrementTests(); //Counting the number of trials for a neuron (to compute average later)
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
                teams_fitness = Envt.evalNet(current_networks, generation);
                for (int q = 0; q < num_teams_predator; q++) {
                    for (int r = 0; r < num_of_predators; r++) { //Individual predator fitness is used in case of competing predators
                        predator_prey_teams_fitness[0][q][r] = predator_prey_teams_fitness[0][q][r]
                                + teams_fitness[0][q][r];
                    }
                }
            }

            //Predator - Distribute fitness to each contributing neurons
            for (int q = 0; q < num_teams_predator; q++) {
                for (int r = 0; r < num_of_predators; r++) {
                    predator_prey_teams_fitness[0][q][r] /= EVALTRIALS;
                    average_predator_prey_teams_fitness[0][q][r] +=
                            predator_prey_teams_fitness[0][q][r];

                    for (int j = 0; j < (total_predator_networks_per_team / num_of_predators - 1);
                            j++) {
                        current_networks[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1) + j]->fitness =
                                predator_prey_teams_fitness[0][q][r]; // distribute each predator's fitness to constituent networks
                        current_networks[q][r
                                * (total_predator_networks_per_team / num_of_predators - 1) + j]->addFitness(); // add network fitness to its neurons
                    }

                    //Following two lines are for the combiner network
                    current_networks[q][(total_predator_networks_per_team - num_of_predators) + r]->fitness =
                            predator_prey_teams_fitness[0][q][r]; // distribute each predator's fitness to constituent networks
                    current_networks[q][(total_predator_networks_per_team - num_of_predators) + r]->addFitness(); // add network fitness to its neurons
                }

                //Computing Generation Best - current team with highest total fitness.
                //Note: individual predators in a team can have different fitness in
                //case of competition. However, all networks in a individual have same fitness
                for (int r = 0; r < num_of_predators; r++) {
                    current_teams_sum_individual_fitness[q] =
                            current_teams_sum_individual_fitness[q]
                                    + current_networks[q][r
                                            * (total_predator_networks_per_team / num_of_predators
                                                    - 1)]->fitness;
                }
                current_teams_sum_individual_fitness[q] /= num_of_predators;   //Averaging over team

                // if team outperforms bestTeam, set bestTeam = team (Note: all networks in a individual have same fitness)
                if (current_teams_sum_individual_fitness[q]
                        > generation_best_teams_sum_individual_fitness[q]) {
                    generation_best_teams_sum_individual_fitness[q] =
                            current_teams_sum_individual_fitness[q];
                    for (int j = 0; j < total_predator_networks_per_team; j++) {
                        generation_best_networks[q][j]->setNetwork(current_networks[q][j]);
                        generation_best_networks[q][j]->fitness = current_networks[q][j]->fitness;
                    }
                }
            }
        }     // end for

        //**************END OF TRIALS********************************************//

        //Add the best over 1000 trials to the Hall of Fame
        hall_of_fame_pred.push_back(generation_best_networks);

        // if generation best team (over 1000 trials) is better than overall best team (across generations) then replace
        for (int q = 0; q < num_teams_predator; q++) {

            if (generation_best_teams_sum_individual_fitness[q]
                    >= overall_best_teams_sum_individual_fitness[q]) {
                overall_best_teams_sum_individual_fitness[q] =
                        generation_best_teams_sum_individual_fitness[q];
                for (int j = 0; j < total_predator_networks_per_team; j++) {
                    overall_best_networks[q][j]->setNetwork(generation_best_networks[q][j]);
                    overall_best_networks[q][j]->fitness = generation_best_networks[q][j]->fitness;
                }
            }
        }

        /***********************START PRINTING FITNESSES FOR PREDATOR-PREY TEAMS*******************************/
        for (int q = 0; q < num_teams_predator; q++) {
            for (int r = 0; r < num_of_predators; r++) {
                LOG(INFO) << "generation " << generation << ":\tPredator Team number " << q
                        << ":\tPredator Number " << r << "  Generation Best fitness:: "
                        << generation_best_networks[q][r
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

        vector<vector<double> > testNet_result; //number of prey caught by all the predators teams in each of the 20 trials

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
                    current_networks[q][j]->setNeuron(
                            hidden_neuron_populations[q][j][k]->selectNeuron(), k);
        }

        testNet_result = Envt.testNet(current_networks, 20);

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

                teams_fitness = Envt.evalNet(generation_best_networks, generation);
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
    }

    // NOTE dirty class member functions here

//----------------------------------------------------------------------
// Make a decision about what to do when performace stagnates.
// bestNetwork is basically the network during the last delta phase
    void EspExperiment::handleStagnation(int num_of_predators, int num_of_prey)
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
    void EspExperiment::newDeltaPhase(int pred)
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
    void EspExperiment::average(int num_teams_predator, int num_teams_prey)
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
    void EspExperiment::printNeurons()
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
    void EspExperiment::loadSeedNet(char *filename)
    {
        LOG(INFO) << "ESP::loadSeedNet (char* filename) is commented out" << endl;
        /*  netType = SCDORDER;  //change this to read the type from the filename;
         numPops = bestNetwork->load(filename);
         printf("NUM_POPS %d\n", numPops);
         printf("GENE_SIZE_PRED_NW %d\n", GENE_SIZE_PRED_NW);
         */
    }

    void EspExperiment::save(char* fname, int num_of_predators, int num_of_prey,
            int num_teams_predator, int num_teams_prey)
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

    void EspExperiment::findChampion()
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

    void EspExperiment::endEvolution()
    {
        //save ("end.bin", num_of_predators, num_of_prey);
        printStats();
        fout_champfitness.close();
        LOG(INFO) << "Done with evolution";
        //exit(0);
    }

    void EspExperiment::printStats()
    {
        LOG(INFO) << "ESP::printStats() is commented out" << endl;
//  printf("\nTotal number of network evaluations : %d\n", generation*numTrials);
    }

//----------------------------------------------------------------------
// add the appropriate connection wieghts that are needed to add
// a unit to each type of network.
    void EspExperiment::addConnections()
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
    void EspExperiment::removeConnections(int sp)
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
    void EspExperiment::addSubPop(int pred, int num_of_predators, int num_of_prey)
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
    int EspExperiment::removeSubPop(vector<Network*>& team, int pred, int num_of_predators,
            int num_of_prey)
    {
        int sp;

        // see which (if any) subpop can be removed from pred
        sp = team[pred]->lesion(Envt, team, num_of_predators, num_of_prey);
        return (removeSubPop(sp, pred, num_of_predators, num_of_prey));     // remove it.
    }

//----------------------------------------------------------------------
    int EspExperiment::removeSubPop(int sp, int pred, int num_of_predators, int num_of_prey)
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
    void EspExperiment::recombine_hall_of_fame(Network* network)
    {
        int i = 0;
        for (; i < numBreed - 5; ++i) {
            crossover(neurons[i]->weight, neurons[findMate(i)]->weight,
                    neurons[numHiddenNeurons - (1 + i * 2)]->weight,
                    neurons[numHiddenNeurons - (2 + i * 2)]->weight);
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
