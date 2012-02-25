/*
 * EspExperimentSubtask.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "EspExperimentSubtask.h"
#include "../Domain/GridWorld.h"
#include "../Domain/Domain.h"
#include "../Esp/NetworkContainerEsp.h"
#include "../Esp/NetworkContainerSubtask.h"

#include <limits>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using PredatorPreyHunter::Domain;
    using PredatorPreyHunter::fetchRandomDouble;

    EspExperimentSubtask::EspExperimentSubtask(const char* configFilePath)
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

        const uint popSize = static_cast<uint>(cfg.lookup("experiment:esp:population_size"));
        const uint numHiddenNeurons = static_cast<uint>(cfg.lookup(
                "experiment:esp:num_hidden_neurons"));
        const uint netType = static_cast<uint>(cfg.lookup("experiment:esp:net_type"));
        // TODO Configurable number of actions

        const int numInputsPerNetwork = 3;
        const int numOutputsPerNetwork = 5;

        LOG(INFO) << "Initialising total network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << 1 << " number of inputs per network as "
                << numOutputsPerNetwork * 2 + numInputsPerNetwork << " and number of outputs as "
                << numOutputsPerNetwork;
        networkContainerTotal = new NetworkContainerSubtask(numHiddenNeurons, popSize, netType, 1,
                numOutputsPerNetwork * 2 + numInputsPerNetwork, numOutputsPerNetwork);
        LOG(INFO) << "Initialising network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << numPrey << " number of inputs per network as "
                << numInputsPerNetwork << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerPrey = new NetworkContainerEsp(numHiddenNeurons, popSize, netType, numPrey,
                numInputsPerNetwork, numOutputsPerNetwork);
        LOG(INFO) << "Initialising network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << numHunters << " number of inputs per network as "
                << numInputsPerNetwork << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerHunter = new NetworkContainerEsp(numHiddenNeurons, popSize, netType,
                numHunters, numInputsPerNetwork, numOutputsPerNetwork);

        numGenerations = cfg.lookup("experiment:esp:num_generations");
        LOG(INFO) << "Number of generations is " << numGenerations;

        numEvalTrials = 5;
        LOG(INFO) << "Number of trials for evaluation is " << numEvalTrials;

        // NOTE
        numTrialsPerGen = popSize * 10;
        LOG(INFO) << "Number of trials per generation is " << numTrialsPerGen;

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
    void EspExperimentSubtask::start()
    {
        NetworkContainer* networkContainerSt1;

        LOG(INFO) << "Evolving for subtask 1";
        networkContainerSt1 = evolve(domainPrey, networkContainerPrey);
        LOG(INFO) << "Subtask 1 done";
        VLOG(5) << "Num networks in st1 is " << networkContainerSt1->getNetworks().size();

        LOG(INFO) << "Evolving for subtask 2";
        NetworkContainer* networkContainerSt2 = evolve(domainHunter, networkContainerHunter);
        LOG(INFO) << "Subtask 2 done";
        VLOG(5) << "Num networks in st2 is " << networkContainerSt2->getNetworks().size();
        vector<NetworkContainer*> networkContainers = vector<NetworkContainer*>();
        networkContainers.push_back(networkContainerSt1);
        networkContainers.push_back(networkContainerSt2);
        LOG(INFO) << "Evolving for overall task";
        VLOG(5) << "NetworkContainers size is " << networkContainers.size();
        (dynamic_cast<NetworkContainerSubtask*>(networkContainerTotal))->setNetworkContainers(
                networkContainers);
        evolve(domainTotal, networkContainerTotal);
        LOG(INFO) << "Overall task done";
    }
}

