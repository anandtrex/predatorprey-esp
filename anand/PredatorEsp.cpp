/*
 * PredatorEsp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "PredatorEsp.h"
#include <algorithm>

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;

    PredatorEsp::PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            NetworkContainer* networkContainer) :
            Predator(ptrGridWorld, agentId, p), networkContainer(networkContainer)
    {
    }

    Position PredatorEsp::move(const std::vector<AgentInformation>& vAgentInformation)
    {
        typedef vector<AgentInformation>::const_iterator VAICI;
        vector<double> tempInput = vector<double>();

        // TODO Important!! This has to be a properly ordered list given as input later on.
        VAICI itPreyClosest = vAgentInformation.end();
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == PREY) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
            }
        }
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == HUNTER) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
            }
        }

        vector<double> input = vector<double>(tempInput);

        // random predator
        vector<double> output = vector<double>(5);
        networkContainer->activate(input, output);
        Action predatorAction = (Action)getMaxIndex(output);
        position = ptrGridWorld->move(this->position, predatorAction);
        return position;
    }

    uint PredatorEsp::getMaxIndex(const vector<double>& vec)
    {
        return std::distance(vec.begin(), max_element(vec.begin(), vec.end()));
    }
}

