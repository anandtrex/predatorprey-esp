/*
 * PredatorEsp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "PredatorEsp.h"
#include <algorithm>
#include <limits>
#include <sstream>
#include "../Esp/NetworkContainerSelection.h"

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;
    using std::ostringstream;
    using EspPredPreyHunter::NetworkContainerSelection;

    PredatorEsp::PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            NetworkContainer* networkContainer)
            : Predator(ptrGridWorld, agentId, p), networkContainer(networkContainer)
    {
    }

    Position PredatorEsp::move(const std::vector<AgentInformation>& vAgentInformation,
            const uint& stepNo)
    {
        VLOG(5) << "Moving";

        ostringstream ss;

        typedef vector<AgentInformation>::const_iterator VAICI;
        vector<double> tempInput = vector<double>();

        // TODO Important!! This has to be a properly ordered list given as input later on.
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == PREY) {
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceX(this->position, itAgent->position))/ptrGridWorld->getWidth());
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceY(this->position, itAgent->position))/ptrGridWorld->getHeight());
                // NOTE Type should always be AFTER position
                tempInput.push_back(PREY);
                ss << PREY << " " << ptrGridWorld->distanceX(this->position, itAgent->position)
                        << " " << ptrGridWorld->distanceY(this->position, itAgent->position) << " ";
            }
        }
        VLOG(5) << "Done prey";
        for (VAICI itAgent = vAgentInformation.begin(); itAgent != vAgentInformation.end();
                ++itAgent) {
            if (itAgent->agentType == HUNTER) {
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceX(this->position, itAgent->position))/ptrGridWorld->getWidth());
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceY(this->position, itAgent->position))/ptrGridWorld->getHeight());
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER);

                ss << HUNTER << " " << ptrGridWorld->distanceX(this->position, itAgent->position)
                        << " " << ptrGridWorld->distanceY(this->position, itAgent->position) << " ";

                /*
                 // FIXME Hack to handle another network that takes in same input
                 tempInput.push_back(ptrGridWorld->distanceX(this->position, itAgent->position));
                 tempInput.push_back(ptrGridWorld->distanceY(this->position, itAgent->position));
                 // NOTE Type should always be AFTER position
                 tempInput.push_back(HUNTER);*/

            } else if (itAgent->agentType == HUNTER_WEAK) {
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceX(this->position, itAgent->position))/ptrGridWorld->getWidth());
                tempInput.push_back(static_cast<double>(ptrGridWorld->distanceY(this->position, itAgent->position))/ptrGridWorld->getHeight());
                // NOTE Type should always be AFTER position
                tempInput.push_back(HUNTER_WEAK);
                ss << HUNTER_WEAK << " "
                        << ptrGridWorld->distanceX(this->position, itAgent->position) << " "
                        << ptrGridWorld->distanceY(this->position, itAgent->position) << " ";

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
        if (networkContainer->getNetworkContainerType() == SELECTION) {
            ss << dynamic_cast<NetworkContainerSelection*>(networkContainer)->getNetworkSelected();
        } else {
            ss << "NA";
        }
        lastSelection = ss.str();

        Action predatorAction = (Action) getMaxIndex(output);
        position = ptrGridWorld->move(position, predatorAction);

        VLOG(5) << "Moved";
        return position;
    }
}

