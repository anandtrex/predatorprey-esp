/*
 * neuron.h
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <vector>

#include "common.h"

namespace EspPredPreyHunter
{
    class Neuron
    {
        uint geneSize;
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
        Neuron(const uint& geneSize);
        ~Neuron();
        Neuron operator=(const Neuron &);
        void create();
        void perturb(const Neuron &n);
        void perturb(const Neuron &n, double(*randFn)(double), double);
        Neuron* perturb();
    };
}

#endif /* NEURON_H_ */
