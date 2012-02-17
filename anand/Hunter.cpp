#include "Hunter.h"
#include <iostream>
#include <cstdlib>

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;

    Hunter::Hunter(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            const double& moveProbability)
            : Agent(ptrGridWorld, agentId, p), moveProbability(moveProbability)
    {
        this->typeAgent = HUNTER;
    }

    Position Hunter::move(const std::vector<AgentInformation>& vAgentInformation)
    {
        typedef vector<AgentInformation>::const_iterator VAICI;
        // if the random number between 0 and 1 is greater than the moveProbability
        // then don't move
        if (fetchRandomDouble() > moveProbability) {
            return this->position;
        }
        const int MAX_DISTANCE = ptrGridWorld->getWidth() + ptrGridWorld->getHeight();
        // find the closest predator
        int minDist, dist;
        minDist = MAX_DISTANCE;
        VAICI itPredatorClosest = vAgentInformation.end();
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == PREDATOR) {
                dist = ptrGridWorld->distance(this->position, itAgent->position);
                if (dist < minDist) {
                    minDist = dist;
                    itPredatorClosest = itAgent;
                }
            }
        }
        if (MAX_DISTANCE == minDist || vAgentInformation.end() == itPredatorClosest) {
            LOG(ERROR) << "Houston, we have a problem" << endl;
            LOG(ERROR) << "Hunter::move could not find the nearest predator." << endl;
        }

        // move away from the closest predator
        // the following code has been taken from Padmini and Aditya
        // uses the same conventions
        const int MAP_LENGTH = ptrGridWorld->getWidth();
        const int MAP_HEIGHT = ptrGridWorld->getHeight();
        int x_dist = itPredatorClosest->position.x - this->position.x; // NOTE: Predator - Hunter
        int temp;
        if ((abs(x_dist)) > (MAP_LENGTH / 2)) { // account for the toroid
            temp = x_dist;
            x_dist = MAP_LENGTH - abs(x_dist);
            if (temp > 0)
                x_dist = 0 - x_dist; // account for who is behind who
        }
        // do the same thing for y
        int y_dist = itPredatorClosest->position.y - this->position.y; // NOTE: Predator - Hunter
        if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
            temp = y_dist;
            y_dist = MAP_HEIGHT - abs(y_dist);
            if (temp > 0)
                y_dist = 0 - y_dist;
        }
        Action hunterAction;
        if (y_dist < 0 && (abs(y_dist) >= abs(x_dist))) {
            hunterAction = SOUTH; // NOTE: opposite of prey movement
        } else if (x_dist < 0 && (abs(x_dist) >= abs(y_dist))) {
            hunterAction = WEST;
        } else if (y_dist > 0 && (abs(y_dist) >= abs(x_dist))) {
            hunterAction = NORTH;
        } else if (x_dist > 0 && (abs(x_dist) >= abs(y_dist))) {
            hunterAction = EAST;
        } else {
            hunterAction = STAY;
        }
        this->position = ptrGridWorld->move(this->position, hunterAction);
        return this->position;
    }
}

