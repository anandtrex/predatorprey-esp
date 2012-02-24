/*
 * PredatorEsp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "PredatorEsp.h"
#include <algorithm>
#include <limits>

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;
    using std::numeric_limits;

    PredatorEsp::PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            NetworkContainer* networkContainer) :
            Predator(ptrGridWorld, agentId, p), networkContainer(networkContainer)
    {
    }

    Position PredatorEsp::move(const std::vector<AgentInformation>& vAgentInformation)
    {
        VLOG(5) << "Moving";
        typedef vector<AgentInformation>::const_iterator VAICI;
        vector<double> tempInput = vector<double>();

        // TODO Important!! This has to be a properly ordered list given as input later on.
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == PREY) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                tempInput.push_back(PREY);
            }
        }
        VLOG(5) << "Done prey";
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == HUNTER) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                tempInput.push_back(HUNTER);
            }
        }
        VLOG(5) << "Done hunter";

        vector<double> input = vector<double>(tempInput);

        vector<double> output = vector<double>(5);
        networkContainer->activate(input, output);
        Action predatorAction = (Action)getMaxIndex(output);
        VLOG(2) << "Action selected is " << predatorAction;
        position = ptrGridWorld->move(position, predatorAction);
        VLOG(5) << "Moved";
        return position;
    }

    uint PredatorEsp::getMaxIndex(const vector<double>& vec)
    {
        VLOG(5) << "Returning the max index";
        //return std::distance(vec.begin(), max_element(vec.begin(), vec.end()));
        vector<uint> maxIndexes = vector<uint>();
        double maxValue = -numeric_limits<double>::max();

        vector<double>::const_iterator it = vec.begin();
        while(it != vec.end())
        {
            VLOG(5) << *it;
            if(*it > maxValue){
                maxValue = *it;
                maxIndexes = vector<uint>();
                maxIndexes.push_back(std::distance(vec.begin(), it));
            } else if (*it == maxValue) {
                maxIndexes.push_back(std::distance(vec.begin(), it));
            }
            it++;
        }
        if(maxIndexes.size() == 1){
            VLOG(5) << "Only one max index";
            return maxIndexes[0];
        }
        else {
            VLOG(5) << "Multiple max indexes";
            return maxIndexes[fetchRandomLong() % (maxIndexes.size() - 1)];
        }
    }
}

