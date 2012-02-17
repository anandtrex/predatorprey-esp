#include "Basic.h"
#include <stdlib.h>

namespace PredatorPreyHunter {
  // wrapper for drand48() so that you can change this function later
  // fetch random number between 0 and 1
  double fetchRandomNumber() {
    return drand48();
  }
}
