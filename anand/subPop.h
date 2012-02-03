/*
 * subPop.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef SUBPOP_H_
#define SUBPOP_H_

#include "neuron.h"

namespace EspPredPreyHunter
{
    class subPop
    {

    public:
        //protected:
        std::vector<neuron*> pop;

        // Second Hidden layer
        //std::vector<neuron*> pop2;

//public:
        int numNeurons;
        //int numNeurons2;

        subPop(int size);
        ~subPop();
        //  subPop(const subPop &s);
        //void addHiddenLayer(int size);
        void create();  // creates a random subpopulation of neurons
        void evalReset();
        neuron *selectNeuron();
        void average();
        void qsortNeurons();
        void recombine();
        void recombine_hall_of_fame(int pred_or_prey_team, int pred_or_prey_number, int pop_number); //EVOLVE_PREY
        void burstMutate();
        void mutate();
        void deltify(neuron *bestNeuron);
        void save(char *fname);
        void addConnection(int locus);
        void removeConnection(int locus);
        void saveBin(char *fname);
        void saveText(char *fname);
        int load(char *filename);
        void print();
        void printWeight(FILE *file);
        void printDelta(FILE *file);
        bool evolvable;

    private:
        int numBreed;  //number of neurons to be mated in subpop
        void crossover(const std::vector<double> &parent1, const std::vector<double> &parent2,
                std::vector<double> &child1, std::vector<double> &child2);
        int findMate(int num);
    };
}

#endif /* SUBPOP_H_ */
