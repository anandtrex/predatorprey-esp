#include "Basic.h"
#include <stdlib.h>

namespace PredatorPreyHunter {
  // wrapper for drand48() so that you can change this function later
  double fetchRandomNumber() {
    return drand48();
  }
}
