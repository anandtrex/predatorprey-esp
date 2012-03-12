/*
 * Experiment.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#include "EspExperiment.h"
#include "../Domain/Domain.h"
#include "../Esp/NetworkContainer.h"
#include "../Esp/NetworkContainerEsp.h"

#include <limits>
#include <algorithm>
#include <fstream>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using std::max;
    using std::ofstream;
    using PredatorPreyHunter::Domain;
    using PredatorPreyHunter::fetchRandomDouble;

    EspExperiment::EspExperiment(const string& configFilePath)
            : Experiment()
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

        const uint numHunters = static_cast<uint>(cfg.lookup("agents:hunter:number"));
        const uint numPredators = static_cast<uint>(cfg.lookup("agents:predator:number"));
        const uint numPrey = static_cast<uint>(cfg.lookup("agents:prey:number"));

        const uint hunterType = static_cast<uint>(cfg.lookup("agents:hunter:hunters:[0]:type"));
        const double hunterMoveProb = static_cast<double>(cfg.lookup(
                "agents:hunter:hunters:[0]:move_probability"));
        const double preyMoveProb = static_cast<double>(cfg.lookup(
                "agents:prey:preys:[0]:move_probability"));

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
        LOG(INFO) << "Initialized domain with " << numPredators << " predators," << numPrey
                << " prey and " << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"))
                << " and prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));

        const uint popSize = static_cast<uint>(cfg.lookup("experiment:esp:population_size"));
        const uint numHiddenNeurons = static_cast<uint>(cfg.lookup(
                "experiment:esp:num_hidden_neurons"));
        const uint netType = static_cast<uint>(cfg.lookup("experiment:esp:net_type"));
        // TODO Configurable number of actions

        const int numInputsPerNetwork = 3;
        const int numOutputsPerNetwork = 5;

        LOG(INFO) << "Initialising network container with " << numHiddenNeurons
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of networks as " << numHunters + numPrey
                << " number of inputs per network as " << numInputsPerNetwork
                << " and number of outputs as " << numOutputsPerNetwork;

        networkContainerTotal = new NetworkContainerEsp(numHiddenNeurons, popSize, netType,
                numHunters + numPrey, numInputsPerNetwork, numOutputsPerNetwork);

        numGenerations = cfg.lookup("experiment:esp:num_generations");
        LOG(INFO) << "Number of generations is " << numGenerations;

        numEvalTrials = cfg.lookup("experiment:esp:num_eval_trials");
        LOG(INFO) << "Number of trials for evaluation is " << numEvalTrials;

        // NOTE
        numTrialsPerGen = popSize * 10;
        LOG(INFO) << "Number of trials per generation is " << numTrialsPerGen;

        // Display related stuff
        const bool displayEnabled = cfg.lookup("experiment:display");

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
        }
    }

    void EspExperiment::saveNetworkToFile(const string& fileName,
            NetworkContainer* networkContainer)
    {
        ofstream fout;
        fout.open(fileName.c_str());
        fout << networkContainer->toString();
        fout.close();
    }

    /**
     * This pretty much runs the entire ESP experiment
     */
    void EspExperiment::start()
    {
        // TODO Change this
        LOG(INFO) << "Starting evolve";
        evolve(domainTotal, networkContainerTotal);
    }

    NetworkContainer* EspExperiment::evolve(Domain* domain, NetworkContainer* networkContainer)
    {
        vector<NetworkContainer*> hallOfFame = vector<NetworkContainer*>();
        NetworkContainer* generationBestNetwork;

        double fitness;
        double genMaxFitness;
        double genAverageFitness;
        VLOG(4) << "Initializing networks";
        networkContainer->initializeNetworks();
        VLOG(4) << "Done initializing networks";

        for (uint generation = 1; generation <= numGenerations; generation++) {
            genMaxFitness = -numeric_limits<double>::max();     // Minimum possible float value
            genAverageFitness = 0.0;
            networkContainer->evalReset();
            for (uint trial = 0; trial < numTrialsPerGen; trial++) {
                VLOG(4) << "Initializing networks";
                networkContainer->initializeNetworks();
                VLOG(4) << "Done initializing networks";
                networkContainer->incrementTests();
                fitness = 0.0;
                for (uint evalTrial = 0; evalTrial < numEvalTrials; evalTrial++) {
                    LOG(INFO) << "evalTrial is " << evalTrial;
                    domain->init(networkContainer);
                    double tempfitness = domain->run();
                    if (tempfitness > 10 * domain->getMaxSteps()) {
                        LOG(FATAL) << "Too high fitness " << tempfitness << "!!";
                    }
                    fitness += tempfitness;
                }
                fitness /= numEvalTrials;
                if (fitness > 10.0 * domain->getMaxSteps()) {
                    LOG(FATAL) << "Too high fitness " << fitness << "!!";
                }
                genAverageFitness += fitness;
                networkContainer->setFitness(fitness);
                if (fitness > genMaxFitness) {
                    LOG(INFO) << "Fitness is " << fitness << " and gen max fitness is "
                            << genMaxFitness;
                    genMaxFitness = fitness;
                    generationBestNetwork = networkContainer;
                }
            }
            hallOfFame.push_back(generationBestNetwork);
            networkContainer->average();
            networkContainer->qsortNeurons();
            networkContainer->recombineHallOfFame(
                    hallOfFame[fetchRandomDouble() * (hallOfFame.size() - 1)]);
            networkContainer->mutate();

            LOG(INFO) << "Generation: " << generation;
            LOG(INFO) << "Generation max fitness was: " << genMaxFitness;
            genAverageFitness /= numTrialsPerGen;
            LOG(INFO) << "Generation average fitness was: " << genAverageFitness;
            foutGenAverage << generation << " " << genAverageFitness << "\n";
            foutGenMax << generation << " " << genMaxFitness << "\n";
            // Run it once with the generation champion to get file output
            const int tempDiff = (numGenerations - 20);
            if (generation > max(tempDiff, 0)) {
                domain->init(generationBestNetwork);
                domain->run(concatStringDouble("champion_", generation) + ".log");
                saveNetworkToFile(concatStringDouble("champion_", generation) + ".net",
                        generationBestNetwork);
            }
        }

        return hallOfFame[hallOfFame.size() - 1];
    }
}
