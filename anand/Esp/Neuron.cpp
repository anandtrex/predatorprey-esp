/*
 * neuron.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "Neuron.h"

namespace EspPredPreyHunter
{
    /**
     * Neuron constructor
     */
    Neuron::Neuron(const uint& geneSize)
        :geneSize(geneSize)
    {
        for (int i = 0; i < geneSize; ++i)
            weight.push_back(0.0);     //fill weight vector with zeros
        tests = 0;
    }

    /**
     * Destructor
     */
    Neuron::~Neuron()
    {
    }

    /**
     * perturb the weights of a neuron. Used to search in a neighborhood
     * around some neuron (best).
     * @param n
     * @param randFn
     * @param coeff
     */
    void Neuron::perturb(const Neuron &n, double(*randFn)(double), double coeff)
    {
        for (int i = 0; i < geneSize; ++i) {
            weight[i] = n.weight[i] + (randFn)(coeff);
        }
    }

    void Neuron::perturb(const Neuron &n)
    {
        perturb(n, rndCauchy, 0.3);
    }

    /**
     * same as above but called on self and returns new neuron
     * @return
     */
    Neuron* Neuron::perturb()
    {
        Neuron *n = new Neuron(geneSize);
        for (int i = 0; i < geneSize; ++i) {
            n->weight[i] = weight[i] + rndCauchy(0.3);
        }
        return n;
    }

    /**
     * neuron assignment op
     * @param n
     * @return
     */
    Neuron Neuron::operator=(const Neuron &n)
    {
        weight = n.weight;
        return *this;
    }

    /**
     * create a new set of random weights
     */
    void Neuron::create()
    {
        int i;
        for (i = 0; i < geneSize; ++i) {
            weight[i] = (drand48() * 12.0) - 6.0;
        }

    }
}

