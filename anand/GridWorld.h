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
    public:
        uint width;
        uint height;

        GridWorld(const int& width, const int& height)
                : width(width), height(height)
        {
        }

        uint distance(const Position& p1, const Position& p2) const;
        uint distanceX(const Position& p1, const Position& p2) const;
        uint distanceY(const Position& p1, const Position& p2) const;

        /**
         * Calculates and returns the new position of the agent for the given action.
         * NOTE: This class doesn't keep track of the current positions of the agents.
         * @param pCurrent - Current position
         * @param action - Action taken
         * @return
         */
        Position move(const Position& pCurrent, const Action& action) const;

        uint getWidth() const
        {
            return width;
        }
        uint getHeight() const
        {
            return height;
        }

        Position getRandomPosition();
    };
}

#endif /* GRIDWORLD_H_ */
