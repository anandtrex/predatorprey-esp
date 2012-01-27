/*
 * Experiment.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#include "Experiment.h"
#include <iostream>
#include <fstream>

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::string;
    using std::ofstream;
    Experiment::Experiment(const int& maxSteps, const int& width, const int& height)
    {
        this->maxSteps = maxSteps;
        ptrGridWorld = new GridWorld(width, height);
        LOG(INFO) << "[CREATED] GridWorld of size " << width << ", " << height << endl;
        // initialize predator
        Position position;
        position.x = static_cast<int>(fetchRandomNumber() * width);
        position.y = static_cast<int>(fetchRandomNumber() * height);
        ptrPredator = new Predator(ptrGridWorld, 1, position);
        LOG(INFO) << "[CREATED] Predator at " << position.x << ", " << position.y << endl;
        // initialize prey
        position.x = static_cast<int>(fetchRandomNumber() * width);
        position.y = static_cast<int>(fetchRandomNumber() * height);
        ptrPrey = new Prey(ptrGridWorld, 2, position);
        LOG(INFO) << "[CREATED] Prey at " << position.x << ", " << position.y << endl;
        // initialize hunter
        position.x = static_cast<int>(fetchRandomNumber() * width);
        position.y = static_cast<int>(fetchRandomNumber() * height);
        ptrHunter = new Hunter(ptrGridWorld, 3, position);
        LOG(INFO) << "[CREATED] Hunter at " << position.x << ", " << position.y << endl;
    }
    Experiment::~Experiment()
    {
        delete ptrPredator;
        delete ptrPrey;
        delete ptrHunter;
        delete ptrGridWorld;
    }
    Experiment::Caught Experiment::step()
    {
        // check if prey is caught
        // return appropriate caught signal
        AgentInformation aiPredator, aiPrey, aiHunter;
        aiPredator = ptrPredator->getAgentInformation();
        aiPrey = ptrPrey->getAgentInformation();
        aiHunter = ptrHunter->getAgentInformation();
        if ((aiPrey.position.x == aiPredator.position.x)
                && (aiPrey.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Prey caught by Predator";
            return PREY_CAUGHT;
        }
        // check if predator is caught
        // return appropriate caught signal
        if ((aiHunter.position.x == aiPredator.position.x)
                && (aiHunter.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Predator caught by Hunter";
            return PREDATOR_CAUGHT;
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
        return NONE_CAUGHT;
    }
    double Experiment::run()
    {
        double fitness = 0.0;
        int noSteps = 0;
        Caught caught; // for catching caught signal
        while (noSteps < maxSteps) {
            LOG(INFO) << "step: " << noSteps << endl;
            caught = step();
            switch (caught) {
                case PREY_CAUGHT:
                    // if prey is caught
                    // update fitness of predator
                    LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                    return fitness;
                    // break;
                case PREDATOR_CAUGHT:
                    // if predator is caught
                    // update fitness of predator
                    LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
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
        return fitness;
    }
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
    }
}

