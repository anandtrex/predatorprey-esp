/*
 * Gridworld.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include "GridWorld.h"
#include <cstdlib>
#include <iostream>
#include <math.h>

namespace PredatorPreyHunter
{
    using std::endl;
    using std::abs;

    uint GridWorld::distance(const Position& p1, const Position& p2) const
    {
        int distX, distY;
        distX = distY = 0;
        distX = abs(p1.x - p2.x);
        if (distX > (width / 2)) {
            distX = width - distX;
        }
        distY = abs(p1.y - p2.y);
        if (distY > (height / 2)) {
            distY = height - distY;
        }
        if (distX < 0 || distY < 0) {
            LOG(ERROR) << "GridWorld::distance distX or distY is less than 0" << endl;
        }
        return static_cast<uint>(sqrt(distX * distX + distY * distY));
    }

    uint GridWorld::distanceX(const Position& p1, const Position& p2) const
    {
        int distX;
        distX = abs(p1.x - p2.x);
        if (distX > (width / 2)) {
            distX = width - distX;
        }
        if (distX < 0) {
            LOG(ERROR) << "GridWorld::distanceX distX < 0!" << endl;
        }
        return static_cast<uint>(distX);
    }

    uint GridWorld::distanceY(const Position& p1, const Position& p2) const
    {
        int distY;
        distY = abs(p1.y - p2.y);
        if (distY > (height / 2)) {
            distY = height - distY;
        }
        if (distY < 0) {
            LOG(ERROR) << "GridWorld::distanceY distY < 0!" << endl;
        }
        return static_cast<uint>(distY);
    }

    Position GridWorld::move(const Position& pCurrent, const Action& action) const
    {
        Position pNew = pCurrent;
        if (NORTH == action) {
            pNew.y = pNew.y + 1;
        } else if (SOUTH == action) {
            pNew.y = pNew.y - 1;
        } else if (EAST == action) {
            pNew.x = pNew.x + 1;
        } else if (WEST == action) {
            pNew.x = pNew.x - 1;
        } else if (STAY == action) {
            // Do nothing
        } else {
            LOG(ERROR) << "Unknown action";
        }
        if (pNew.x < 0) {
            pNew.x = width - 1;
        }
        if (pNew.x >= width) {
            pNew.x = 0;
        }
        if (pNew.y < 0) {
            pNew.y = height - 1;
        }
        if (pNew.y >= height) {
            pNew.y = 0;
        }
        return pNew;
    }

    Position GridWorld::getRandomPosition()
    {
        Position position;
        position.x = static_cast<int>(fetchRandomDouble() * width);
        position.y = static_cast<int>(fetchRandomDouble() * height);
        return position;
    }
}

