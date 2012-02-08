/*
 * Experiment.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#include "Experiment.h"
#include "PredatorRandom.h"
#include "PredatorNaive.h"

#include <iostream>
#include <fstream>

#include <libconfig.h++>

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::map;
    using std::pair;
    using PredPreyHunterVisualizer::Visualizer;
    //using std::ofstream;

    Experiment::Experiment(const char* configFilePath)
    {
        numPredCaught = 0;
        numPreyCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();

        libconfig::Config cfg;

        LOG(INFO) << "Reading from config file " << configFilePath;
        cfg.readFile(configFilePath);

        maxSteps = cfg.lookup("experiment:max_steps");
        LOG(INFO) << "Max steps in experiment is "
                << static_cast<uint>(cfg.lookup("experiment:max_steps"));

        int gridWidth = cfg.lookup("experiment:grid:width");
        int gridHeight = cfg.lookup("experiment:grid:height");
        LOG(INFO) << "Grid size is " << static_cast<int>(gridWidth) << "x"
                << static_cast<int>(gridHeight);

        numHunters = cfg.lookup("agents:hunter:number");
        numPredators = cfg.lookup("agents:predator:number");
        numPrey = cfg.lookup("agents:prey:number");

        this->ptrGridWorld = new GridWorld(gridWidth, gridHeight);
        LOG(INFO) << "[CREATED] GridWorld of size " << gridWidth << ", " << gridHeight << endl;

        // initialize predator
        Position position;
        position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
        position.y = static_cast<int>(fetchRandomNumber() * gridHeight);

        int predType = cfg.lookup("agents:predator:predators:[0]:type");

        if (predType == 0) {     // Random predator
            LOG(INFO) << "Initializing random predator";
            this->ptrPredator = dynamic_cast<Predator*>(new PredatorRandom(ptrGridWorld,
                    cfg.lookup("agents:predator:predators:[0]:id"), position));
        }

        else if (predType == 1) {     // Naive predator
            LOG(INFO) << "Initializing naive predator";
            this->ptrPredator = dynamic_cast<Predator*>(new PredatorNaive(ptrGridWorld,
                    cfg.lookup("agents:predator:predators:[0]:id"), position,
                    cfg.lookup("agents:predator:predators:[0]:move_probability")));
            LOG(INFO)
                    << "Predator move probability is "
                    << static_cast<double>(cfg.lookup(
                            "agents:predator:predators:[0]:move_probability"));
        }

        LOG(INFO) << "[CREATED] Predator at " << position.x << ", " << position.y << " with id "
                << static_cast<int>(cfg.lookup("agents:predator:predators:[0]:id")) << endl;

        // initialize prey
        position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
        position.y = static_cast<int>(fetchRandomNumber() * gridHeight);
        this->ptrPrey = new Prey(ptrGridWorld, cfg.lookup("agents:prey:preys:[0]:id"), position,
                cfg.lookup("agents:prey:preys:[0]:move_probability"));
        LOG(INFO) << "Prey move probability is "
                << static_cast<double>(cfg.lookup("agents:prey:preys:[0]:move_probability"));
        LOG(INFO) << "[CREATED] Prey at " << position.x << ", " << position.y << " with id "
                << static_cast<int>(cfg.lookup("agents:prey:preys:[0]:id")) << endl;

        // initialize hunter
        position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
        position.y = static_cast<int>(fetchRandomNumber() * gridHeight);
        this->ptrHunter = new Hunter(ptrGridWorld, cfg.lookup("agents:hunter:hunters:[0]:id"),
                position, cfg.lookup("agents:hunter:hunters:[0]:move_probability"));
        LOG(INFO) << "Hunter move probability is "
                << static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:move_probability"));
        LOG(INFO) << "[CREATED] Hunter at " << position.x << ", " << position.y << " with id "
                << static_cast<int>(cfg.lookup("agents:hunter:hunters:[0]:id")) << endl;

        // Display related stuff
        displayEnabled = cfg.lookup("experiment:display");

        if (displayEnabled) {
            LOG(INFO) << "Display enabled";
            map<int, vector<double> > idColorMapping = map<int, vector<double> >();
            pair<int, vector<double> > p = pair<int, vector<double> >();

            vector<double> color = vector<double>();
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:r")));
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:g")));
            color.push_back(
                    static_cast<double>(cfg.lookup("agents:predator:predators:[0]:color:b")));
            p = pair<int, vector<double> >(
                    static_cast<int>(cfg.lookup("agents:predator:predators:[0]:id")), color);
            idColorMapping.insert(p);
            VLOG(4)
                            << "Id -> r,g,b is "
                            << static_cast<int>(cfg.lookup("agents:predator:predators:[0]:id"))
                            << " -> "
                            << static_cast<float>(cfg.lookup(
                                    "agents:predator:predators:[0]:color:r"))
                            << ", "
                            << static_cast<float>(cfg.lookup(
                                    "agents:predator:predators:[0]:color:g"))
                            << ", "
                            << static_cast<float>(cfg.lookup(
                                    "agents:predator:predators:[0]:color:b"));

            color = vector<double>();
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:r")));
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:g")));
            color.push_back(static_cast<double>(cfg.lookup("agents:prey:preys:[0]:color:b")));
            p = pair<int, vector<double> >(static_cast<int>(cfg.lookup("agents:prey:preys:[0]:id")),
                    color);
            idColorMapping.insert(p);

            color = vector<double>();
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:r")));
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:g")));
            color.push_back(static_cast<double>(cfg.lookup("agents:hunter:hunters:[0]:color:b")));
            p = pair<int, vector<double> >(
                    static_cast<int>(cfg.lookup("agents:hunter:hunters:[0]:id")), color);
            idColorMapping.insert(p);

            this->visualizer = Visualizer(idColorMapping, gridWidth, gridHeight);
        }
    }

    Experiment::~Experiment()
    {
        delete ptrPredator;
        delete ptrPrey;
        delete ptrHunter;
        delete ptrGridWorld;
    }

    void Experiment::step()
    {
        // check if prey is caught
        // return appropriate caught signal
        AgentInformation aiPredator, aiPrey, aiHunter;
        aiPredator = ptrPredator->getAgentInformation();
        aiPrey = ptrPrey->getAgentInformation();
        aiHunter = ptrHunter->getAgentInformation();

        VLOG(2) << "Predator at " << aiPredator.position.x << "," << aiPredator.position.y;
        VLOG(2) << "Prey at " << aiPrey.position.x << "," << aiPrey.position.y;
        VLOG(2) << "Hunter at " << aiHunter.position.x << "," << aiHunter.position.y;

        if ((aiPrey.position.x == aiPredator.position.x)
                && (aiPrey.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Prey caught by Predator";
            //return PREY_CAUGHT;
            this->preyCaughtIds.push_back(aiPrey.agentId);
            this->numPreyCaught++;
        }

        // check if predator is caught
        // return appropriate caught signal
        if ((aiHunter.position.x == aiPredator.position.x)
                && (aiHunter.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Predator caught by Hunter";
            //return PREDATOR_CAUGHT;
            this->predatorCaughtIds.push_back(aiPredator.agentId);
            this->numPredCaught++;
        }

        // build vector<AgentInformation>
        vector<AgentInformation> vAgentInformation;
        vAgentInformation.clear();
        vAgentInformation.push_back(aiPredator);
        vAgentInformation.push_back(aiPrey);
        vAgentInformation.push_back(aiHunter);

        // move prey
        ptrPrey->move(vAgentInformation);
        // move hunter
        ptrHunter->move(vAgentInformation);
        // move predator
        ptrPredator->move(vAgentInformation);

        // Show display
        if (displayEnabled) {
            // FIXME Need to do this in a better way
            static vector<AgentInformation> vAgentInformationPrevious = vector<AgentInformation>();
            if (vAgentInformationPrevious.size() == 0) {
                vAgentInformationPrevious = vAgentInformation;
            }
            this->visualizer.show(vAgentInformationPrevious, vAgentInformation);
            vAgentInformationPrevious = vAgentInformation;
        }
    }

    double Experiment::run()
    {
        double fitness = 0.0;
        int noSteps = 0;
        int prevNumPreyCaught = 0;
        int prevNumPredCaught = 0;

        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void) step();
            if (numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught) {
                LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                prevNumPreyCaught = numPreyCaught;
                if (numPreyCaught == numPrey) {
                    LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                    return fitness;
                }
            } else if (numPredCaught > 0 && numPredCaught > prevNumPredCaught) {
                LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
                prevNumPredCaught = numPredCaught;
                if (numPredCaught == numPredators) {
                    LOG(INFO) << "All predators caught in " << noSteps << " steps!";
                    return fitness;
                }
            }
        } while (noSteps++ < maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return fitness;
    }

/*
 double Experiment::run(string pathFile)
 {
 LOG(INFO) << "[BEGINS] Experiment::run()" << endl;
 // order is pred.x pred.y prey.x prey.y hunter.x hunter.y
 ofstream fout(pathFile.c_str());
 if (!fout.is_open()) {
 LOG(ERROR) << "Could not open file " << pathFile << " for writing!" << endl;
 throw 1; // throw something meaningful later
 }
 double fitness = 0.0;
 int noSteps = 0;
 Caught caught; // for catching caught signal
 Position positionPredator, positionPrey, positionHunter;
 while (noSteps < maxSteps) {
 LOG(INFO) << "step: " << noSteps << endl;
 caught = step();
 positionPredator = ptrPredator->getPosition();
 positionPrey = ptrPrey->getPosition();
 positionHunter = ptrHunter->getPosition();
 fout << positionPredator.x << " " << positionPredator.y << " ";
 fout << positionPrey.x << " " << positionPrey.y << " ";
 fout << positionHunter.x << " " << positionHunter.y << endl;
 switch (caught) {
 case PREY_CAUGHT:
 // if prey is caught
 // update fitness of predator
 LOG(INFO) << "PREY CAUGHT!!!!" << endl;
 fout.close(); // use smart pointer later
 return fitness;
 // break;
 case PREDATOR_CAUGHT:
 // if predator is caught
 // update fitness of predator
 LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
 fout.close(); // use smart pointer later
 return fitness;
 // break;
 case NONE_CAUGHT:
 break;
 default:
 LOG(ERROR) << "Experiment::run() I should not have reached here" << endl;
 throw 1; // throw something meaningful later
 };
 noSteps++; // NOTE: Very Important. Do not delete.
 }
 fout.close();
 LOG(INFO) << "[ENDS] Experiment::run()" << endl;
 return fitness;
 }*/
}
