/*
 * RandomAgent.h
 *
 *  Created on: Jan 29, 2012
 *      Author: anand
 */

#ifndef RANDOMAGENT_H_
#define RANDOMAGENT_H_

#include "Agent.h"
#include <vector>

namespace PredatorPreyHunter {
  class RandomAgent : public Agent {
    public:
	  RandomAgent( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p );
      Position move( const std::vector<AgentInformation>& vAgentInformation );
  };
}
#endif /* RANDOMAGENT_H_ */
