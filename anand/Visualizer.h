/*
 * Visualize.h
 *
 *  Created on: Jan 30, 2012
 *      Author: anand
 */

#ifndef VISUALIZE_H_
#define VISUALIZE_H_

#include <vector>
#include <map>

#include "Agent.h"

namespace PredPreyHunterVisualizer
{
    class Visualizer
    {
        std::map<int, std::vector<double> > idColorMapping;
        uint pointSize;
    public:
        Visualizer(std::map<int, std::vector<double> > idColorMapping, int width, int height);
        Visualizer()
        {
            this->idColorMapping = std::map<int, std::vector<double> >();
        }
        void show(const std::vector<AgentInformation>& vAgentInformationPrevious,
                const std::vector<AgentInformation>& vAgentInformationCurrent);
    };
}

#endif /* VISUALIZE_H_ */
