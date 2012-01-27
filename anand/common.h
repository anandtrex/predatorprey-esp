/*
 * common.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <glog/logging.h>

typedef unsigned int uint;

// wrapper for drand48() so that you can change this function later
  // fetch random number between 0 and 1
namespace PredatorPreyHunter {
	double fetchRandomNumber() {
		return drand48();
	}
}

#endif /* COMMON_H_ */
