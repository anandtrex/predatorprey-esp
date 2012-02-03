/*
 * neuron.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <vector>

namespace EspPredPreyHunter
{
    class neuron
    {
    public:
        // weight[0..n-1] are input weights, for n inputs
        // weight[n..n+k-1] are output weights, for k outputs
        std::vector<double> weight;
        std::vector<double> delta;
        double fitness;  //neuron's fitness value
        int tests;  //holds the total # of networks participated in
        double activation;  //neuron's activation level
        // QUE What does lesioned do?
        bool lesioned;

        //constructor
        neuron();
        ~neuron();
        neuron operator=(const neuron &);
        void create();
        void perturb(const neuron &n);
        void perturb(const neuron &n, double(*randFn)(double), double);
        neuron* perturb();
    };
}

#endif /* NEURON_H_ */
