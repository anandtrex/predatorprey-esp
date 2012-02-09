// -----------------------------------------------------
// Chern Han's modifications to allow for team evolution or improvements
//
// total_hidden_neuron_populations changed to a vector <vector <subPops*> >
// Thus, total_hidden_neuron_populations[i][j] refers to Predator i's subpop j.
//
// numPops changed to a vector<int> corresponding to number of neurons
// in each predator.
//
// numPreds added to class ESP.
//
// genNetType has new argument to indicate number of neurons of the net
// 
// addSubPop has new argument to indicate which predator to add the new
// subpop to.
//
// A removeSubPop's arguments changed to (team, pred), since it calls
// Network::lesion which has to lesion pred's neurons and evaluate the
// resulting team.
//
// Network::lesion has new argument (team). It will attempt to lesion 
// neurons from the network on which its called, and then test team.
// Also, it does not update the fitness of the net/team since it does
// not actually change it. Thus removeSubpop does not change any
// fitness values.
//
// When Network::lesion lesions each neuron, it tests the net for 
// a number of trials instead of just once.
//
// In ESP::handleStagnation, first conditional changed to <= from ==
//
// evalNet is now (usually) called for a number of trials (global 
// variable evalTrials) instead of just once, to take into account the
// randomness of the domain.
//
// All code dealing with nets other than feedforward has been commented
// out (only feedforward nets will be used at this point).
//
// Environment::evalNet's argument has been changed to accept a team
// (vector<Network*>&).
//
// ES::newDeltaPhase has a new argument pred to tell it which predator
// to perform the delta phase on.
//
// subPop::~subPop does not call delete now, just clears the vector,
// because i think one isn't supposed to call delete on vector elements.
// Same in ESP::~ESP, Network::removeNeuron.
//
// To facilitate loading Esp from file, I have moved Esp::create
// into Esp's constructor
//
// In ESP::performEval, when checking to go to nextTask, bestNetwork's
// fitness is set to 0, so that in ESP::handleStagnation, overall_best_teams's
// fitness won't be compared to an easier task's bestNetwork's fitness
//
// -----------------------------------------------------

//////////////////////////////////////////////////////////////////////
//   Faustino Gomez  
// 
//   ESP C++ implementation.
//
//   This was code started from Daniel Polani's translation of 
//   Dave Moriarty's original SANE code from C to C++.
//   
//   
//   $Log: ESP.C,v $
//   Revision 2.9  2000/02/24 18:58:15  inaki
//   Naming convention is now uniform.  Delta chromosomes no longer represented
//   explicitly which simplified the interface to the 'create' fns.  
//   Mutation is now cauchy distributed noise.  Code more fully commented.
//
//   Revision 2.8  2000/02/04 04:10:57  inaki
//   Networks are now in separate file.  Class Network has been subclassed
//   into Feedforward, SRN, 2nd-Order Nets.  Net type can be selected
//   from the command line.  Added machinery for adding/removing
//   subpopulations; not connected yet.
//
//   Revision 2.7  1999/10/19 23:59:55  inaki
//   Eliminated all 'mallocs'.   Fixed bug in Delta-Coding.  In 2.6
//   Delta-Coding was not working correctly at all.  Problem with using
//   STL 'inner-product' algorithm. Track this down soon.
//
//   Revision 2.6  1999/09/21 03:29:19  inaki
//   New unix style command line interface.  Delta coding is now working.
//   Many vector operations (e.g. dot-products) now use STL algorithms.
//
//   Revision 2.5  1999/09/18 23:30:34  inaki
//   STL vectors for chromosomes
//
//   Revision 2.4  1999/09/18 00:07:39  inaki
//   Length of chromosomes is now set a run-time.  These will be made STL
//   vectors later.  Next is the Delta-Coding.
//
//   Revision 2.3  1999/09/17 23:57:09  inaki
//   Simple Recurrent Nets (SRNs) can now be evolved by setting RECURRENT
//   switch.
//
//   Revision 2.2  1999/09/17 23:26:48  inaki
//   Cleaned up the code a little more.  Eliminated most #defines and
//   unnecessary globals.  The size if the network input/ouput dimensions
//   is no set in the environment.  Next will be to add recurrency.
//
//   Revision 2.0  1999/09/17 20:29:54  inaki
//   Made all storage allocation dynamic.
//   Chromosomes are now real-value coded and connectivity is
//   not evolved.
//////////////////////////////////////////////////////////////////////////

//   My work begins here                inaki
//////////////////////////////////////////////////////////////////////////
//
//   Revision 1.9  1997/09/20 00:10:26  polani
//   Moved to Sane with Subpopulations. Snapshot V2_1.
//
//   Revision 1.8  1997/09/19 23:02:32  polani
//   This is the standard SANE approach.
//
//   Revision 1.7  1997/09/16 00:54:54  polani
//   It works now! It has been a bug in the selection of the 'init()'
//   function in State, which was a bit ambiguously reported by gcc.
//
//   The Experiment has relegated to the 'State' class, which can be
//   derived from to generate new test scenarios. Not everything is as
//   clean as I would like to have it - for instance the whole constant
//   setting stuff is pretty ugly yet - but I don't want to waste more time
//   on this right now, since the main aspects of the structure are pretty
//   clear and that was the important thing to me.
//
//   The next step will therefore be an attempt to introduce the Eugenic
//   approach into this framework.
//
//   Revision 1.6  1997/09/16 00:18:12  polani
//   Now also the experiment is a class, and now it does not work
//   anymore. Debugging is the next step.
//
//   Revision 1.5  1997/09/15 19:37:00  polani
//   Further objectization of code. Some more functions included as static
//   class members into neuron_pop.
//
//   Revision 1.4  1997/09/15 18:53:28  polani
//   More parts of code have been objectivized. Still produces the same
//   result with -O flag on, but as I noted, -O3 or no optimization
//   generate a different result with seed 1234. I always obtained 53953
//   evaluations for -O optimization. Now I don't quite trust anymore this
//   compiler or - possibly - this program.
//
//   Revision 1.3  1997/09/15 16:59:43  polani
//   Made structs to classes. Still works. *grin*
//
//   Revision 1.2  1997/09/15 16:47:48  polani
//   Slight modifications to the original. C++ization begins now.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <vector>
//#include <deque.h>
#include <algorithm>
#include <numeric>
#include "signal-handler.h"
#include "Environment.h"
#include "PredPrey.h"
#include "PredPreyST1.h"
#include "PredPreyST2.h"
#include "Network.h"
#include "Esp.h"
#include <fstream>
#include <sstream>
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <glog/logging.h>
#include <libconfig.h++>

int SIMU_TRIALS = 5;
int NUM_INPUTS_PRED_COMBINER;
//int NUM_INPUTS_PREY_COMBINER;
int NUM_OUTPUT_PRED_COMBINER;
//int NUM_OUTPUT_PREY_COMBINER;
int NUM_INPUTS;  // number of input units; includes recurrent input EVOLVE_PREY
//int NUM_INPUTS_PREY;  // number of input units; includes recurrent input EVOLVE_PREY
int NUM_OUTPUTS;  // number of output units EVOLVE_PREY
//int NUM_OUTPUTS_PREY;  // number of output units EVOLVE_PREY
int GENE_SIZE_PRED_NW;  // number of weights each neuron has (in & out) EVOLVE_PREY
//int GENE_SIZE_PREY_NW;  // number of weights each neuron has (in & out) EVOLVE_PREY
int GENE_SIZE_PRED_COMBINER_NW;  // number of weights each neuron has (in & out) EVOLVE_PREY
//int GENE_SIZE_PREY_COMBINER_NW;  // number of weights each neuron has (in & out) EVOLVE_PREY
int SKIP = 0;  // skip recombination; EVOLVE_PREY
int MIN = 0;  // 0 means higher fitness better, 1 otherwise
double MUT_RATE = 0.4;  // mutation rate in %  EVOLVE_PREY
double MUT_RATE_PREY = 0.4;  // mutation rate in %  EVOLVE_PREY
int STAG = 20;  // the length of the performance queue. EVOLVE_PREY
int PLOTTING = 0;

int TOTAL_EVALUATIONS;  //Total number of evaluations within a generation
int EVALTRIALS = 5;  // number of times to call evalNet
bool INCREMENTAL_LEARNING = false;
int BURST_MUTATE = 0;  //EVOLVE_PREY
int BURST_MUTATE_PREY = 0;  //EVOLVE_PREY

int IS_COMBINER_NW = 0;
int COMBINE = 1;

//int total_prey_networks_per_team = 2; //number of prey ******PADMINI //ADITYA

bool SHOW;  // when true, evalNet will display the actions of the pred
int delay_pred_evolution;  //EVOLVE_PREY
int prey_slow;  //EVOLVE_PREY
bool IS_PREY = true;

class neuron;

void reseed(int val);
double rndCauchy(double wtrange);

struct maximize_fit: public binary_function<neuron*, neuron*, bool>
{
    bool operator()(neuron *x, neuron *y)
    {
        return x->fitness > y->fitness;
    }
};

struct minimize_fit: public binary_function<neuron*, neuron*, bool>
{
    bool operator()(neuron *x, neuron *y)
    {
        return x->fitness < y->fitness;
    }
};

vector<vector<vector<Network*> > > Esp::hall_of_fame_pred;  //Static members need to be defined outside the class (besides their declaration inside the class)
//vector<vector<vector<Network*> > > Esp::hall_of_fame_prey;  //Static members need to be defined outside the class (besides their declaration inside the class)

//*************************Parameters Read from the Config file**********************
int start_generation;  // Specify the generation number from which we start the esp (if greater than one, then should have a corresponding .bin file to load from)
bool if_display;  // 1 - Simulation display is ON, 0 - OFF
int display_frequency;  // Specify after how many generations you want display
int maxGenerations;  // Maximum number of generations
bool if_shared_fitness_predator;  // 1 - Fitness sharing in predator team, 0 - individual fitness in predator team
bool pred_communication;  // 1 - Direct communication in predator teams, 0 - No direct communication in predator teams
bool pred_messaging;  //  1 - Broadcast Messaging in predator teams, 0 - No Broadcast Messaging in predator teams
bool prey_reappears;  //  1 - Each Prey reappears once it gets killed, 0 - Prey does not reappear
bool if_evolve_prey;  // 1 - Evolve prey, 0 - Do not evolve prey (fixed behavior prey)
vector<int> reward_prey_team;  // Reward for prey team 0 (If there are more prey teams, then add more lines below this to specify the prey team reward size)
vector<double> prey_move_probability;  // Prey speed relative to the predator speed (If there are more prey teams, then add more lines below this to specify the prey team speed)
bool prey_communication;  // 1 - Direct communication in prey teams, 0 - No direct communication in prey teams
bool prey_messaging;  //  1 - Broadcast Messaging in prey teams, 0 - No Broadcast Messaging in prey teams
//*************************END Parameters Read from the Config file**********************

string file_prefix;

//////////////////////////////////////////////////////////////////////
//
// Procedures
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// Neuron 
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//neuron constructor
neuron::neuron()
{
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            for (int i = 0; i < GENE_SIZE_PRED_COMBINER_NW; ++i)
                weight.push_back(0.0);  //fill weight vector with zeros
        } else {
            for (int i = 0; i < GENE_SIZE_PRED_NW; ++i)
                weight.push_back(0.0);  //fill weight vector with zeros
        }
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        if (IS_COMBINER_NW == 1) {
//            for (int i = 0; i < GENE_SIZE_PREY_COMBINER_NW; ++i)
//                weight.push_back(0.0);  //fill weight vector with zeros
//        } else {
//            for (int i = 0; i < GENE_SIZE_PREY_NW; ++i)
//                weight.push_back(0.0);  //fill weight vector with zeros
//        }
    }
}

//---------------------------------------------------------------------
//destructor
neuron::~neuron()
{
}

//---------------------------------------------------------------------
// perturb the weights of a neuron. Used to search in a neighborhood
//   around some neuron (best).
void neuron::perturb(const neuron &n, double(*randFn)(double), double coeff)
{
    if (!IS_PREY) {
        for (int i = 0; i < GENE_SIZE_PRED_NW; ++i)
            weight[i] = n.weight[i] + (randFn)(coeff);
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        for (int i = 0; i < GENE_SIZE_PREY_NW; ++i)
//            weight[i] = n.weight[i] + (randFn)(coeff);
    }
}

void neuron::perturb(const neuron &n)
{
    perturb(n, rndCauchy, 0.3);
}

//---------------------------------------------------------------------
// same as above but called on self and returns new neuron
neuron* neuron::perturb()
{
    neuron *n = new neuron();
    if (!IS_PREY) {
        for (int i = 0; i < GENE_SIZE_PRED_NW; ++i)
            n->weight[i] = weight[i] + rndCauchy(0.3);
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        for (int i = 0; i < GENE_SIZE_PREY_NW; ++i)
//            n->weight[i] = weight[i] + rndCauchy(0.3);
    }

    return n;
}

//---------------------------------------------------------------------
// neuron assignment op
neuron neuron::operator=(const neuron &n)
{
    weight = n.weight;
    return *this;
}

//---------------------------------------------------------------------
// create a new set of random weights
void neuron::create()
{
    int i;
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            for (i = 0; i < GENE_SIZE_PRED_COMBINER_NW; ++i)
                weight[i] = (drand48() * 12.0) - 6.0;
        } else {
            for (i = 0; i < GENE_SIZE_PRED_NW; ++i)
                weight[i] = (drand48() * 12.0) - 6.0;
        }
    } else {
        LOG(FATAL) << "Shouldn't be here";
//        if (IS_COMBINER_NW == 1) {
//            for (i = 0; i < GENE_SIZE_PREY_COMBINER_NW; ++i)
//                weight[i] = (drand48() * 12.0) - 6.0;
//        } else {
//            for (i = 0; i < GENE_SIZE_PREY_NW; ++i)
//                weight[i] = (drand48() * 12.0) - 6.0;
//        }
    }

}

//////////////////////////////////////////////////////////////////////
//
// SubPop
//
//////////////////////////////////////////////////////////////////////

// subPop constructor
subPop::subPop(int size) :
        pop(size), pop2(0)  // What is happening here ??
{
    evolvable = true;
    numNeurons = size;
    numNeurons2 = 0;
    numBreed = (int) numNeurons / 4;
    // We leave subPop::create to actually create the neurons.
    // This way neurons are not created when we initially
    // create a Network
}

//---------------------------------------------------------------------
// destructor
subPop::~subPop()
{

    // Delete pointer elements of vector pop one by one (freeing up memory only - not deleting pointers vector)
    for (int i = 0; i < numNeurons; ++i)
        delete pop[i];

    //Clear after freeing up the memory in the previous step
    pop.clear();
}

//---------------------------------------------------------------------
// create the neurons, initial their weights, and put them in the subpop.
void subPop::create()
{
    if (evolvable)
        for (int i = 0; i < numNeurons; ++i) {
            pop[i] = new neuron();
            pop[i]->create();
        }
}

//---------------------------------------------------------------------
// used to perform "delta-coding" like 
void subPop::deltify(neuron *bestNeuron)
{

    for (int i = 0; i < numNeurons; ++i) {
        pop[i]->perturb(*bestNeuron);  // make each neuron a perturbation of the
                                       // neuron in the best network that
                                       // corresponds to that subpop
        //delete pop[i];
        //if(i > (numNeurons/2))
        //  pop[i] = bestNeuron->perturb();
        //else
        //pop[i] = tmp.perturb();
    }
}

//----------------------------------------------------------------------
// reset fitness and test vals of all neurons
void subPop::evalReset()
{
    for (int i = 0; i < numNeurons; ++i) {
        pop[i]->fitness = 0;
        pop[i]->tests = 0;
    }
}

//----------------------------------------------------------------------
// select a neuron at random
neuron *subPop::selectNeuron()
{
    return pop[lrand48() % numNeurons];
}

//----------------------------------------------------------------------
// normalize the neuron fitnesses 
void subPop::average()
{
    for (int i = 0; i < numNeurons; ++i) {
        if (pop[i]->tests)
            pop[i]->fitness = pop[i]->fitness / pop[i]->tests;
        else
            pop[i]->fitness = 0;

    }
}

//----------------------------------------------------------------------
// sort the neurons in each subpop using quicksort.
void subPop::qsortNeurons()
{
    if (MIN)
        sort(pop.begin(), pop.end(), minimize_fit());
    else
        sort(pop.begin(), pop.end(), maximize_fit());
}

//----------------------------------------------------------------------
// recombine neurons with members of their subpop using crossover.
void subPop::recombine()
{
    int i = 0;
    for (; i < numBreed; ++i) {
        crossover(pop[i]->weight, pop[findMate(i)]->weight, pop[numNeurons - (1 + i * 2)]->weight,
                pop[numNeurons - (2 + i * 2)]->weight);
    }
}

//----------------------------------------------------------------------
// recombine neurons with members of their subpop using crossover. EVOLVE_PREY
void subPop::recombine_hall_of_fame(int pred_or_prey_team, int pred_or_prey_number, int pop_number)
{
    int i = 0;
    for (; i < numBreed - 5; ++i) {
        crossover(pop[i]->weight, pop[findMate(i)]->weight, pop[numNeurons - (1 + i * 2)]->weight,
                pop[numNeurons - (2 + i * 2)]->weight);
    }

    if (!IS_PREY) {
        for (; i < numBreed; ++i) {
            crossover(
                    pop[i]->weight,
                    Esp::hall_of_fame_pred[drand48() * (Esp::hall_of_fame_pred.size() - 1)][pred_or_prey_team][pred_or_prey_number]->pop[pop_number]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight, pop[numNeurons - (2 + i * 2)]->weight);
        }
    } else {
        LOG(FATAL) << "Not supposed to be here!";
//        for (; i < numBreed; ++i) {
//            crossover(
//                    pop[i]->weight,
//                    Esp::hall_of_fame_prey[drand48() * (Esp::hall_of_fame_prey.size() - 1)][pred_or_prey_team][pred_or_prey_number]->pop[pop_number]->weight,
//                    pop[numNeurons - (1 + i * 2)]->weight, pop[numNeurons - (2 + i * 2)]->weight);
//        }
    }
}

//----------------------------------------------------------------------
// mutate some of the neurons with cauchy noise.
void subPop::mutate()
{
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            for (int i = numBreed * 2; i < numNeurons; ++i)
                if (drand48() < MUT_RATE)
                    pop[i]->weight[lrand48() % GENE_SIZE_PRED_COMBINER_NW] += rndCauchy(0.3);
        } else {
            for (int i = numBreed * 2; i < numNeurons; ++i)
                if (drand48() < MUT_RATE)
                    pop[i]->weight[lrand48() % GENE_SIZE_PRED_NW] += rndCauchy(0.3);
        }
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        if (IS_COMBINER_NW == 1) {
//            for (int i = numBreed * 2; i < numNeurons; ++i)
//                if (drand48() < MUT_RATE_PREY)
//                    pop[i]->weight[lrand48() % GENE_SIZE_PREY_COMBINER_NW] += rndCauchy(0.3);
//        } else {
//            for (int i = numBreed * 2; i < numNeurons; ++i)
//                if (drand48() < MUT_RATE_PREY)
//                    pop[i]->weight[lrand48() % GENE_SIZE_PREY_NW] += rndCauchy(0.3);
//        }
    }
}

//----------------------------------------------------------------------
// mutate all of the neurons with cauchy noise.
void subPop::burstMutate()
{
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            for (int i = 0; i < numNeurons; ++i)
                if (drand48() < MUT_RATE)
                    pop[i]->weight[lrand48() % GENE_SIZE_PRED_COMBINER_NW] += rndCauchy(0.3);
        } else {
            for (int i = 0; i < numNeurons; ++i)
                if (drand48() < MUT_RATE)
                    pop[i]->weight[lrand48() % GENE_SIZE_PRED_NW] += rndCauchy(0.3);
        }
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        if (IS_COMBINER_NW == 1) {
//            for (int i = 0; i < numNeurons; ++i)
//                if (drand48() < MUT_RATE_PREY)
//                    pop[i]->weight[lrand48() % GENE_SIZE_PREY_COMBINER_NW] += rndCauchy(0.3);
//        } else {
//            for (int i = 0; i < numNeurons; ++i)
//                if (drand48() < MUT_RATE_PREY)
//                    pop[i]->weight[lrand48() % GENE_SIZE_PREY_NW] += rndCauchy(0.3);
//        }
    }
}

//----------------------------------------------------------------------
// randomly find a mate in the same subpop.
int subPop::findMate(int num)
{
    return (lrand48() % numBreed);
//  if (num) {
//    return (lrand48()%numBreed);
//  }
//  else  {
//    return (lrand48()%num);
//  }
}

//----------------------------------------------------------------------
// 1-point crossover
void subPop::crossover(const vector<double> &parent1, const vector<double> &parent2,
        vector<double> &child1, vector<double> &child2)
{
    int cross1;
    //find crossover point
    if (!IS_PREY) {
        if (IS_COMBINER_NW == 1) {
            cross1 = lrand48() % GENE_SIZE_PRED_COMBINER_NW;
        } else {
            cross1 = lrand48() % GENE_SIZE_PRED_NW;
        }
    } else {
        LOG(FATAL) << "Not supposed to be here!";
//        if (IS_COMBINER_NW == 1) {
//            cross1 = lrand48() % GENE_SIZE_PREY_COMBINER_NW;
//        } else {
//            cross1 = lrand48() % GENE_SIZE_PREY_NW;
//        }
    }
    child1 = parent2;
    child2 = parent1;
    swap_ranges(child1.begin(), child1.begin() + cross1, child2.begin());
}

//////////////////////////////////////////////////////////////////////
//
// Esp
//
//////////////////////////////////////////////////////////////////////

// Esp constructor -- constructs a new ESP
Esp::Esp(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey,
        int num_teams_hunters, int num_of_hunters, int nPops /* num hidden neurons */, int nSize,
        Environment &e, int netTp = FF) :
        Envt(e)
{
    hall_of_fame_pred = vector<vector<vector<Network*> > >();
    fout_champfitness.open("champion_fitness.log");
        if(!fout_champfitness.is_open()){
            LOG(FATAL) << "File not opened";
        } else {
            DLOG(INFO) << "Opened file";
        }

    this->sub_hall_of_fame_pred = vector<vector<vector<vector<Network*> > > >();

    generation = 0;  // start at generation 0

    total_predator_networks_per_team = num_of_predators * (num_of_prey * num_teams_prey /* total no. of prey */
    + num_of_hunters * num_teams_hunters /* total num of hunters */
    + num_of_predators * (num_teams_predator - 1) /* total number of predators in the other teams */
    ) + COMBINE * num_of_predators;  //Total Predator Networks in a team

    LOG(INFO) << "total_predator_networks_per_team is " << total_predator_networks_per_team;

//    total_prey_networks_per_team = num_of_prey * (num_of_predators * num_teams_predator)
//            + COMBINE * num_of_prey;  //Total Prey Networks in a team

    if (pred_communication == true) {
        total_predator_networks_per_team = total_predator_networks_per_team
                + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator direct communication
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
    setupNetDimensions(num_of_predators, num_of_prey);  // calculate the input layer size and gene size

    vector<Network*> temp_overall_best_teams;
//    vector<Network*> temp_overall_best_teams_prey;

    IS_PREY = false;
    for (int j = 0; j < num_teams_predator; j++) {
        IS_COMBINER_NW = 0;
        for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        IS_COMBINER_NW = 1;
        for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                i < total_predator_networks_per_team; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        overall_best_teams_sum_individual_fitness.push_back(0.0);
        overall_best_teams.push_back(temp_overall_best_teams);
        temp_overall_best_teams.clear();
    }

    IS_PREY = true;
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

    IS_PREY = false;
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

    IS_PREY = true;
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

    create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);  // create the neurons.
    LOG(INFO) << " ESP Constructor Done " << endl;

}

// Esp constructor -- constructs a new ESP for a combination of subtasks
Esp::Esp(vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred, int num_of_predators,
        int num_of_prey, int num_teams_predator, int num_teams_prey, int num_teams_hunters,
        int num_of_hunters, int nPops /* num hidden neurons */, int nSize, Environment &e,
        int netTp = FF) :
        Envt(e)
{
    hall_of_fame_pred = vector<vector<vector<Network*> > >();
    this->sub_hall_of_fame_pred = sub_hall_of_fame_pred;

    fout_champfitness.open("champion_fitness.log");
            if(!fout_champfitness.is_open()){
                LOG(FATAL) << "File not opened";
            } else {
                DLOG(INFO) << "Opened file";
            }

    generation = 0;  // start at generation 0

    total_predator_networks_per_team = num_of_predators * (num_of_prey * num_teams_prey /* total no. of prey */
    + num_of_hunters * num_teams_hunters /* total num of hunters */
    + num_of_predators * (num_teams_predator - 1) /* total number of predators in the other teams */
    ) + COMBINE * num_of_predators;  //Total Predator Networks in a team

//    total_prey_networks_per_team = num_of_prey * (num_of_predators * num_teams_predator)
//            + COMBINE * num_of_prey;  //Total Prey Networks in a team

    if (pred_communication == true) {
        total_predator_networks_per_team = total_predator_networks_per_team
                + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator direct communication
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
    setupNetDimensions(num_of_predators, num_of_prey);  // calculate the input layer size and gene size

    vector<Network*> temp_overall_best_teams;
    vector<Network*> temp_overall_best_teams_prey;

    IS_PREY = false;
    for (int j = 0; j < num_teams_predator; j++) {
        IS_COMBINER_NW = 0;
        for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        IS_COMBINER_NW = 1;
        for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                i < total_predator_networks_per_team; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        overall_best_teams_sum_individual_fitness.push_back(0.0);
        overall_best_teams.push_back(temp_overall_best_teams);
        temp_overall_best_teams.clear();
    }

    IS_PREY = true;
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

    IS_PREY = false;
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

    IS_PREY = true;
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

    create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);  // create the neurons.
    LOG(INFO) << " ESP Constructor Done " << endl;

}

// ----------------------------------------------------------------------
//New Function for Loading weights from file
Esp::Esp(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
        int num_teams_prey, int num_teams_hunters, int num_of_hunters, int nPops, int nSize,
        Environment &e, int netTp = FF) :
        Envt(e)

{

    hall_of_fame_pred = vector<vector<vector<Network*> > >();

    fout_champfitness.open("champion_fitness.log");
            if(!fout_champfitness.is_open()){
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
                    + num_of_predators * (num_teams_predator - 1)) + COMBINE * num_of_predators;  //Total Predator Networks in a team

//    total_prey_networks_per_team = num_of_prey * (num_of_predators * num_teams_predator)
//            + COMBINE * num_of_prey;  //Total Prey Networks in a team

    if (pred_communication == true) {
        total_predator_networks_per_team = total_predator_networks_per_team
                + num_of_predators * (num_of_predators - 1);  //Extra NNs for predator direct communication
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

    for (int i = 0; i < total_predator_networks_per_team; i++)
        num_hidden_neurons.push_back(nPops);  // set the number of subpops

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
    setupNetDimensions(num_of_predators, num_of_prey);  // calculate the input layer size and gene size

    vector<Network*> temp_overall_best_teams;
    vector<Network*> temp_overall_best_teams_prey;

    IS_PREY = false;
    for (int j = 0; j < num_teams_predator; j++) {
        IS_COMBINER_NW = 0;
        for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        IS_COMBINER_NW = 1;
        for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                i < total_predator_networks_per_team; i++) {
            temp_overall_best_teams.push_back(generateNetwork(netType, num_hidden_neurons[i]));  // generate new net
            temp_overall_best_teams[i]->create();  // create net
        }
        overall_best_teams_sum_individual_fitness.push_back(0.0);
        overall_best_teams.push_back(temp_overall_best_teams);
        temp_overall_best_teams.clear();
    }

    IS_PREY = true;
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
    vector<vector<subPop*> > total_hidden_neuron_populations_per_team;

    IS_PREY = false;
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

    IS_PREY = true;
    // construct total_predator_networks_per_team number of subpop vectors
//    for (int k = 0; k < num_teams_prey; k++) {
//        total_hidden_neuron_populations_per_team.clear();
//        for (int i = 0; i < total_prey_networks_per_team; i++) {
//            temp = new vector<subPop*>;
//            for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                temp->push_back(new subPop(nSize));  //Creating a (pointer to the) population of 100 neurons for each hidden neuron
//
//            }
//            total_hidden_neuron_populations_per_team.push_back(*temp);  //Should we clear/delete temp or not?. What is this line doing?
//        }
//        hidden_neuron_populations_prey.push_back(total_hidden_neuron_populations_per_team);
//    }

    create(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);  // create the neurons.

    // read neurons' weights from file, and set them
    for (int p = 0; p < num_teams_predator; p++)
        for (int i = 0; i < total_predator_networks_per_team - (COMBINE * num_of_predators); i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++)
                for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                    for (int m = 0; m < GENE_SIZE_PRED_NW; m++) {
                        fread(&tempDouble, sizeof(double), 1, fptr);
                        hidden_neuron_populations[p][i][j]->pop[k]->weight[m] = tempDouble;  // weights
                    }
//    for (int p = 0; p < num_teams_prey; p++)
//        for (int i = 0; i < total_prey_networks_per_team - (COMBINE * num_of_prey); i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                for (int k = 0; k < hidden_neuron_populations_prey[p][i][j]->numNeurons; k++)
//                    for (int m = 0; m < GENE_SIZE_PREY_NW; m++) {
//                        fread(&tempDouble, sizeof(double), 1, fptr);
//                        hidden_neuron_populations_prey[p][i][j]->pop[k]->weight[m] = tempDouble;  // weights
//                    }

    //if (COMBINE == 1) {
    for (int p = 0; p < num_teams_predator; p++)
        for (int i = total_predator_networks_per_team - (COMBINE * num_of_predators);
                i < total_predator_networks_per_team; i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++)
                for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                    for (int m = 0; m < GENE_SIZE_PRED_COMBINER_NW; m++) {
                        fread(&tempDouble, sizeof(double), 1, fptr);
                        hidden_neuron_populations[p][i][j]->pop[k]->weight[m] = tempDouble;  // weights
                    }

//    for (int p = 0; p < num_teams_prey; p++)
//        for (int i = total_prey_networks_per_team - (COMBINE * num_of_prey);
//                i < total_prey_networks_per_team; i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                for (int k = 0; k < hidden_neuron_populations_prey[p][i][j]->numNeurons; k++)
//                    for (int m = 0; m < GENE_SIZE_PREY_COMBINER_NW; m++) {
//                        fread(&tempDouble, sizeof(double), 1, fptr);
//                        hidden_neuron_populations_prey[p][i][j]->pop[k]->weight[m] = tempDouble;  // weights
//                    }
    //}

    fclose(fptr);

    LOG(INFO) << "Loading from file : each predator is a separate neural net" << endl
            << "Total Number of Predator networks: "
            << total_predator_networks_per_team
            << endl
//            << "Total Number of Prey Networks:" << total_prey_networks_per_team << endl
            << "Initial total subpops: " << total_predator_networks_per_team * nPops << endl
            << "Incremental learning: " << INCREMENTAL_LEARNING << endl << "Burst mutation: "
            << BURST_MUTATE << endl << "EvalTrials: " << EVALTRIALS << endl << "STAG: " << STAG
            << endl << endl;

}

// destructor
Esp::~Esp()
{
    //Freeing up memory pointed to by subpop pointers (corresponding to the new statements above)

    int size = hidden_neuron_populations.size();  //This gives num_teams_predator value

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
                / (total_predator_networks_per_team - (COMBINE * num_of_predators)) + NUM_OUTPUTS;  // Gene Size per hidden neuron (excepting combiner)
//        if(((total_prey_networks_per_team - (COMBINE * num_of_prey)) + NUM_OUTPUTS_PREY) == 0){
//            GENE_SIZE_PREY_NW = 0;
//        }
//        else {
//        GENE_SIZE_PREY_NW = (NUM_INPUTS_PREY * num_of_prey)
//                / (total_prey_networks_per_team - (COMBINE * num_of_prey)) + NUM_OUTPUTS_PREY;
//        }
        GENE_SIZE_PRED_COMBINER_NW = (NUM_INPUTS_PRED_COMBINER + NUM_OUTPUT_PRED_COMBINER);  // Gene Size per hidden neuron of a combiner network
//        GENE_SIZE_PREY_COMBINER_NW = (NUM_INPUTS_PREY_COMBINER + NUM_OUTPUT_PREY_COMBINER);

        LOG(INFO) << "Gene size per hidden neuron of non-combiner networks: " << " predator:: "
                << GENE_SIZE_PRED_NW;  //<< "  prey:: " << GENE_SIZE_PREY_NW << endl;
        LOG(INFO) << "Gene size per hidden neuron of combiner networks: " << " predator:: "
                << GENE_SIZE_PRED_COMBINER_NW;  // << "  prey:: " << GENE_SIZE_PREY_COMBINER_NW << endl;
//        LOG(INFO) << "Total networks in a team  " << " predators:: " << total_predator_networks_per_team
//                << "  prey:: " << total_prey_networks_per_team << endl;
        LOG(INFO) << "FEED FORWARD\n";
        break;

    }
}

//----------------------------------------------------------------------
// create the subpopulations of neurons, initializing them to 
//   random weights
void Esp::create(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey)
{
    IS_PREY = false;
    IS_COMBINER_NW = 0;
    for (int k = 0; k < num_teams_predator; k++) {
        for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++) {
                hidden_neuron_populations[k][i][j]->create();  // create each subpop
            }
    }

    IS_COMBINER_NW = 1;
    for (int k = 0; k < num_teams_predator; k++) {
        for (int i = total_predator_networks_per_team - COMBINE * num_of_predators;
                i < total_predator_networks_per_team; i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++) {
                hidden_neuron_populations[k][i][j]->create();  // create each subpop
            }
    }

    IS_PREY = true;
    IS_COMBINER_NW = 0;
//    for (int k = 0; k < num_teams_prey; k++) {
//        for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                hidden_neuron_populations_prey[k][i][j]->create();  // create each subpop
//            }
//    }

    IS_COMBINER_NW = 1;
//    for (int k = 0; k < num_teams_prey; k++) {
//        for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
//                i < total_prey_networks_per_team; i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                hidden_neuron_populations_prey[k][i][j]->create();  // create each subpop
//            }
//    }

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
                num_teams_hunters, num_of_hunters);  //evaluate neurons

        IS_PREY = false;
        IS_COMBINER_NW = 0;

        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators;
                    i++) {
                for (int j = 0; j < num_hidden_neurons[i]; j++) {
                    hidden_neuron_populations[k][i][j]->qsortNeurons();
                    hidden_neuron_populations[k][i][j]->recombine_hall_of_fame(k, i, j);  //(Hall of fame probably doesn't work for Competing agents in a single team)
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
                    hidden_neuron_populations[k][i][j]->recombine_hall_of_fame(k, i, j);  //(Hall of fame probably doesn't work for Competing agents in a single team)
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

        IS_PREY = true;
        IS_COMBINER_NW = 0;
//        for (int k = 0; k < num_teams_prey; k++) {
//            for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++) {
//                for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                    hidden_neuron_populations_prey[k][i][j]->qsortNeurons();
//                    hidden_neuron_populations_prey[k][i][j]->recombine_hall_of_fame(k, i, j);  //(Hall of fame probably doesn't work for Competing agents in a single team)
//                }
//            }
//        }

        // mutate population
//        for (int k = 0; k < num_teams_prey; k++) {
//            for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++) {
//                for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                    hidden_neuron_populations_prey[k][i][j]->mutate();
//                }
//            }
//        }

        IS_COMBINER_NW = 1;
//        for (int k = 0; k < num_teams_prey; k++) {
//            for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
//                    i < total_prey_networks_per_team; i++) {
//                for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                    hidden_neuron_populations_prey[k][i][j]->qsortNeurons();
//                    hidden_neuron_populations_prey[k][i][j]->recombine_hall_of_fame(k, i, j);  //(Hall of fame probably doesn't work for Competing agents in a single team)
//                }
//            }
//        }

        // mutate population
//        for (int k = 0; k < num_teams_prey; k++) {
//            for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
//                    i < total_prey_networks_per_team; i++) {
//                for (int j = 0; j < num_hidden_neurons[0]; j++) {
//                    hidden_neuron_populations_prey[k][i][j]->mutate();
//                }
//            }
//        }

    }  //end while

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

void Esp::evalPop(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey,
        int num_teams_hunters, int num_of_hunters)
{

    evalReset(num_teams_predator, num_teams_prey);  // reset fitness and test values of each neuron
    performEval(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
            num_teams_hunters, num_of_hunters);  // evaluate the networks
    average(num_teams_predator, num_teams_prey);  // normalize neuron fitnesses

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

//    for (int k = 0; k < num_teams_prey; k++) {
//        for (int i = 0; i < total_prey_networks_per_team; i++) {
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                hidden_neuron_populations_prey[k][i][j]->evalReset();
//        }
//    }
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
Network* Esp::generateNetwork(int networkType, int num_hidden_neurons)
{
    switch (networkType) {
    case FF:
        return (new FeedForwardNetwork(num_hidden_neurons));
        //break;
        /*  case FR :
         return( new FullyRecurrentNetwork(nPops) ); break;
         case SRN :
         return( new SimpleRecurrentNetwork(nPops) ); break;
         case SCDORDER :
         return( new SecondOrderRecurrentNetwork(nPops) );
         */

    default:
        return (new FeedForwardNetwork(num_hidden_neurons));
        //break;
    }
}

//--------------------------------------------------------------------
// evaluate the networks on the task
// TODO Add appropriate addition of sub-task networks
void Esp::performEval(int num_of_predators, int num_of_prey, int num_teams_predator,
        int num_teams_prey, int num_teams_hunters, int num_of_hunters)
{
    //cout << " Starting Perform Eval " <<endl;

    static int evaluations = 0;
    vector<Network*> temp_team;  // vector of current networks
//    vector<Network*> temp_team_prey;  // vector of current networks
    vector<Network*> temp_bestTeam;  // vector of best networks
//    vector<Network*> temp_bestTeam_prey;  // vector of best networks

    vector<vector<Network*> > current_teams_pred;  // vector of current pred teams
//    vector<vector<Network*> > current_teams_prey;  // vector of current prey teams
    vector<vector<Network*> > generation_best_teams;  // vector of best pred teams in a given generation
//    vector<vector<Network*> > generation_best_teams_prey;  // vector of best prey teams in a given generation

    vector<double> temp_individual_fitness;  //Temporary variable for storing predator/prey team fitness
    vector<vector<double> > temp_team_fitness;  //Temporary variable for storing predator/prey team fitness
    vector<vector<vector<double> > > predator_prey_teams_fitness;  //Vector for all teams fitness together after averaging over 6 trials
    vector<vector<vector<double> > > average_predator_prey_teams_fitness;  //Vector for all teams fitness together after averaging over 6000 trials
    vector<vector<vector<double> > > teams_fitness;  //temporary variable for all teams fitness together per trial

    vector<double> current_teams_sum_individual_fitness;  //Sum of individual network fitnesses
    vector<double> generation_best_teams_sum_individual_fitness;  //Sum of individual network fitnesses
//    vector<double> current_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses
//    vector<double> generation_best_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses

    delay_pred_evolution = 1;
    if (generation < 1) {  //EVOLVE_PREY
        prey_slow = 1;  //vary prey speed
    } else {
        prey_slow = 0;  //vary prey speed
    }

    IS_PREY = false;
    for (int k = 0; k < num_teams_predator; k++) {
        IS_COMBINER_NW = 0;
        temp_team.clear();
        temp_bestTeam.clear();
        for (int i = 0; i < total_predator_networks_per_team - COMBINE * num_of_predators; i++) {
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

    IS_PREY = true;
//    for (int k = 0; k < num_teams_prey; k++) {
//        IS_COMBINER_NW = 0;
//        temp_team_prey.clear();
//        temp_bestTeam_prey.clear();
//        for (int i = 0; i < total_prey_networks_per_team - COMBINE * num_of_prey; i++) {
//            temp_team_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));
//            temp_bestTeam_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));
//            temp_bestTeam_prey[i]->create();
//        }
//
//        IS_COMBINER_NW = 1;
//        for (int i = total_prey_networks_per_team - COMBINE * num_of_prey;
//                i < total_prey_networks_per_team; i++) {
//            temp_team_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));
//            temp_bestTeam_prey.push_back(generateNetwork(netType, num_hidden_neurons[0]));
//            temp_bestTeam_prey[i]->create();
//        }
//        current_teams_prey.push_back(temp_team_prey);
//        generation_best_teams_prey.push_back(temp_bestTeam_prey);
//    }

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

        //cout << " PerformEval:: Starting with numTrials" << i << " Pred " << total_hidden_neuron_populations.size()
        //     << "    " << total_hidden_neuron_populations[0].size()
        //     << "    " << total_hidden_neuron_populations[0][0].size()
        //     << "    Prey " << total_hidden_neuron_populations_prey.size()
        //     << endl ;
        // find random subpopulation (only for current team)
        IS_PREY = false;

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
                for (i = 0; i < sub_total_predator_networks_per_team;  //num_of_predators * num_teams_prey * num_of_prey /*actually, number of prey networks. Should be num_teams_prey * num_of_prey*/;
                        i++) {
                    if (i < num_of_predators * num_teams_prey * num_of_prey) {
                        current_teams_pred[j][i]->setNetwork(
                                sub_hall_of_fame_pred[0][sub_hall_of_fame_pred_size - 1][j][i]);
                        current_teams_pred[j][i]->incrementTests();
                    } else {
                        for (int k = 0; k < num_of_predators * num_teams_prey * num_of_prey; k++) {
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
                for (k = i; k < i + sub_total_predator_networks_per_team;  //num_of_predators * num_teams_hunters * num_of_hunters /*actually, number of hunter networks. Should be num_teams_hunter * num_of_hunter*/;
                        k++) {
                    if (k < i + num_of_predators * num_teams_hunters * num_of_hunters) {
                        current_teams_pred[j][k]->setNetwork(
                                sub_hall_of_fame_pred[1][sub_hall_of_fame_pred_size - 1][j][k - i]);
                        current_teams_pred[j][k]->incrementTests();
                    } else {
                        for (int l = 0; l < num_of_predators * num_teams_hunters * num_of_hunters;
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

        } else {  // If there are no existing subnetworks already evolved, set neurons for all the hidden layers (including combiner network)

            for (int p = 0; p < num_teams_predator; p++) {
                for (int j = 0; j < total_predator_networks_per_team; j++) {
                    for (int k = 0; k < num_hidden_neurons[j]; k++) {
                        current_teams_pred[p][j]->setNeuron(
                                hidden_neuron_populations[p][j][k]->selectNeuron(), k);
                    }
                    current_teams_pred[p][j]->incrementTests();  //Counting the number of trials for a neuron (to compute average later)
                }
            }
        }

        IS_PREY = true;
//        for (int p = 0; p < num_teams_prey; p++) {
//            for (int j = 0; j < total_prey_networks_per_team; j++) {
//                for (int k = 0; k < num_hidden_neurons[0]; k++) {
//                    current_teams_prey[p][j]->setNeuron(
//                            hidden_neuron_populations_prey[p][j][k]->selectNeuron(), k);
//                }
//                current_teams_prey[p][j]->incrementTests();  //Counting the number of trials for a neuron (to compute average later)
//            }
//        }

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
//        generation_best_teams_prey_sum_individual_fitness.clear();

//        for (int p = 0; p < num_teams_prey; p++) {
//            for (int q = 0; q < num_of_prey; q++) {
//                temp_individual_fitness.push_back(0.0);
//            }
//            temp_team_fitness.push_back(temp_individual_fitness);
//            temp_individual_fitness.clear();
//            current_teams_prey_sum_individual_fitness.push_back(0.0);
//            generation_best_teams_prey_sum_individual_fitness.push_back(0.0);
//        }
//        predator_prey_teams_fitness.push_back(temp_team_fitness);
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
                for (int r = 0; r < num_of_predators; r++) {  //Individual predator fitness is used in case of competing predators
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
                            predator_prey_teams_fitness[0][q][r];  // distribute each predator's fitness to constituent networks
                    current_teams_pred[q][r
                            * (total_predator_networks_per_team / num_of_predators - 1) + j]->addFitness();  // add network fitness to its neurons
                }

                //Following two lines are for the combiner network
                current_teams_pred[q][(total_predator_networks_per_team - num_of_predators) + r]->fitness =
                        predator_prey_teams_fitness[0][q][r];  // distribute each predator's fitness to constituent networks
                current_teams_pred[q][(total_predator_networks_per_team - num_of_predators) + r]->addFitness();  // add network fitness to its neurons
            }

            //Computing Generation Best - current team with highest total fitness.
            //Note: individual predators in a team can have different fitness in
            //case of competition. However, all networks in a individual have same fitness
            for (int r = 0; r < num_of_predators; r++) {
                current_teams_sum_individual_fitness[q] =
                        current_teams_sum_individual_fitness[q]
                                + current_teams_pred[q][r
                                        * (total_predator_networks_per_team / num_of_predators - 1)]->fitness;
            }
            current_teams_sum_individual_fitness[q] /= num_of_predators;  //Averaging over team

            // if team outperforms bestTeam, set bestTeam = team (Note: all networks in a individual have same fitness)
            if (current_teams_sum_individual_fitness[q]
                    > generation_best_teams_sum_individual_fitness[q]) {
                //cout << " Replacing Best Team " <<endl;
                generation_best_teams_sum_individual_fitness[q] =
                        current_teams_sum_individual_fitness[q];
                for (int j = 0; j < total_predator_networks_per_team; j++) {
                    generation_best_teams[q][j]->setNetwork(current_teams_pred[q][j]);
                    generation_best_teams[q][j]->fitness = current_teams_pred[q][j]->fitness;
                }
            }
            //LOG(INFO) << overall_fitness << " " << generation << "\n";
        }


        //Prey - Distribute fitness to each contributing neurons
//        for (int q = 0; q < num_teams_prey; q++) {
//
//            for (int r = 0; r < num_of_prey; r++) {
//                predator_prey_teams_fitness[1][q][r] /= EVALTRIALS;
//                average_predator_prey_teams_fitness[1][q][r] +=
//                        predator_prey_teams_fitness[1][q][r];
//
//                for (int j = 0; j < (total_prey_networks_per_team / num_of_prey - 1); j++) {
//                    current_teams_prey[q][r * (total_prey_networks_per_team / num_of_prey - 1) + j]->fitness =
//                            predator_prey_teams_fitness[1][q][r];  // distribute each prey's fitness to constituent networks
//                    current_teams_prey[q][r * (total_prey_networks_per_team / num_of_prey - 1) + j]->addFitness();  // add network fitness to its neurons
//                }
//
//                //Following two lines are for the combiner network
//                current_teams_prey[q][(total_prey_networks_per_team - num_of_prey) + r]->fitness =
//                        predator_prey_teams_fitness[1][q][r];  // distribute each prey's fitness to constituent networks
//                current_teams_prey[q][(total_prey_networks_per_team - num_of_prey) + r]->addFitness();  // add network fitness to its neurons
//            }
//
//            //Computing Generation Best - current team with highest total fitness.
//            //Note: individual prey in a team can have different fitness in
//            //case of competition. However, all networks in a individual have same fitness
//            for (int r = 0; r < num_of_prey; r++) {
//                current_teams_prey_sum_individual_fitness[q] =
//                        current_teams_prey_sum_individual_fitness[q]
//                                + current_teams_prey[q][r
//                                        * (total_prey_networks_per_team / num_of_prey - 1)]->fitness;
//            }
//            current_teams_prey_sum_individual_fitness[q] /= num_of_prey;  //Averaging over team
//
//            // if team outperforms bestTeam, set bestTeam = team  (Note: all networks in a individual have same fitness)
//            if (current_teams_prey_sum_individual_fitness[q]
//                    > generation_best_teams_prey_sum_individual_fitness[q]) {
//                generation_best_teams_prey_sum_individual_fitness[q] =
//                        current_teams_prey_sum_individual_fitness[q];
//                for (int j = 0; j < total_prey_networks_per_team; j++) {
//                    generation_best_teams_prey[q][j]->setNetwork(current_teams_prey[q][j]);
//                    generation_best_teams_prey[q][j]->fitness = current_teams_prey[q][j]->fitness;
//                }
//            }
//        }

    }  // end for
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
                overall_best_teams[q][j]->setNetwork(generation_best_teams[q][j]);
                overall_best_teams[q][j]->fitness = generation_best_teams[q][j]->fitness;
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
                    << overall_best_teams[q][r
                            * (total_predator_networks_per_team / num_of_predators - 1)]->fitness
                    << ", Generation Average fitness  "
                    << average_predator_prey_teams_fitness[0][q][r] / numTrials << "\n";
        }
    }

    fout_champfitness << generation << " " << average_predator_prey_teams_fitness[0][0][0] / numTrials << "\n";

    //********Commenting below because prey does not evolve****************
    // for (int q=0; q<num_teams_prey; q++) {
    //         for (int r=0; r<num_of_prey; r++) {
    //             cout << "generation " << generation << ":\tPrey Team number "<< q << ":\tPrey Number "<< r <<"  Generation Best fitness:: "<< generation_best_teams_prey[q][r*(total_prey_networks_per_team/num_of_prey -1)]-> fitness
    //                     << ",  Overall Best fitness(JUNK):: " << overall_best_teams_prey[q][0]->fitness
    //                     << ", Generation Average fitness  " << average_predator_prey_teams_fitness[1][q][r]/numTrials
    //                     << "\n";
    //         }
    // }
    //********END Commenting below because prey does not evolve****************

    vector<vector<double> > testNet_result;  //number of prey caught by all the predators teams in each of the 20 trials

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
    testNet_result = Envt.testNet(overall_best_teams, 20);

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
    // form a random net for predator teams, and test it for 20 trials
//    for (int q = 0; q < num_teams_prey; q++) {
//        for (int j = 0; j < total_prey_networks_per_team; j++)
//            for (int k = 0; k < num_hidden_neurons[0]; k++)
//                current_teams_prey[q][j]->setNeuron(
//                        hidden_neuron_populations_prey[q][j][k]->selectNeuron(), k);
//    }

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
//        for (int p = 0; p < num_teams_prey; p++) {
//            for (int q = 0; q < num_of_prey; q++) {
//                temp_individual_fitness.push_back(0.0);
//            }
//            temp_team_fitness.push_back(temp_individual_fitness);
//            temp_individual_fitness.clear();
//        }
//        predator_prey_teams_fitness.push_back(temp_team_fitness);
        //************END INITIALIZING predator_prey_teams_fitness ********************

        for (int p = 0; p < SIMU_TRIALS; p++) {
            if (SHOW) {
                FILE* fptr;
                //Saving predator-prey locations
                if ((fptr = fopen("pred_prey_location.txt", "at")) == NULL) {
                    LOG(FATAL) << endl << "Error - cannot open " << "pred_prey_location.txt"
                            << endl;
                    //exit(1);
                }
                fprintf(fptr, "\n");  //  next line
                fprintf(fptr, "\n");  //  next line
                fprintf(fptr, "%d ", generation);  // generation  number
                fprintf(fptr, " ");  //  space
                fprintf(fptr, "%d ", p);  // simu trial  number
                fclose(fptr);
            }

            teams_fitness = Envt.evalNet(generation_best_teams, generation);
            LOG(INFO) << endl << " Simulation Trial Number:: " << p << endl;
            for (int q = 0; q < num_teams_predator; q++) {
                for (int r = 0; r < num_of_predators; r++) {
                    predator_prey_teams_fitness[0][q][r] = predator_prey_teams_fitness[0][q][r]
                            + teams_fitness[0][q][r];
                    LOG(INFO) << " Generation Best Predator Team number " << q
                            << "  Predator Number " << r << "  fitness:: " << teams_fitness[0][q][r]
                            << endl;
                }
            }
//            for (int q = 0; q < num_teams_prey; q++) {
//                for (int r = 0; r < num_of_prey; r++) {
//                    predator_prey_teams_fitness[1][q][r] = predator_prey_teams_fitness[1][q][r]
//                            + teams_fitness[1][q][r];
//                    LOG(INFO) << " Generation Best Prey Team number " << q << "  Prey Number " << r
//                            << "  fitness:: " << teams_fitness[1][q][r] << endl;
//                }
//            }
        }

        for (int q = 0; q < num_teams_predator; q++) {
            for (int r = 0; r < num_of_predators; r++) {
                LOG(INFO) << "Average fitness for Predator Team Number " << q
                        << " Predator Number " << r << ":: "
                        << predator_prey_teams_fitness[0][q][r] / SIMU_TRIALS << endl;
            }
        }
//        for (int q = 0; q < num_teams_prey; q++) {
//            for (int r = 0; r < num_of_prey; r++) {
//                LOG(INFO) << "Average fitness for Prey Team Number " << q << " Prey Number " << r
//                        << ":: " << predator_prey_teams_fitness[1][q][r] / SIMU_TRIALS << endl;
//            }
//        }
        SHOW = 0;
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
        save("out_10000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 10500)
        save("out_10500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 11000)
        save("out_11000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 11500)
        save("out_11500.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 12000)
        save("out_12000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 13000)
        save("out_13000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 14000)
        save("out_14000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 15000)
        save("out_15000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 16000)
        save("out_16000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 17000)
        save("out_17000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
    else if (generation == 18000)
        save("out_18000.bin", num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);

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
    // cout << "DELTA started on predator " << pred << endl;
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

//    for (int k = 0; k < num_teams_prey; k++) {
//        for (int i = 0; i < total_prey_networks_per_team; i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                hidden_neuron_populations_prey[k][i][j]->average();
//    }
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

    fwrite(&generation, 4, 1, fptr);  // int generation

    for (int p = 0; p < num_teams_predator; p++)
        for (int i = 0; i < total_predator_networks_per_team - (COMBINE * num_of_predators); i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++)
                for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                    for (int m = 0; m < GENE_SIZE_PRED_NW; m++) {  // weights
                        fwrite(&(hidden_neuron_populations[p][i][j]->pop[k]->weight[m]),
                                sizeof(double), 1, fptr);
                    }

//    for (int p = 0; p < num_teams_prey; p++)
//        for (int i = 0; i < total_prey_networks_per_team - (COMBINE * num_of_prey); i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                for (int k = 0; k < hidden_neuron_populations_prey[p][i][j]->numNeurons; k++)
//                    for (int m = 0; m < GENE_SIZE_PREY_NW; m++) {  // weights
//                        fwrite(&(hidden_neuron_populations_prey[p][i][j]->pop[k]->weight[m]),
//                                sizeof(double), 1, fptr);
//                    }

    for (int p = 0; p < num_teams_predator; p++)
        for (int i = total_predator_networks_per_team - (COMBINE * num_of_predators);
                i < total_predator_networks_per_team; i++)
            for (int j = 0; j < num_hidden_neurons[i]; j++)
                for (int k = 0; k < hidden_neuron_populations[p][i][j]->numNeurons; k++)
                    for (int m = 0; m < GENE_SIZE_PRED_COMBINER_NW; m++) {  // weights
                        fwrite(&(hidden_neuron_populations[p][i][j]->pop[k]->weight[m]),
                                sizeof(double), 1, fptr);
                    }

//    for (int p = 0; p < num_teams_prey; p++)
//        for (int i = total_prey_networks_per_team - (COMBINE * num_of_prey);
//                i < total_prey_networks_per_team; i++)
//            for (int j = 0; j < num_hidden_neurons[0]; j++)
//                for (int k = 0; k < hidden_neuron_populations_prey[p][i][j]->numNeurons; k++)
//                    for (int m = 0; m < GENE_SIZE_PREY_COMBINER_NW; m++) {  // weights
//                        fwrite(&(hidden_neuron_populations_prey[p][i][j]->pop[k]->weight[m]),
//                                sizeof(double), 1, fptr);
//                    }

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

////////////////////////////////////////////////////////////////////////////
//
// Code to add/remove subpops
//
//
///////////////////////////////////////////////////////////////////////////

// add a weight at position 'locus' for all neuron in the subpop.
void subPop::addConnection(int locus)
{
    // if(locus < GENE_SIZE_PRED_NW)
    for (int i = 0; i < numNeurons; ++i)
        pop[i]->weight.insert(pop[i]->weight.begin() + locus,
        //(drand48() * 12.0) - 6.0 );
                1.0);//rndCauchy(0.3) );
}

//----------------------------------------------------------------------
// opposite of addConnection.
void subPop::removeConnection(int locus)
{
    if (!IS_PREY) {
        if (locus < GENE_SIZE_PRED_NW)
            for (int i = 0; i < numNeurons; ++i) {
                pop[i]->weight.erase(pop[i]->weight.begin() + locus);
            }
    } else {
        LOG(FATAL) << "NOt suupposed to be here";
//        if (locus < GENE_SIZE_PREY_NW)
//            for (int i = 0; i < numNeurons; ++i) {
//                pop[i]->weight.erase(pop[i]->weight.begin() + locus);
//            }
    }

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

    // cout << "Adding SUBPOP " << numPops[pred]
    //      << " to predator " << pred << endl;
}

//----------------------------------------------------------------------
// opposite of addSubPop.
int Esp::removeSubPop(vector<Network*>& team, int pred, int num_of_predators, int num_of_prey)
{
    int sp;

    // see which (if any) subpop can be removed from pred
    sp = team[pred]->lesion(Envt, team, num_of_predators, num_of_prey);
    return (removeSubPop(sp, pred, num_of_predators, num_of_prey));  // remove it.
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
//      cout << "Remove SUBPOP " << sp << " from predator " << pred << endl;
//      cout << "Now " << numPops[pred] << " subpops\n";
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
static void parseArgs(int argc, char *argv[], int *numPops, int *popSize, int *seed, int *netType,
        bool *analyze, int *seedNet)
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
            cout << "Usage: " << argv[0] << " <options>\n";
            cout << "options:\n";
            cout << "  -z Z : Z number of hidden units.\n";
            cout << "  -n N : N neurons in each subpop\n";
            cout << "  -s S : Initial integer seed S.\n";
            cout << "  -t T : Type of Network.\n";
            cout << "         0 = Feedforward, 1 = SRN, 2 = 2ndOrder Recurrent\n";
            cout << "         3 = Fully Recurrent\n";
            //cout << "  -f F : Seed network. \n";
            cout << "  --mutation M : Mutation rate in percent (0,1). \n";
            cout << "  --stag STAG : Stagnation threshold, number of generations. \n";
            cout << "  --minimize : Minimize fitness. \n";
            //      cout << "  -x   : analyze nets.\n";
            cout << "  -h   : this message.\n";
            cout << "Default: " << argv[0] << " -z " << DEFAULT_NUM_POPS << " -n "
                    << DEFAULT_POP_SIZE << " -s getpid()" << " -t " << FF
                    << " --mutation 0.4" << " --stag 20";
            cout << "\n" << flush;
            exit(0);
        }
    }
}

//----------------------------------------------------------------------
// Check command-line arguments.
static void checkArgs(int numPops, int popSize)
{
    if (MIN)
        LOG(INFO) << "MINIMIZING\n";
}

//--------------------------------------------------------------------
// OpenGL initialization functions, copied from
// http://www.opengl.org/About/FAQ/technical/gettingstarted.htm#0030
//--------------------------------------------------------------------
static int sAttribList[] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, None };

//////////////////////////////////////////////////////////////////////
//
// main
//
//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    libconfig::Config cfg;
    google::InitGoogleLogging("");

    cfg.readFile("esp_predprey.cfg");

    // Parse command-line arguments.
    int num_hidden_neurons, popSize, seed, seedNet = 0;
    int netType;
    bool analyze = false;

    if (argc == 1) {
        cout << "Enter name of config file when running the command\n";
        cout << "For example: \"./esp-predprey config_file.txt\"\n";
        exit(1);
    }

    //*******************************Reading parameters from config file*************************

    double value;
    string line;
    ifstream config_file(argv[1]);
    vector<double> config_data;
    while (getline(config_file, line)) {
        istringstream iss2(line, istringstream::in);
        iss2 >> value;
        config_data.push_back(value);
    }

    start_generation = (int) config_data[0];  // Specify the generation number from which we start the esp (if greater than one, then should have a corresponding .bin file to load from)
    if_display = (bool) config_data[1];  // 1 - Simulation display is ON, 0 - OFF
    display_frequency = (int) config_data[2];  // Specify after how many generations you want display
    maxGenerations = (int) config_data[3];  // Maximum number of generations
    int num_teams_predator = (int) config_data[4];  // Number of predator teams
    int num_of_predators = (int) config_data[5];  // Number of predators in each team
    if_shared_fitness_predator = (bool) config_data[6];  // 1 - Fitness sharing in predator team, 0 - individual fitness in predator team
    pred_communication = (bool) config_data[7];  // 1 - Direct communication in predator teams, 0 - No direct communication in predator teams
    pred_messaging = (bool) config_data[8];  // 1 - Broadcast Messaging in predator teams, 0 - No Broadcast Messaging in predator teams
    int num_teams_prey = (int) config_data[9];  // Number of prey teams
    int num_of_prey = (int) config_data[10];  // Number of prey in each team
    prey_reappears = (bool) config_data[11];  // 1 - Each Prey reappears once it gets killed, 0 - Prey does not reappear
    if_evolve_prey = (bool) config_data[12];  // 1 - Evolve prey, 0 - Do not evolve prey (fixed behavior prey)
    prey_communication = (bool) config_data[13];  // 1 - Direct communication in prey teams, 0 - No direct communication in prey teams
    prey_messaging = (bool) config_data[14];  // 1 - Broadcast Messaging in prey teams, 0 - No Broadcast Messaging in prey teams
    int count, i;
    for (i = 15; i < 15 + num_teams_prey; i++) {
        reward_prey_team.push_back((int) config_data[i]);  // Reward for prey team 0 (If there are more prey teams, then add more lines below this to specify the prey team reward size)
    }
    count = i;
    for (; i < count + num_teams_prey; i++) {
        prey_move_probability.push_back((double) config_data[i]);  // Prey speed relative to the predator speed (If there are more prey teams, then add more lines below this to specify the prey team speed)
    }

    int num_of_hunters = (int) config_data[i];
    //cout << "Num hunters in main is "<<num_of_hunters<<endl;

    if (if_evolve_prey) {
        LOG(FATAL) << " Prey evolution is not supported now." << endl;
        //exit(0);
    }
    //*******************************END Reading parameters from config file*************************

    setUpCtrlC();
    parseArgs(argc, argv, &num_hidden_neurons, &popSize, &seed, &netType, &analyze, &seedNet);
    checkArgs(num_hidden_neurons, popSize);
    reseed(seed);
    //PredPrey predprey(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
    //        prey_move_probability, num_of_hunters);  // this is our environment.

    LOG(INFO) << "  ESP.C :: number of predator teams:: " << num_teams_predator
            << " number of prey teams:: " << num_teams_prey
            << " number of predators in each team:: " << num_of_predators
            << " num of prey in each team:: " << num_of_prey << " num of hunter teams:: " << 1
            << " num of hunters in each team:: " << num_of_hunters << endl;

    num_hidden_neurons = cfg.lookup("num_hidden_neurons");  //10;  //This is the number of Hidden Neurons in each Network
    popSize = cfg.lookup("pop_size");  //100;

    if (if_display) {
        // OpenGL variables
        Display *dpy;
        XVisualInfo *vi;
        XSetWindowAttributes swa;
        Window win;
        GLXContext cx;

        //Switching off display for condor runs - ADITYA
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                LOG(ERROR) << "Error" << endl;
        }  //ADI

        dpy = XOpenDisplay(0);

        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                LOG(ERROR) << "Error" << endl;
        }  //ADI
        if ((vi = glXChooseVisual(dpy, DefaultScreen(dpy), sAttribList)) == NULL) {
            fprintf(stderr, "ERROR: Can't find suitable visual!\n");
            return 0;
        }
        swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;
        //  win = XCreateSimpleWindow(dpy, RootWindow(dpy,DefaultScreen(dpy)),
        //                      0, 0, 1000, 1000, 1,
        //                      BlackPixel(dpy, DefaultScreen(dpy)),
        //                      WhitePixel(dpy, DefaultScreen(dpy)));
        win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 30, 30, 500, 500, 0, vi->depth,
                CopyFromParent, vi->visual, CWBackPixel | CWBorderPixel | CWColormap, &swa);
        XMapWindow(dpy, win);
        cx = glXCreateContext(dpy, vi, 0, GL_TRUE);
        glXMakeCurrent(dpy, win, cx);

        // clear window
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
        glPointSize(10);
        glFlush();
    }

    bool use_monolithic_network = (int)cfg.lookup("use_monolithic_network") == 1 ? true : false;
    double hunter_move_probability = (double) cfg.lookup("hunter_move_probability");

    // --- Construct a new ESP ---
    if (start_generation == 1) {
        if (!use_monolithic_network) {
            file_prefix = "st1_";
            // Sub-task 1
            PredPreyST1 *predprey_st1 = new PredPreyST1(num_of_predators, num_of_prey,
                    num_teams_predator, num_teams_prey, prey_move_probability);  // this is our environment.
            Esp *esp1 = new Esp(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                    0 /*num_teams_hunters*/, 0, num_hidden_neurons /*nPops*/, popSize/*nSize*/,
                    *predprey_st1/*environment*/, netType);
            esp1->evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey, 0, 0);  // evolve
            vector<vector<vector<Network*> > > hall_of_fame_pred_1 = esp1->hall_of_fame_pred;
            LOG(INFO) << "Done with subtask one" << endl << endl;

            file_prefix = "st2_";
            // Sub-task 2
            PredPreyST2 *predprey_st2 = new PredPreyST2(num_of_predators, num_teams_predator,
                    num_of_hunters, 1, hunter_move_probability);  // this is our environment.
            Esp *esp2 = new Esp(num_of_predators, 0, num_teams_predator, 0, 1 /*num_teams_hunters*/,
                    num_of_hunters, num_hidden_neurons /*nPops*/, popSize/*nSize*/,
                    *predprey_st2/*environment*/, netType);
            esp2->evolve(maxGenerations, num_of_predators, 0, num_teams_predator, 0, 1,
                    num_of_hunters);  // evolve
            vector<vector<vector<Network*> > > hall_of_fame_pred_2 = esp2->hall_of_fame_pred;
            LOG(INFO) << "Done with subtask two" << endl << endl;

            file_prefix = "ot_";
            vector<vector<vector<vector<Network*> > > > hall_of_fame_pred_both = vector<
                    vector<vector<vector<Network*> > > >();
            hall_of_fame_pred_both.push_back(hall_of_fame_pred_1);
            hall_of_fame_pred_both.push_back(hall_of_fame_pred_2);

            // Overall task
            PredPrey *predprey = new PredPrey(num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey, prey_move_probability, num_of_hunters, hunter_move_probability);  // this is our environment.
            Esp *esp = new Esp(hall_of_fame_pred_both, num_of_predators, num_of_prey,
                    num_teams_predator, num_teams_prey, 1 /*num_teams_hunters*/, num_of_hunters,
                    num_hidden_neurons /*nPops*/, popSize/*nSize*/, *predprey/*environment*/,
                    netType);
            esp->evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey, 1, num_of_hunters);  // evolve
            LOG(INFO) << "Done with overall task";
        } else {
            LOG(INFO) << "Using monolithic network";
            PredPrey *predprey = new PredPrey(num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey, prey_move_probability, num_of_hunters, hunter_move_probability);  // this is our environment.
            Esp *esp = new Esp(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                    1 /*num_teams_hunters*/, num_of_hunters, num_hidden_neurons /*nPops*/,
                    popSize/*nSize*/, *predprey/*environment*/, netType);
            esp->evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                    num_teams_prey, 1, num_of_hunters);  // evolve
        }

        /*
         if (analyze) {
         esp.findChampion();
         exit(1);
         }
         */
    }
    // --- Load an old ESP from file ---
    else {
        char filename[50] = "";
        std::string s;
        std::stringstream out;
        out << start_generation;
        s = out.str();
        const char * a = s.c_str();
        strcat(filename, "out_");
        strcat(filename, a);
        strcat(filename, ".bin");
        if (use_monolithic_network) {
            PredPrey predprey(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                    prey_move_probability, num_of_hunters, hunter_move_probability);  // this is our environment.
            Esp esp(filename, num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                    1 /*num_teams_hunters*/, num_of_hunters, num_hidden_neurons, popSize, predprey,
                    netType);
            esp.evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                            num_teams_prey, 1, num_of_hunters);  // evolve
        } else {

        }

//        if (analyze) {
//            esp.findChampion();
//            //exit(1);
//        }
    }

}

