/*
 * subPop.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "subPop.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////
//
// SubPop
//
//////////////////////////////////////////////////////////////////////
namespace EspPredPreyHunter
{
    using std::sort;
    using std::binary_function;
    using std::vector;

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

    // subPop constructor
    subPop::subPop(int size) :
            pop(size)//, pop2(0)  // What is happening here ??
    {
        evolvable = true;
        numNeurons = size;
        //numNeurons2 = 0;
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
            crossover(pop[i]->weight, pop[findMate(i)]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight, pop[numNeurons - (2 + i * 2)]->weight);
        }
    }

//----------------------------------------------------------------------
// recombine neurons with members of their subpop using crossover. EVOLVE_PREY
    void subPop::recombine_hall_of_fame(int pred_or_prey_team, int pred_or_prey_number,
            int pop_number)
    {
        int i = 0;
        for (; i < numBreed - 5; ++i) {
            crossover(pop[i]->weight, pop[findMate(i)]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight, pop[numNeurons - (2 + i * 2)]->weight);
        }

        if (!IS_PREY) {
            for (; i < numBreed; ++i) {
                crossover(
                        pop[i]->weight,
                        Esp::hall_of_fame_pred[drand48() * (Esp::hall_of_fame_pred.size() - 1)][pred_or_prey_team][pred_or_prey_number]->pop[pop_number]->weight,
                        pop[numNeurons - (1 + i * 2)]->weight,
                        pop[numNeurons - (2 + i * 2)]->weight);
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
}

