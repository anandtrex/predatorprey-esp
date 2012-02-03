/*
 * neuron.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anands
 */

#include "neuron.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////
//
// Neuron
//
//////////////////////////////////////////////////////////////////////

namespace EspPredPreyHunter
{
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
            LOG(FATAL) << "Not supposed to be here";
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
}

