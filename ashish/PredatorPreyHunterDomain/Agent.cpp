#include "Agent.h"
#include <iostream>

namespace PredatorPreyHunter {
  using std::cerr;
  using std::endl;
  Agent::Agent( const GridWorld* ptrGridWorld, const uint& agentId, const Position& p ) {
      if ( ptrGridWorld == NULL ) {
        cerr << "Agent::constructor received a NULL grid world pointer" << endl;
        throw 1; // throw something meaningful later
      }
      this->ptrGridWorld = ptrGridWorld;
      this->agentId = agentId;
      this->position = p;
  }
  AgentInformation Agent::getAgentInformation() {
    AgentInformation agentInformation;
    agentInformation.agentId = this->agentId;
    agentInformation.typeAgent = this->typeAgent;
    agentInformation.position = this->position;
    return agentInformation;
  }
}

