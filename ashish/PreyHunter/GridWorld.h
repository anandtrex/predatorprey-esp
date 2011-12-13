#ifndef PPH_GRIDWORLD_H
#define PPH_GRIDWORLD_H

#include "Basic.h"

namespace PredatorPreyHunter {
  class GridWorld {
    int width;
    int height;
  public:
    GridWorld( const int& width, const int& height );
    int distance( const Position& p1, const Position& p2 ) const;
    int distanceX( const Position& p1, const Position& p2 ) const;
    int distanceY( const Position& p1, const Position& p2 ) const;
    Position move( const Position& pCurrent, const Action& action ) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
  };
}

#endif // PPH_GRIDWORLD_H

