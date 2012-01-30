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

typedef unsigned int uint;

// wrapper for drand48() so that you can change this function later
  // fetch random number between 0 and 1
namespace PredatorPreyHunter {
	double fetchRandomNumber();
}

#endif /* COMMON_H_ */
