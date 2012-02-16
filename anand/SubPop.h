/*
 * subPop.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef SUBPOP_H_
#define SUBPOP_H_

#include "common.h"

#include "Neuron.h"
#include "Network.h"

namespace EspPredPreyHunter
{
    class Network;
    class SubPop
    {

        // FIXME
        const double MUT_RATE; // = 0.4;  // mutation rate
        const int MIN;  // = 0; 0 means higher fitness better, 1 otherwise

        uint geneSize;

    public:
        std::vector<Neuron*> pop;

        int numNeurons;
        bool evolvable;

        SubPop(const int& size, const int& geneSize);
        ~SubPop();
        //  subPop(const subPop &s);
        //void addHiddenLayer(int size);
        void create();  // creates a random subpopulation of neurons
        void evalReset();
        Neuron *selectNeuron();
        void average();
        void qsortNeurons();
        void recombine();
        void burstMutate();
        void mutate();
        void deltify(Neuron *bestNeuron);
        void save(char *fname);
        void addConnection(int locus);
        void removeConnection(int locus);
        void saveBin(char *fname);
        void saveText(char *fname);
        int load(char *filename);
        void print();
        void printWeight(FILE *file);
        void printDelta(FILE *file);
        void recombineHallOfFame(Network* hallOfFameNetwork, const uint& neuronNumber);     //EVOLVE_PREY
    private:
        int numBreed;  //number of neurons to be mated in subpop
        void crossover(const std::vector<double> &parent1, const std::vector<double> &parent2,
                std::vector<double> &child1, std::vector<double> &child2);
        int findMate(int num);
    };
}

#endif /* SUBPOP_H_ */
