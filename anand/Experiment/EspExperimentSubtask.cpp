/*
 * EspExperimentSubtask.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: anand
 */

#include "EspExperimentSubtask.h"
#include "../Esp/NetworkContainerEsp.h"
#include "../Esp/NetworkContainerCombiner.h"
#include "../Esp/NetworkContainerSelection.h"

#include <limits>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using PredatorPreyHunter::fetchRandomDouble;

    template<class T>
    EspExperimentSubtask<T>::EspExperimentSubtask(const string& configFilePath)
            : EspExperiment()
    {
        libconfig::Config cfg;

        LOG(INFO) << "Reading from config file " << configFilePath;
        cfg.readFile(configFilePath.c_str());

        const uint maxSteps = cfg.lookup("experiment:max_steps");
        LOG(INFO) << "Max steps in experiment is "
                << static_cast<uint>(cfg.lookup("experiment:max_steps"));

        const uint gridWidth = cfg.lookup("experiment:grid:width");
        const uint gridHeight = cfg.lookup("experiment:grid:height");
        LOG(INFO) << "Grid size is " << static_cast<uint>(gridWidth) << "x"
                << static_cast<uint>(gridHeight);

        coevolve = cfg.lookup("experiment:esp:coevolve");
        LOG(ERROR) << "Co-evolve is " << coevolve;

        numHunters = static_cast<uint>(cfg.lookup("agents:hunter:number"));
        const uint numPredators = static_cast<uint>(cfg.lookup("agents:predator:number"));
        const uint numPrey = static_cast<uint>(cfg.lookup("agents:prey:number"));

        const uint numNonPredAgents = numPrey + numHunters;

        const double preyMoveProb = static_cast<double>(cfg.lookup(
                "agents:prey:preys:[0]:move_probability"));

        domainPrey = new DomainOne<Prey>(maxSteps, gridWidth, gridHeight, preyMoveProb);
        LOG(INFO) << "Initialized prey domain";
        LOG(INFO) << " Prey move probability is " << preyMoveProb;

        const uint hunterType = static_cast<uint>(cfg.lookup("agents:hunter:hunters:[0]:type"));
        const double hunterMoveProb = static_cast<double>(cfg.lookup(
                "agents:hunter:hunters:[0]:move_probability"));

        domainHunter = new DomainOne<Hunter>(maxSteps, gridWidth, gridHeight, hunterMoveProb);
        LOG(INFO) << "Initialized hunter domain";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"));

        // Changing hunter type only in overall domain
        LOG(INFO) << "Initializing overall domain with " << numPredators << " predators,"
                << numPrey << " prey and " << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is " << hunterMoveProb
                << " and prey move probability is " << preyMoveProb;
        if (hunterType == 0) {
            domainTotal = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                    numHunters, preyMoveProb - 0.1, hunterMoveProb - 0.1);
        } else if (hunterType == 1) {
            const double hunterRoleReversalProb = static_cast<double>(cfg.lookup(
                    "agents:hunter:hunters:[0]:role_reversal_probability"));
            domainTotal = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey,
                    numHunters, preyMoveProb - 0.1, hunterMoveProb - 0.1, hunterRoleReversalProb);
            LOG(INFO) << "Hunter has MPD. Role reversal probability is "
                    << hunterRoleReversalProb;
        }

        const uint popSize = static_cast<uint>(cfg.lookup("experiment:esp:population_size"));
        const uint numHiddenNeurons = static_cast<uint>(cfg.lookup(
                "experiment:esp:num_hidden_neurons"));
        const uint netType = static_cast<uint>(cfg.lookup("experiment:esp:net_type"));
        // TODO Configurable number of actions

        const int numInputsPerNetwork = 2;
        const int numOutputsPerNetwork = 5;

        networkContainerTotal = generateNetworkContainer(numHiddenNeurons, netType,
                numOutputsPerNetwork, numNonPredAgents, numInputsPerNetwork, popSize);
        LOG(INFO) << "Initialising prey network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << numPrey << " number of inputs per network as "
                << numInputsPerNetwork << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerPrey = new NetworkContainerEsp(numHiddenNeurons, popSize, netType, 1,
                numInputsPerNetwork, numOutputsPerNetwork);
        LOG(INFO) << "Initialising hunter network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << 1 << " number of inputs per network as "
                << numInputsPerNetwork << " and number of outputs as " << numOutputsPerNetwork;
        networkContainerHunter = new NetworkContainerEsp(numHiddenNeurons, popSize, netType, 1,
                numInputsPerNetwork, numOutputsPerNetwork);

        numGenerations = cfg.lookup("experiment:esp:num_generations");
        LOG(INFO) << "Number of generations is " << numGenerations;

        numEvalTrials = cfg.lookup("experiment:esp:num_eval_trials");
        LOG(INFO) << "Number of trials for evaluation is " << numEvalTrials;

        // NOTE
        numTrialsPerGen = popSize * 10;
        LOG(INFO) << "Number of trials per generation is " << numTrialsPerGen;
    }

    /**
     * This pretty much runs the entire ESP experiment
     */
    template<class T>
    void EspExperimentSubtask<T>::start()
    {
        NetworkContainer* networkContainerChase = networkContainerPrey;
        NetworkContainer* networkContainerEvade = networkContainerHunter;

        const uint genPerStep = 5;

        if (coevolve) {
            LOG(INFO) << "Co-evolving";
            for (uint i = 0; i < numGenerations / genPerStep; i++) {
                LOG(INFO) << "Evolving for subtask with prey";
                networkContainerChase = evolve(domainPrey, networkContainerChase, genPerStep);
                LOG(INFO) << "Subtask 1 done";

                LOG(INFO) << "Evolving for subtask with hunter";
                networkContainerEvade = evolve(domainHunter, networkContainerEvade, genPerStep);
                LOG(INFO) << "Subtask 2 done";
                vector<NetworkContainer*> networkContainers = vector<NetworkContainer*>();

                // NOTE This order is important! First the prey network, then the hunter network. This is
                // because this is the order in which the inputs are given by the predator, and used
                // while activating

                networkContainers.push_back(networkContainerChase);
                for (uint i = 0; i < numHunters; i++) {
                    networkContainers.push_back(networkContainerEvade);
                }
                LOG(INFO) << "Evolving for overall task";
                (dynamic_cast<T*>(networkContainerTotal))->setNetworkContainers(networkContainers);
                networkContainerTotal = evolve(domainTotal, networkContainerTotal, genPerStep);
                LOG(INFO) << "Overall task done";
            }
        } else {
            LOG(INFO) << "Not co-evolving";
            LOG(INFO) << "Evolving for subtask with prey";
            networkContainerChase = evolve(domainPrey, networkContainerChase, numGenerations);
            LOG(INFO) << "Subtask 1 done";

            LOG(INFO) << "Evolving for subtask with hunter";
            networkContainerEvade = evolve(domainHunter, networkContainerEvade, numGenerations);
            LOG(INFO) << "Subtask 2 done";
            vector<NetworkContainer*> networkContainers = vector<NetworkContainer*>();

            // NOTE This order is important! First the prey network, then the hunter network. This is
            // because this is the order in which the inputs are given by the predator, and used
            // while activating

            networkContainers.push_back(networkContainerChase);
            for (uint i = 0; i < numHunters; i++) {
                networkContainers.push_back(networkContainerEvade);
            }
            LOG(INFO) << "Evolving for overall task";
            (dynamic_cast<T*>(networkContainerTotal))->setNetworkContainers(networkContainers);
            networkContainerTotal = evolve(domainTotal, networkContainerTotal, numGenerations);
            LOG(INFO) << "Overall task done";
        }
    }

    template<>
    NetworkContainer* EspExperimentSubtask<NetworkContainerCombiner>::generateNetworkContainer(
            const uint& numHiddenNeurons, const uint& netType, const uint& numOutputsPerNetwork,
            const uint& numNonPredAgents, const uint& numInputsPerNetwork, const uint& popSize)
    {
        const int numInputs = numOutputsPerNetwork * (numNonPredAgents + 1)
                + (numInputsPerNetwork + 1) * (numNonPredAgents)/* extra agent type input*/
                ;
        LOG(INFO) << "Initialising total network container for combiner with "
                << numHiddenNeurons << " number of hidden neurons, " << "population size "
                << popSize << ", number of networks " << 1 << ", number of inputs per network "
                << numInputs << " and number of outputs as " << numOutputsPerNetwork;
        return new NetworkContainerCombiner(numHiddenNeurons, popSize, netType, 1, numInputs,
                numOutputsPerNetwork);
    }

    template<>
    NetworkContainer* EspExperimentSubtask<NetworkContainerSelection>::generateNetworkContainer(
            const uint& numHiddenNeurons, const uint& netType, const uint& numOutputsPerNetwork,
            const uint& numNonPredAgents, const uint& numInputsPerNetwork, const uint& popSize)
    {
        const int numInputs = (numInputsPerNetwork + 1) /* extra agent type input*/
        * (numNonPredAgents);
        const int numOutputs = numNonPredAgents;
        LOG(INFO) << "Initialising total network container for selection with "
                << numHiddenNeurons << " number of hidden neurons, " << "population size "
                << popSize << ", number of networks " << 1 << ", number of inputs per network as "
                << numInputs << " and number of outputs as " << numOutputs;
        return new NetworkContainerSelection(numHiddenNeurons, popSize, netType, 1, numInputs,
                numOutputs);
    }

    template class EspExperimentSubtask<NetworkContainerCombiner> ;
    template class EspExperimentSubtask<NetworkContainerSelection> ;
}

