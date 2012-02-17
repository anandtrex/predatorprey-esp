/*
 * subPop.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */
#include <algorithm>
#include <vector>

#include "common.h"
#include "SubPop.h"


namespace EspPredPreyHunter
{
    using std::sort;
    using std::binary_function;
    using std::vector;

    struct maximize_fit: public binary_function<Neuron*, Neuron*, bool>
    {
        bool operator()(Neuron *x, Neuron *y)
        {
            return x->fitness > y->fitness;
        }
    };

    struct minimize_fit: public binary_function<Neuron*, Neuron*, bool>
    {
        bool operator()(Neuron *x, Neuron *y)
        {
            return x->fitness < y->fitness;
        }
    };

    // FIXME! Constants
    const double SubPop::MUT_RATE = 0.4;

    /**
     * subPop constructor
     * @param size
     */
    SubPop::SubPop(const int& size, const int& geneSize)
            : geneSize(geneSize), pop(size)
    {
        evolvable = true;
        numNeurons = size;
        numBreed = (int) numNeurons / 4;

        // We leave subPop::create to actually create the neurons.
        // This way neurons are not created when we initially
        // create a Network
    }

    /**
     * destructor
     */
    SubPop::~SubPop()
    {
        /*
        // Delete pointer elements of vector pop one by one (freeing up memory only - not deleting pointers vector)
        for (int i = 0; i < numNeurons; ++i)
            delete pop[i];

        //Clear after freeing up the memory in the previous step
        pop.clear();
        */
    }

    /**
     * create the neurons, initial their weights, and put them in the subpop.
     */
    void SubPop::create()
    {
        if (evolvable)
            for (int i = 0; i < numNeurons; ++i) {
                pop[i] = new Neuron(geneSize);
                pop[i]->create();
            }
    }

    /**
     * used to perform "delta-coding" like
     * @param bestNeuron
     */
    void SubPop::deltify(Neuron *bestNeuron)
    {
        for (int i = 0; i < numNeurons; ++i) {
            pop[i]->perturb(*bestNeuron);     // make each neuron a perturbation of the
                                              // neuron in the best network that
                                              // corresponds to that subpop
            //delete pop[i];
            //if(i > (numNeurons/2))
            //  pop[i] = bestNeuron->perturb();
            //else
            //pop[i] = tmp.perturb();
        }
    }

    /**
     * reset fitness and test vals of all neurons
     */
    void SubPop::evalReset()
    {
        for (int i = 0; i < numNeurons; ++i) {
            pop[i]->fitness = 0;
            pop[i]->tests = 0;
        }
    }

    /**
     * select a neuron at random
     * @return
     */
    Neuron *SubPop::selectNeuron()
    {
        return pop[lrand48() % numNeurons];
    }

    /**
     * normalize the neuron fitnesses
     */
    void SubPop::average()
    {
        VLOG(5) << "Averaging neuron fitness in neuron";
        for (int i = 0; i < numNeurons; ++i) {
            if (pop[i]->tests)
                pop[i]->fitness = pop[i]->fitness / pop[i]->tests;
            else
                pop[i]->fitness = 0;

        }
    }

    /**
     * sort the neurons in each subpop using quicksort.
     */
    void SubPop::qsortNeurons()
    {
        VLOG(5) << "Sorting in subpop";
        if (MIN)
            sort(pop.begin(), pop.end(), minimize_fit());
        else
            sort(pop.begin(), pop.end(), maximize_fit());
    }

    /**
     * recombine neurons with members of their subpop using crossover.
     */
    void SubPop::recombine()
    {
        int i = 0;
        for (; i < numBreed; ++i) {
            crossover(pop[i]->weight, pop[findMate(i)]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight, pop[numNeurons - (2 + i * 2)]->weight);
        }
    }

    /**
     * mutate some of the neurons with cauchy noise.
     */
    void SubPop::mutate()
    {
        VLOG(5) << "Doing mutate in subpop";
        // TODO Remember combiner network details
        for (int i = numBreed * 2; i < numNeurons; ++i)
            if (drand48() < MUT_RATE)
                pop[i]->weight[lrand48() % geneSize] += rndCauchy(0.3);
    }

    /**
     * mutate all of the neurons with cauchy noise.
     */
    void SubPop::burstMutate()
    {
        // TODO Remember combiner network details
        for (int i = 0; i < numNeurons; ++i)
            if (drand48() < MUT_RATE)
                pop[i]->weight[lrand48() % geneSize] += rndCauchy(0.3);
    }

    /**
     * randomly find a mate in the same subpop.
     * @param num
     * @return
     */
    int SubPop::findMate(int num)
    {
        return (lrand48() % numBreed);
//  if (num) {
//    return (lrand48()%numBreed);
//  }
//  else  {
//    return (lrand48()%num);
//  }
    }

    /**
     * 1-point crossover
     * @param parent1
     * @param parent2
     * @param child1
     * @param child2
     */
    void SubPop::crossover(const vector<double> &parent1, const vector<double> &parent2,
            vector<double> &child1, vector<double> &child2)
    {
        int cross1;
        //find crossover point
        // TODO Remember combiner network details
        cross1 = lrand48() % geneSize;
        child1 = parent2;
        child2 = parent1;
        swap_ranges(child1.begin(), child1.begin() + cross1, child2.begin());
    }

    /**
     * Code to add subpops
     * add a weight at position 'locus' for all neuron in the subpop.
     * @param locus
     */
    void SubPop::addConnection(int locus)
    {
        // if(locus < GENE_SIZE_PRED_NW)
        for (int i = 0; i < numNeurons; ++i)
            pop[i]->weight.insert(pop[i]->weight.begin() + locus,
            //(drand48() * 12.0) - 6.0 );
                    1.0);//rndCauchy(0.3) );
    }

    /**
     * Code to remove subpops
     * opposite of addConnection.
     * @param locus
     */
    void SubPop::removeConnection(int locus)
    {
        if (locus < geneSize)
            for (int i = 0; i < numNeurons; ++i) {
                pop[i]->weight.erase(pop[i]->weight.begin() + locus);
            }

    }

    // QUE Why does it do it like this??
    /**
     * recombine neurons with members of their subpop using crossover.
     */
    void SubPop::recombineHallOfFame(Network* hallOfFameNetwork, const uint& neuronNumber)
    {
        VLOG(5) << "Doing recombine hall of fame in subpop";
        int i = 0;
        for (; i < numBreed - 5; ++i) {
            crossover(pop[i]->weight, pop[findMate(i)]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight,
                    pop[numNeurons - (2 + i * 2)]->weight);
        }
        for (; i < numBreed; ++i) {
            crossover(pop[i]->weight, hallOfFameNetwork->neurons[neuronNumber]->weight,
                    pop[numNeurons - (1 + i * 2)]->weight,
                    pop[numNeurons - (2 + i * 2)]->weight);
        }
    }
}

