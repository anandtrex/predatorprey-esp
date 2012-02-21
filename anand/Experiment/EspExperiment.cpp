/*
 * Experiment.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#include "EspExperiment.h"
#include "../Domain/GridWorld.h"
#include "../Domain/Domain.h"
#include "../Esp/NetworkContainer.h"
#include "../Esp/NetworkContainerEsp.h"

#include <limits>
#include <algorithm>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using std::max;
    using PredatorPreyHunter::Domain;
    using PredatorPreyHunter::fetchRandomDouble;

    EspExperiment::EspExperiment(const char* configFilePath)
            : Experiment()
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

        domain = new DomainTotal(maxSteps, gridWidth, gridHeight, numPredators, numPrey, numHunters,
                static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability")),
                static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability")));
        LOG(INFO) << "Initialized domain with " << numPredators << " predators," << numPrey
                << " prey and " << numHunters << " hunters.";
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"))
                << " and prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));

        popSize = cfg.lookup("experiment:esp:population_size");
        // TODO Configurable number of actions
        esp = new Esp(static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons")), popSize,
                0, numHunters + numPrey, 2, 5);
        LOG(INFO) << "Initialized esp with "
                << static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons"))
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of other agents as " << numHunters + numPrey << "and "
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

            domain->enableDisplay(colors[0], colors[1], colors[2]);
        }
        //domain.disableDisplay();
    }

    /**
     * This pretty much runs the entire ESP experiment
     */
    void EspExperiment::start()
    {
        evolve(domain, new NetworkContainerEsp(), esp, false);
    }

    NetworkContainer* EspExperiment::evolve(Domain* domain, NetworkContainer* networkContainer, Esp* esp, bool append)
    {
        vector<NetworkContainer*> hallOfFame = vector<NetworkContainer*>();
        NetworkContainer* generationBestNetwork;

        uint numEvalTrials = 5;
        LOG(INFO) << "Number of trials for evaluation is " << numEvalTrials;

        // NOTE
        uint numTrialsPerGen = popSize * 10;
        LOG(INFO) << "Number of trials per generation is " << numTrialsPerGen;

        double fitness;
        double genMaxFitness;
        double genAverageFitness;
        double overallMaxFitness = static_cast<double>(-1) * numeric_limits<double>::max();     // Minimum possible float value

        for (uint generation = 0; generation < numGenerations; generation++) {
            genMaxFitness = -numeric_limits<double>::max();     // Minimum possible float value
            genAverageFitness = 0.0;
            esp->evalReset();
            for (uint trial = 0; trial < numTrialsPerGen; trial++) {
                networkContainer->setNetwork(*(esp->getNetwork()));
                networkContainer->incrementTests();
                fitness = 0.0;
                domain->init(networkContainer);
                for (uint evalTrial = 0; evalTrial < numEvalTrials; evalTrial++) {
                    fitness += domain->run();
                }
                fitness /= numEvalTrials;
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
            LOG(INFO) << "Generation average fitness was: "
                    << static_cast<double>(genAverageFitness);
            foutGenAverage << generation << " " << genAverageFitness << "\n";
            foutGenMax << generation << " " << genMaxFitness << "\n";
            // Run it once with the generation champion to get file output
            const int tempDiff = (numGenerations - 20);
            if(generation > max(tempDiff, 0)){
                domain->init(generationBestNetwork);
                domain->run(concatStringDouble("champion_", generation)+".log");
            }
        }

        return hallOfFame[hallOfFame.size() - 1];
    }
}
