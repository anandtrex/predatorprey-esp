/*
 * EspExperimentSubtaskInvertedInverted.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "EspExperimentSubtaskInverted.h"
#include "../Domain/GridWorld.h"
#include "../Domain/Domain.h"
#include "../Esp/NetworkContainerEsp.h"
#include "../Esp/NetworkContainerSubtaskInverted.h"

#include <limits>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using PredatorPreyHunter::Domain;
    using PredatorPreyHunter::fetchRandomDouble;

    EspExperimentSubtaskInverted::EspExperimentSubtaskInverted(const char* configFilePath)
            : EspExperiment()
    {
        libconfig::Config cfg;

        LOG(INFO) << "Reading from config file " << configFilePath;
        cfg.readFile(configFilePath);

        uint maxSteps = cfg.lookup("experiment:max_steps");
        LOG(INFO) << "Max steps in experiment is "
                << static_cast<uint>(cfg.lookup("experiment:max_steps"));

        uint gridWidth = cfg.lookup("experiment:grid:width");
        uint gridHeight = cfg.lookup("experiment:grid:height");
        LOG(INFO) << "Grid size is " << static_cast<uint>(gridWidth) << "x"
                << static_cast<uint>(gridHeight);

        uint numHunters = static_cast<uint>(cfg.lookup("agents:hunter:number"));
        uint numPredators = static_cast<uint>(cfg.lookup("agents:predator:number"));
        uint numPrey = static_cast<uint>(cfg.lookup("agents:prey:number"));

        // TODO Restructure this to use a single domain variable
        domainTotal = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                numHunters,
                static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability")),
                static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability")));
        LOG(INFO) << "Initialized overall domain with " << numPredators << " predators,"
                << numPrey << " prey and " << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"))
                << " and prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));

        domainPrey = new DomainPrey(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability")));
        LOG(INFO) << "Initialized prey domain with " << numPredators << " predators," << numPrey
                << " prey.";
        LOG(INFO) << " Prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));
        domainHunter = new DomainHunter(maxSteps, gridWidth, gridHeight, numPredators, numHunters,
                static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability")));

        LOG(INFO) << "Initialized hunter domain with " << numPredators << " predators,"
                << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"));

        popSize = cfg.lookup("experiment:esp:population_size");

        // TODO Configurable number of actions
        espDomainTotal = new Esp(static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons")),
                popSize, 0, 1,  domainTotal->getNumOtherAgents() * 2 + 10, 2); // FIXME: Hardcoded values
        LOG(INFO) << "Initialized esp for the total domain with "
                << static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons"))
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of other agents as " << domainTotal->getNumOtherAgents() << "and "
                << "number of actions as " << 5;

        espDomainPrey = new Esp(static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons")),
                popSize, 0, domainPrey->getNumOtherAgents(), 2, 5); // FIXME: Hardcoded values
        LOG(INFO) << "Initialized esp for prey domain with "
                << static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons"))
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of other agents as " << domainPrey->getNumOtherAgents() << "and "
                << "number of actions as " << 5;

        espDomainHunter = new Esp(static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons")),
                popSize, 0, domainHunter->getNumOtherAgents(), 2, 5); // FIXME: Hardcoded values
        LOG(INFO) << "Initialized esp for the hunter domain with "
                << static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons"))
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of other agents as " << domainHunter->getNumOtherAgents() << "and "
                << "number of actions as " << 5;

        numGenerations = cfg.lookup("experiment:esp:num_generations");
        LOG(INFO) << "Number of generations is " << numGenerations;

        // Display related stuff
        bool displayEnabled = cfg.lookup("experiment:display");

        if (displayEnabled) {
            LOG(INFO) << "Display enabled";

            vector<vector<double> > colors = vector<vector<double> >();
            vector<double> color = vector<double>();
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:r")));
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:g")));
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:b")));
            colors.push_back(color);
            color = vector<double>();
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:r")));
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:g")));
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:b")));
            colors.push_back(color);

            color = vector<double>();
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:r")));
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:g")));
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:b")));
            colors.push_back(color);

            domainTotal->enableDisplay(colors[0], colors[1], colors[2]);
            domainPrey->enableDisplay(colors[0], colors[1]);
            domainHunter->enableDisplay(colors[0], colors[2]);
        }
        //domainTotal->disableDisplay();
    }

    /**
     * This pretty much runs the entire ESP experiment
     */
    void EspExperimentSubtaskInverted::start()
    {
        LOG(INFO) << "Evolving for subtask 1";
        NetworkContainer* networkContainerSt1 = evolve(domainPrey,  new NetworkContainerEsp(), espDomainPrey, false);
        LOG(INFO) << "Subtask 1 done";
        LOG(INFO) << "Evolving for subtask 2";
        NetworkContainer* networkContainerSt2 = evolve(domainHunter,  new NetworkContainerEsp(), espDomainHunter, false);
        LOG(INFO) << "Subtask 2 done";
        vector<NetworkContainer*> networkContainers = vector<NetworkContainer*>();
        networkContainers.push_back(networkContainerSt1);
        networkContainers.push_back(networkContainerSt2);
        LOG(INFO) << "Evolving for overall task";
        NetworkContainer* networkContainerOt = new NetworkContainerSubtaskInverted(networkContainers);
        evolve(domainTotal, networkContainerOt, espDomainTotal, true);
        LOG(INFO) << "Overall task done";
    }
}


