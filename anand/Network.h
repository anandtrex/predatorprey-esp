/*
 * Network.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "Neuron.h"

namespace EspPredPreyHunter
{
    using std::vector;

    class Network     //: public subPop
    {
        const int MIN;     // = 0; 0 means higher fitness better, 1 otherwise

        uint geneSize;

        uint numHiddenNeurons;
        bool evolvable;

    protected:
        vector<double> activation;

    public:
        double fitness;
        vector<Neuron*> neurons;

        Network(const int& numHiddenNeurons, const int& geneSize)
                : MIN(0), geneSize(geneSize), numHiddenNeurons(numHiddenNeurons), activation(numHiddenNeurons), neurons(numHiddenNeurons)     // constants! FIXME
        {
            if (MIN)
                fitness = 10000000;
            else
                fitness = 0.0;

            for (int i = 0; i < numHiddenNeurons; i++) {
                neurons[i] = new Neuron(geneSize);
            }
            evolvable = true;
        }

        virtual ~Network();

        void create();  // creates a random subpopulation of neurons

        void operator=(Network &n);
        void resetActivation();
        void setNeuron(Neuron *n, int position);
        void setNetwork(Network *n);

        virtual void addNeuron() = 0;
        virtual void removeNeuron(int sp) = 0;

        int lesion();
        void addFitness();
        void incrementTests();
        virtual void activate(vector<double> &input, vector<double> &output,
                int inputSize_combiner) = 0;

        virtual void save(char *filename) = 0;
    };
}

#endif /* NETWORK_H_ */
