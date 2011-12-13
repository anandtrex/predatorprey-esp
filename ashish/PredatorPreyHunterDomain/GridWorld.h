#ifndef PPH_GRIDWORLD_H
#define PPH_GRIDWORLD_H

#include "Basic.h"

namespace PredatorPreyHunter {
  class GridWorld {
    int width;
    int height;
  public:
    GridWorld( const int& width, const int& height );
    uint distance( const Position& p1, const Position& p2 ) const;
    uint distanceX( const Position& p1, const Position& p2 ) const;
    uint distanceY( const Position& p1, const Position& p2 ) const;
    Position move( const Position& pCurrent, const Action& action ) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
  };
}

#endif // PPH_GRIDWORLD_H

