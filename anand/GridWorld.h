/*
 * Gridworld.h
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#ifndef GRIDWORLD_H_
#define GRIDWORLD_H_

#include "common.h"

struct Position
{
    int x;
    int y;
};

enum Action
{
    STAY = 0, NORTH = 1, EAST = 2, SOUTH = 3, WEST = 4
};

namespace PredatorPreyHunter
{
class GridWorld
{
    int width;
    int height;

public:
    GridWorld(const int& width, const int& height);
    uint distance(const Position& p1, const Position& p2) const;
    uint distanceX(const Position& p1, const Position& p2) const;
    uint distanceY(const Position& p1, const Position& p2) const;
    Position move(const Position& pCurrent, const Action& action) const;
    int getWidth() const
    {
        return width;
    }
    int getHeight() const
    {
        return height;
    }
};
}

#endif /* GRIDWORLD_H_ */
