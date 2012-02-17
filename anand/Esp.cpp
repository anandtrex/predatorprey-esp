/*
 * Esp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "Esp.h"
#include "common.h"
#include "Network.h"
#include "FeedForwardNetwork.h"
#include "SubPop.h"

#define FF 0
#define SRN 1
#define SCDORDER 2
#define FR 3

// Assume combiner network used.
// Assume number of hidden neurons is the same for all networks

namespace EspPredPreyHunter
{
    using std::vector;
    using std::endl;

    Esp::Esp() :
            numHiddenNeurons(0), popSize(0), netType(0), numOtherAgents(0), numActions(0)
    {
    }

    Esp::Esp(const uint& nHiddenNeurons, const uint& popSize, const uint& netTp,
            const uint& numOtherAgents, const uint& numActions) :
            numHiddenNeurons(nHiddenNeurons), popSize(popSize), netType(netTp), numOtherAgents(
                    numOtherAgents), numActions(numActions)
    {
        generation = 0;
        inputDimensions = 2;
        numInputs = numOtherAgents * inputDimensions; // NOTE: Assuming (x,y) gives position of agent.
        // NOTE: (contd.) Should change it for other problems/ problems with more than 2 dimensions
        numOutputs = numActions;
        totalNumNetworks = numOtherAgents + COMBINE;
        neuronGeneSize = getNeuronGeneSize();
        combinerNeuronGeneSize = getCombinerNeuronGeneSize();

        LOG(INFO) << endl << "number of inputs is " << numInputs << endl
                << "number of outputs is " << numOutputs << endl << " total number of networks is "
                << totalNumNetworks << endl << " neuron gene size for a normal neuron is "
                << neuronGeneSize << endl << " neuron gene size for a combiner neuron is "
                << combinerNeuronGeneSize << endl;

        networks = vector<Network*>();

        // Initialize and create networks
        for (uint i = 0; i < totalNumNetworks - COMBINE; i++) {
            networks.push_back(generateNetwork(netTp, neuronGeneSize));     // generate new network
        }
        for (uint i = totalNumNetworks - COMBINE; i < totalNumNetworks; i++) {
            networks.push_back(generateNetwork(netTp, combinerNeuronGeneSize)); // generate new network
        }
        LOG(INFO)
                << "Initialized and created networks. (subpopulations initialized within the networks)";

        // FIXME Do we need these?????
        // Initialize and create overall_best_networks
        for (uint i = 0; i < totalNumNetworks - COMBINE; i++) {
            overall_best_networks.push_back(generateNetwork(netTp, neuronGeneSize)); // generate new network
        }
        for (uint i = totalNumNetworks - COMBINE; i < totalNumNetworks; i++) {
            overall_best_networks.push_back(generateNetwork(netTp, combinerNeuronGeneSize)); // generate new network
        }
        LOG(INFO) << "Initialized and created overall_best_networks";

        networkContainer = new NetworkContainer();
        LOG(INFO) << "Created network container for esp";
    }

    Esp::~Esp()
    {
        for (uint i = 0; i < totalNumNetworks; i++) {
            delete overall_best_networks[i];
            delete networks[i];
        }
        overall_best_networks.clear();
        networks.clear();
    }

    uint Esp::getNeuronGeneSize()
    {
        // Assume network type is FF
        return inputDimensions + numOutputs;
    }

    uint Esp::getCombinerNeuronGeneSize()
    {
        // Assume network type is FF
        return (numOutputs * numOtherAgents + numOutputs);
    }

    Network* Esp::generateNetwork(const uint& networkType, const uint& neuronGeneSize)
    {
        VLOG(4) << "Generating new network with network type " << networkType << ", population size " << popSize
                << " and neuron gene size " << neuronGeneSize;
        return new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize, popSize);
        /*
        switch (networkType) {
            case FF:
                return new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize, popSize);
                //break;
                /--  case FR :
                 return( new FullyRecurrentNetwork(nPops) ); break;
                 case SRN :
                 return( new SimpleRecurrentNetwork(nPops) ); break;
                 case SCDORDER :
                 return( new SecondOrderRecurrentNetwork(nPops) );
                 --/

            default:
                return new FeedForwardNetwork(numHiddenNeurons, neuronGeneSize, popSize);
                //break;
        }*/
    }

    NetworkContainer* Esp::getNetwork()
    {
        for (int i = 0; i < totalNumNetworks; i++) {
            // Selects random neurons from subpopulation and returns it
            networks[i]->setNeurons();
        }
        networkContainer->setNetworks(networks);
        return networkContainer;
    }

    void Esp::evalReset()
    {
        for (uint i = 0; i < totalNumNetworks; i++) {
            networks[i]->evalReset();
        }
    }

    // NOTE dirty class member functions here

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

/////////////////////////////////////////////////////////////////////
//
// ESP I/O fns
    void Esp::printNeurons()
    {
        LOG(ERROR) << "ESP::printNeurons() is commented out" << endl;
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
        LOG(ERROR) << "ESP::loadSeedNet (char* filename) is commented out" << endl;
        /*  netType = SCDORDER;  //change this to read the type from the filename;
         numPops = bestNetwork->load(filename);
         printf("NUM_POPS %d\n", numPops);
         printf("GENE_SIZE_PRED_NW %d\n", GENE_SIZE_PRED_NW);
         */
    }

    void Esp::save(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey)
    {
        LOG(ERROR) << "ESP::save (char* filename) is commented out" << endl;
        /*
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

         LOG(INFO) << endl << "Saved to " << fname << endl << endl;*/
    }

    void Esp::findChampion()
    {
        LOG(ERROR) << "ESP::findChampion() is commented out" << endl;
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
        //fout_champfitness.close();
        LOG(INFO) << "Done with evolution";
        //exit(0);
    }

    void Esp::printStats()
    {
        LOG(ERROR) << "ESP::printStats() is commented out" << endl;
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
        LOG(ERROR) << "ESP::printStats() is commented out" << endl;
        return -1;
        /*
         int sp;

         // see which (if any) subpop can be removed from pred
         sp = team[pred]->lesion(Envt, team, num_of_predators, num_of_prey);
         return (removeSubPop(sp, pred, num_of_predators, num_of_prey));     // remove it.*/
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
// is it a number?
    int isnumber(char *str)
    {
        int i, res = 1;

        for (i = 0; i < (int) strlen(str); ++i)
            if (!isdigit(str[i]))
                res = 0;

        return res;
    }
}
