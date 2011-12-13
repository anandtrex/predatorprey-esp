#ifndef PPH_BASIC_H
#define PPH_BASIC_H

namespace PredatorPreyHunter {
  typedef unsigned int uint;
  struct Position {
    int x;
    int y;
  };
  enum TypeAgent {
    PREY = 0,
    PREDATOR = 1,
    HUNTER = 2
  };
  enum Action {
    STAY = 0,
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4
  };
  struct AgentInformation {
    uint agentId;
    TypeAgent typeAgent;
    Position position;
  };
}

#endif // PPH_BASIC_H

