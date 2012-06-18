/*
 * common.cpp
 *
 *  Created on: Jan 29, 2012
 *      Author: anand
 */

#include "common.h"
#include <sstream>

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

namespace EspPredPreyHunter
{
    using std::ostringstream;
    using std::string;

    string vecToString(const vector<double>& vec)
    {
        ostringstream o;
        for (int i = 0; i < vec.size(); i++) {
            o << vec[i] << " ";
        }
        return o.str();
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

    string concatStringDouble(string s, double d){
        ostringstream o;
        o << s << d;
        return o.str();
    }
}

