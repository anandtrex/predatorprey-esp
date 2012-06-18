/*
 * common.cpp
 *
 *  Created on: Jan 29, 2012
 *      Author: anand
 */

#include "common.h"
#include <sstream>
#include <limits>
#include <algorithm>

namespace PredatorPreyHunter
{
    using std::numeric_limits;
    using std::vector;
    using std::max_element;

    double fetchRandomDouble()
    {
        return drand48();
    }
    long fetchRandomLong()
    {
        return lrand48();
    }

    uint getMaxIndex(const vector<double>& vec)
    {
        VLOG(5) << "Returning the max index";
        //return std::distance(vec.begin(), max_element(vec.begin(), vec.end()));
        vector<uint> maxIndexes = vector<uint>();
        double maxValue = -numeric_limits<double>::max();

        vector<double>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            VLOG(5) << *it;
            if (*it > maxValue) {
                maxValue = *it;
                maxIndexes = vector<uint>();
                maxIndexes.push_back(std::distance(vec.begin(), it));
            } else if (*it == maxValue) {
                maxIndexes.push_back(std::distance(vec.begin(), it));
            }
            it++;
        }
        if (maxIndexes.size() == 1) {
            VLOG(5) << "Only one max index";
            return maxIndexes[0];
        } else {
            VLOG(5) << "Multiple max indexes";
            return maxIndexes[fetchRandomLong() % (maxIndexes.size() - 1)];
        }
    }
}

namespace EspPredPreyHunter
{
    using std::ostringstream;
    using std::string;

    string vecToString(const vector<double>& vec)
    {
        ostringstream o;
        for (uint i = 0; i < vec.size(); i++) {
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

    string concatStringDouble(string s, double d)
    {
        ostringstream o;
        o << s << d;
        return o.str();
    }
}

