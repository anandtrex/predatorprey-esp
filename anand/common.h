/*
 * common.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <glog/logging.h>
#include <cstdlib>
#include <math.h>

typedef unsigned int uint;

#define PI 3.1415926535897931

// wrapper for drand48() so that you can change this function later
// fetch random number between 0 and 1
namespace PredatorPreyHunter
{
    using std::vector;

    double fetchRandomDouble();
    long fetchRandomLong();

    /**
     * Returns the index of the maximum element in a given vector. If there are more than one maximum elements,
     * it returns a random index among the maximum elements
     * @param vec
     * @return
     */
    uint getMaxIndex(const vector<double>& vec);
}

namespace EspPredPreyHunter
{
    using std::vector;
    using std::string;
    string vecToString(const vector<double>& vec);
    template<typename T, size_t N>
    vector<T> makeVector(const T (&data)[N])
    {
        return std::vector<T>(data, data + N);
    }
    /**
     * generate a random number form a cauchy distribution centered on zero.
     */
    double rndCauchy(double wtrange);
    string concatStringDouble(string s, double d);

}

#endif /* COMMON_H_ */
