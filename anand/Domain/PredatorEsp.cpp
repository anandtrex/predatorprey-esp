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

    PredatorEsp::PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            NetworkContainer* networkContainer)
            : Predator(ptrGridWorld, agentId, p), networkContainer(networkContainer)
    {
    }

    Position PredatorEsp::move(const std::vector<AgentInformation>& vAgentInformation,
            const uint& stepNo)
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
                // NOTE Type should always be AFTER position
                tempInput.push_back(PREY);
            }
        }
        VLOG(5) << "Done prey";
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == HUNTER) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER);

                /*
                // FIXME Hack to handle another network that takes in same input
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER);*/

            } else if (itAgent->agentType == HUNTER_WEAK) {
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER_WEAK);

                /*
                // FIXME Hack to handle another network that takes in same input
                tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER_WEAK);*/
            }
        }
        VLOG(5) << "Done hunter";

        vector<double> input = vector<double>(tempInput);

        // FIXME Number of actions hardcoded as 5!
        vector<double> output = vector<double>(5);
        networkContainer->activate(input, output);
        Action predatorAction = (Action) getMaxIndex(output);
        //LOG(INFO) << "Predator is at " << position.x << ", " << position.y;
        //LOG(INFO) << "Action selected by predator is " << predatorAction;
        position = ptrGridWorld->move(position, predatorAction);
        //LOG(INFO) << "Moved to " << position.x << ", " << position.y;
        VLOG(5) << "Moved";
        return position;
    }
}

