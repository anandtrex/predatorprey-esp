/*
 * Experiment.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#include "Experiment.h"
#include <iostream>
#include <fstream>
#include <libconfig.h++>

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::string;
    using std::ofstream;
    using std::string;

    Experiment::Experiment(const char* configFilePath)
    {
        this->numPredCaught = 0;
        this->numPreyCaught = 0;
        this->predatorCaughtIds = vector<uint>();
        this->preyCaughtIds = vector<uint>();

    	libconfig::Config cfg;

    	LOG(INFO) << "Reading from config file " << configFilePath;
    	cfg.readFile(configFilePath);

    	this->numHunters = cfg.lookup("agents:hunter:number");
    	this->numPredators = cfg.lookup("agents:predator:number");
    	this->numPrey = cfg.lookup("agents:prey:number");

    	this->maxSteps = cfg.lookup("experiment:max_steps");
    	LOG(INFO) << "Max steps in experiemnt is " << static_cast<uint>(cfg.lookup("experiment:max_steps"));

    	int gridWidth = cfg.lookup("experiment:grid:width");
    	int gridHeight = cfg.lookup("experiment:grid:height");
    	LOG(INFO) << "Grid size is " << static_cast<int>(gridWidth) << "x" << static_cast<int>(gridHeight);

		this->ptrGridWorld = new GridWorld(gridWidth, gridHeight);
		LOG(INFO) << "[CREATED] GridWorld of size " << gridWidth << ", " << gridHeight << endl;
		// initialize predator
		Position position;
		position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
		position.y = static_cast<int>(fetchRandomNumber() * gridHeight);
		this->ptrPredator = new Predator(ptrGridWorld, 1, position);
		LOG(INFO) << "[CREATED] Predator at " << position.x << ", " << position.y << endl;
		// initialize prey
		position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
		position.y = static_cast<int>(fetchRandomNumber() * gridHeight);
		this->ptrPrey = new Prey(ptrGridWorld, 2, position, cfg.lookup("agents:prey:move_probability"));
		LOG(INFO) << "Prey move probability is " << static_cast<double>(cfg.lookup("agents:prey:move_probability"));
		LOG(INFO) << "[CREATED] Prey at " << position.x << ", " << position.y << endl;
		// initialize hunter
		position.x = static_cast<int>(fetchRandomNumber() * gridWidth);
		position.y = static_cast<int>(fetchRandomNumber() * gridHeight);
		this->ptrHunter = new Hunter(ptrGridWorld, 3, position, cfg.lookup("agents:hunter:move_probability"));
		LOG(INFO) << "Hunter move probability is " << static_cast<double>(cfg.lookup("agents:hunter:move_probability"));
		LOG(INFO) << "[CREATED] Hunter at " << position.x << ", " << position.y << endl;
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
        VLOG(1) << "Predator at " << aiPredator.position.x << "," << aiPredator.position.y;
        VLOG(1) << "Prey at " << aiPrey.position.x << "," << aiPrey.position.y;
        VLOG(1) << "Hunter at " << aiHunter.position.x << "," << aiHunter.position.y;

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
    }

    double Experiment::run()
    {
        double fitness = 0.0;
        int noSteps = 0;
        //Caught caught; // for catching caught signal
        int prevNumPreyCaught = 0;
        int prevNumPredCaught = 0;

        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void)step();
            if(numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught){
            	LOG(INFO) << "PREY CAUGHT!!!!" << endl;
            	prevNumPreyCaught = numPreyCaught;
            	if(numPreyCaught == numPrey){
            	    LOG(INFO) << "All prey caught!";
            	    return fitness;
            	}
            } else if (numPredCaught > 0 && numPredCaught > prevNumPredCaught) {
            	LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
            	prevNumPredCaught = numPredCaught;
            	if(numPredCaught == numPredators){
            	    LOG(INFO) << "All predators caught!";
                    return fitness;
                }
            }
        } while (noSteps++ < maxSteps);

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

