/*
 * common.cpp
 *
 *  Created on: Jan 29, 2012
 *      Author: anand
 */

#include "common.h"

namespace PredatorPreyHunter
{
    double fetchRandomDouble()
    {
        return drand48();
    }
    long fetchRandomLong()
    {
        return lrand48();
    }
}

double rndCauchy(double wtrange)
{
    double u = 0.5, Cauchy_cut = 10.0;

    while (u == 0.5)
        u = drand48();
    u = wtrange * tan(u * PI);
    if (fabs(u) > Cauchy_cut)
        return rndCauchy(wtrange);
    else
        return u;
}
