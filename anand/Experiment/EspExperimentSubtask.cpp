/*
 * EspExperimentSubtask.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "EspExperimentSubtask.h"
#include "../Esp/NetworkContainerEsp.h"
#include "../Esp/NetworkContainerSubtask.h"
#include "../Esp/NetworkContainerSubtaskInverted.h"

#include <limits>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using PredatorPreyHunter::fetchRandomDouble;

    template<class T>
    EspExperimentSubtask<T>::EspExperimentSubtask(const char* configFilePath) :
            EspExperiment()
    {
        libconfig::Config cfg;

        LOG(INFO) << "Reading from config file " << configFilePath;
        cfg.readFile(configFilePath);

        const uint maxSteps = cfg.lookup("experiment:max_steps");
        LOG(INFO) << "Max steps in experiment is "
                << static_cast<uint>(cfg.lookup("experiment:max_steps"));

        const uint gridWidth = cfg.lookup("experiment:grid:width");
        const uint gridHeight = cfg.lookup("experiment:grid:height");
        LOG(INFO) << "Grid size is " << static_cast<uint>(gridWidth) << "x"
                << static_cast<uint>(gridHeight);

        const uint numHunters = static_cast<uint>(cfg.lookup("agents:hunter:number"));
        const uint numPredators = static_cast<uint>(cfg.lookup("agents:predator:number"));
        const uint numPrey = static_cast<uint>(cfg.lookup("agents:prey:number"));

        const uint numNonPredAgents = numPrey + numHunters;

        const double preyMoveProb = static_cast<double>(cfg.lookup(
                "agents:prey:preys:[0]:move_probability"));

        domainPrey = new DomainOne<Prey>(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                preyMoveProb);
        LOG(INFO) << "Initialized prey domain with " << numPredators << " predators," << numPrey
                << " prey.";
        LOG(INFO) << " Prey move probability is " << preyMoveProb;

        const uint hunterType = static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:type"));
        const double hunterMoveProb = static_cast<double>(cfg.lookup(
                "agents:hunter:hunters:[0]:move_probability"));

        domainHunter = new DomainOne<Hunter>(maxSteps, gridWidth, gridHeight, numPredators,
                numHunters, hunterMoveProb);
        LOG(INFO) << "Initialized hunter domain with " << numPredators << " predators,"
                << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"));

        // Changing hunter type only in overall domain
        LOG(INFO) << "Initializing overall domain with " << numPredators << " predators,"
                << numPrey << " prey and " << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"))
                << " and prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));
        if (hunterType == 0) {
            domainTotal = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                    numHunters, preyMoveProb - 0.1, hunterMoveProb - 0.1);
        } else if (hunterType == 1) {
            const double hunterRoleReversalProb = static_cast<double>(cfg.lookup(
                    "agents:hunter:hunters:[0]:role_reversal_probability"));
            domainTotal = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                    numHunters, preyMoveProb - 0.1, hunterMoveProb - 0.1, hunterRoleReversalProb);
            LOG(INFO) << "Hunter has MPD. Role reversal probability is " << hunterRoleReversalProb;
        }

        const uint popSize = static_cast<uint>(cfg.lookup("experiment:esp:population_size"));
        const uint numHiddenNeurons = static_cast<uint>(cfg.lookup(
                "experiment:esp:num_hidden_neurons"));
        const uint netType = static_cast<uint>(cfg.lookup("experiment:esp:net_type"));
        // TODO Configurable number of actions

        const int numInputsPerNetwork = 2;
        const int numOutputsPerNetwork = 5;

        LOG(INFO) << "Initialising total network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << 1 << " number of inputs per network as "
                << numOutputsPerNetwork * numNonPredAgents + numInputsPerNetwork * numNonPredAgents
                << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerTotal = generateNetworkContainer(numHiddenNeurons, netType,
                numOutputsPerNetwork, numNonPredAgents, numInputsPerNetwork, popSize);
        LOG(INFO) << "Initialising prey network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << numPrey << " number of inputs per network as "
                << numInputsPerNetwork << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerPrey = new NetworkContainerEsp(numHiddenNeurons, popSize, netType, numPrey,
                numInputsPerNetwork, numOutputsPerNetwork);
        LOG(INFO) << "Initialising hunter network container with " << numHiddenNeurons
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
    template<class T>
    void EspExperimentSubtask<T>::start()
    {
        LOG(INFO) << "Evolving for subtask 1";
        NetworkContainer* networkContainerChase = evolve(domainPrey, networkContainerPrey);
        LOG(INFO) << "Subtask 1 done";
        VLOG(5) << "Num networks in st1 is " << networkContainerChase->getNetworks().size();

        LOG(INFO) << "Evolving for subtask 2";
        NetworkContainer* networkContainerEvade = evolve(domainHunter, networkContainerHunter);
        LOG(INFO) << "Subtask 2 done";
        VLOG(5) << "Num networks in st2 is " << networkContainerEvade->getNetworks().size();
        vector<NetworkContainer*> networkContainers = vector<NetworkContainer*>();
        networkContainers.push_back(networkContainerChase);
        networkContainers.push_back(networkContainerEvade);
        networkContainers.push_back(networkContainerChase);
        LOG(INFO) << "Evolving for overall task";
        VLOG(5) << "NetworkContainers size is " << networkContainers.size();
        (dynamic_cast<T*>(networkContainerTotal))->setNetworkContainers(networkContainers);
        evolve(domainTotal, networkContainerTotal);
        LOG(INFO) << "Overall task done";
    }

    template<>
    NetworkContainer* EspExperimentSubtask<NetworkContainerSubtask>::generateNetworkContainer(
            const uint& numHiddenNeurons, const uint& netType, const uint& numOutputsPerNetwork,
            const uint& numNonPredAgents, const uint& numInputsPerNetwork, const uint& popSize)
    {
        // NOTE +1 is for newer network with 3 agents
        return new NetworkContainerSubtask(numHiddenNeurons, popSize, netType, 1,
                numOutputsPerNetwork * numNonPredAgents + (numInputsPerNetwork + 1)* numNonPredAgents,
                numOutputsPerNetwork);
    }

    template<>
    NetworkContainer* EspExperimentSubtask<NetworkContainerSubtaskInverted>::generateNetworkContainer(
            const uint& numHiddenNeurons, const uint& netType, const uint& numOutputsPerNetwork,
            const uint& numNonPredAgents, const uint& numInputsPerNetwork, const uint& popSize)
    {
        // NOTE +1 is for newer network with 3 agents
        return new NetworkContainerSubtaskInverted(numHiddenNeurons, popSize, netType, 1,
                (numInputsPerNetwork + 1) * numNonPredAgents, 2);
    }

    template class EspExperimentSubtask<NetworkContainerSubtask> ;
    template class EspExperimentSubtask<NetworkContainerSubtaskInverted> ;
}

