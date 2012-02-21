/*
 * Network.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "Neuron.h"
#include "SubPop.h"
#include <math.h>

namespace EspPredPreyHunter
{
    using std::vector;

    inline double sigmoid(double x, double slope = 1.0)
    {
        return (1 / (1 + exp(-(slope * x))));
    }

    class SubPop;
    class Network     //: public subPop
    {
        enum
        {
            MIN = 0
        };     // = 0; 0 means higher fitness better, 1 otherwise

        uint subPopSize;
        bool evolvable;

        /**
         * i-th SubPop corresponds to the i-th hidden neuron in the network
         */
        vector<SubPop*> networkSubPop;
        double fitness;

        /**
         * create the neurons, initial their weights, and put them in the subpop.
         */
        void create();     // creates a random subpopulation of neurons

    protected:
        vector<double> activation;
        uint geneSize;
        uint numHiddenNeurons;

    public:
        vector<Neuron*> neurons;

        Network(const uint& numHiddenNeurons, const uint& geneSize, const uint& subPopSize);

        virtual ~Network()
        {
            for(int i = 0; i < neurons.size(); i++){
                delete neurons[i];
            }
            neurons.clear();

            for(int i = 0; i < networkSubPop.size(); i++){
                delete networkSubPop[i];
            }
            networkSubPop.clear();
        }

        /**
         * Set random neurons from subpopulations as neurons of the network
         * NOTE: This is called for every evaluation/generation
         */
        void setNeurons();
        void setFitness(const double& fitness);
        void evalReset();
        void average();
        void qsortNeurons();
        void mutate();
        void recombineHallOfFame(Network* network);
        void operator=(Network &n);
        void resetActivation();
        void setNeuron(Neuron *n, int position);
        void setNetwork(Network *n);
        virtual void addNeuron() = 0;
        virtual void removeNeuron(int sp) = 0;
        int lesion();
        void addFitness();
        void incrementTests();
        virtual void activate(const vector<double> &input, vector<double> &output) = 0;
        virtual void save(char *filename) = 0;
    };
}

#endif /* NETWORK_H_ */
