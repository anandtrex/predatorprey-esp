/*
 * PredatorNaive.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#include "PredatorNaive.h"
#include <iostream>
#include <cstdlib>

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;

    Position PredatorNaive::move(const std::vector<AgentInformation>& vAgentInformation)
    {
        typedef vector<AgentInformation>::const_iterator VAICI;
        // if the random number between 0 and 1 is greater than the moveProbability
        // then don't move
        if (fetchRandomNumber() > moveProbability) {
            return this->position;
        }
        const int MAX_DISTANCE = ptrGridWorld->getWidth() + ptrGridWorld->getHeight();
        // find the closest prey
        int minDist, dist;
        minDist = MAX_DISTANCE;
        VAICI itPreyClosest = vAgentInformation.end();
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == PREY) {
                dist = ptrGridWorld->distance(this->position, itAgent->position);
                if (dist < minDist) {
                    minDist = dist;
                    itPreyClosest = itAgent;
                }
            }
        }
        if (MAX_DISTANCE == minDist || vAgentInformation.end() == itPreyClosest) {
            LOG(ERROR) << "Houston, we have a problem" << endl;
            LOG(ERROR) << "Predator::move could not find the nearest prey." << endl;
        }
        // move away from the closest predator
        // the following code has been taken from Padmini and Aditya
        // uses the same conventions
        const int MAP_LENGTH = ptrGridWorld->getWidth();
        const int MAP_HEIGHT = ptrGridWorld->getHeight();
        int x_dist = itPreyClosest->position.x - this->position.x; // NOTE: Prey - Predator
        int temp;
        if ((abs(x_dist)) > (MAP_LENGTH / 2)) { // account for the toroid
            temp = x_dist;
            x_dist = MAP_LENGTH - abs(x_dist);
            if (temp > 0)
                x_dist = 0 - x_dist; // account for who is behind who
        }
        // do the same thing for y
        int y_dist = itPreyClosest->position.y - this->position.y; // NOTE: Predator - Hunter
        if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
            temp = y_dist;
            y_dist = MAP_HEIGHT - abs(y_dist);
            if (temp > 0)
                y_dist = 0 - y_dist;
        }
        Action predatorAction;
        if (y_dist < 0 && (abs(y_dist) >= abs(x_dist))) {
            predatorAction = SOUTH; // NOTE: move towards prey
        } else if (x_dist < 0 && (abs(x_dist) >= abs(y_dist))) {
            predatorAction = WEST;
        } else if (y_dist > 0 && (abs(y_dist) >= abs(x_dist))) {
            predatorAction = NORTH;
        } else if (x_dist > 0 && (abs(x_dist) >= abs(y_dist))) {
            predatorAction = EAST;
        } else {
            predatorAction = STAY;
        }
        this->position = ptrGridWorld->move(this->position, predatorAction);
        return this->position;
    }
}

