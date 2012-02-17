/*
 * Experiment.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: anand
 */

#include "EspExperiment.h"
#include "GridWorld.h"
#include "Domain.h"

#include <limits>

#include <libconfig.h++>

namespace EspPredPreyHunter
{
    using std::map;
    using std::pair;
    using std::numeric_limits;
    using PredatorPreyHunter::Domain;
    using PredatorPreyHunter::fetchRandomDouble;

    EspExperiment::EspExperiment(const char* configFilePath)
        :Experiment(configFilePath)
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

        uint popSize = cfg.lookup("experiment:esp:population_size");
        // TODO Configurable number of actions
        esp = new Esp(static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons")), popSize, 0,
                numHunters + numPrey, 5);
        LOG(INFO) << "Initialized esp with " << static_cast<uint>(cfg.lookup("experiment:esp:num_hidden_neurons"))
                << " as number of hidden neurons, " << "population size " << popSize
                << ", number of other agents as " << numHunters + numPrey << "and "
                << "number of actions as " << 5;

        numGenerations = cfg.lookup("experiment:esp:num_generations");
        LOG(INFO) << "Number of generations is " << numGenerations;
        // NOTE
        numTrialsPerGen = popSize * 10;
        LOG(INFO) << "Number of trials per generation is " << numTrialsPerGen;

        numEvalTrials = 5;
        LOG(INFO) << "Number of trials for evaluation is " << numEvalTrials;

        hallOfFame = vector<NetworkContainer*>();

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
        // Evaluate a random population
        // Reset evaluation
        // Perform evaluation
        // For numtrials
        // Select neurons for each network
        // initialize fitnesses to 0
        // For eval trials
        // get fitness
        // Distribute fitness to each neuron
        // compute generation best
        // computer overall best
        // average fitness of neurons
        // recombine hall of fame
        // For each hidden neuron
        // qsort
        // call recombine_hall_of_fame
        // mutate

        NetworkContainer* networkContainer = new NetworkContainer();
        double fitness;
        double genMaxFitness = static_cast<double>(-1) * numeric_limits<double>::max(); // Minimum possible float value
        double genAverageFitness = 0.0;
        double overallMaxFitness = static_cast<double>(-1) * numeric_limits<double>::max(); // Minimum possible float value
        for (uint generation = 0; generation < numGenerations; generation++) {
            LOG(INFO) << "Generation max for generation " << generation << " is " << genMaxFitness;
            esp->evalReset();
            for (uint trial = 0; trial < numTrialsPerGen; trial++) {
                networkContainer = esp->getNetwork();     // selects random neurons from subpopulation for each network
                networkContainer->incrementTests();
                fitness = 0.0;
                domain->init(networkContainer);
                for (uint evalTrial = 0; evalTrial < numEvalTrials; evalTrial++) {
                    fitness += domain->run();
                }
                fitness /= numEvalTrials;
                genAverageFitness += fitness;
                networkContainer->setFitness(fitness);
                if(fitness > genMaxFitness){
                    LOG(INFO) << "Fitness is " << fitness << " and gen max fitness is " << genMaxFitness;
                    genMaxFitness = fitness;
                    generationBestNetwork = networkContainer;
                }
            }
            hallOfFame.push_back(generationBestNetwork);
            networkContainer->average();
            networkContainer->qsortNeurons();
            networkContainer->recombineHallOfFame(hallOfFame[fetchRandomDouble() * (hallOfFame.size() - 1)]);
            networkContainer->mutate();

            if(genMaxFitness > overallMaxFitness){
                overallMaxFitness = genMaxFitness;
                overallBestNetwork = generationBestNetwork;
            }
            LOG(INFO) << "Generation: " << generation;
            LOG(INFO) << "Generation max fitness was: " << static_cast<double>(genMaxFitness);
            genAverageFitness /= numTrialsPerGen;
            LOG(INFO) << "Generation average fitness was: " << static_cast<double>(genAverageFitness);
        }
    }
}
