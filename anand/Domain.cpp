/*
 * Domain.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anands
 */

#include <iostream>
#include <fstream>

#include <libconfig.h++>

#include "Domain.h"
#include "PredatorEsp.h"
#include "NetworkContainer.h"

namespace PredatorPreyHunter
{
    using std::endl;
    using std::vector;
    using std::map;
    using std::pair;
    using PredPreyHunterVisualizer::Visualizer;

    Domain::Domain()
    {
    }

    Domain::Domain(const uint& maxSteps, const uint& width, const uint& height,
            const uint& numPredators, const uint& numPrey, const uint& numHunters,
            const double& preyMoveProb, const double& hunterMoveProb) :
            maxSteps(maxSteps), numPredators(numPredators), numPrey(numPrey), numHunters(
                    numHunters), preyMoveProb(preyMoveProb), hunterMoveProb(hunterMoveProb)
    {
        this->ptrGridWorld = new GridWorld(width, height);
        LOG(INFO) << "[CREATED] GridWorld of size " << width << ", " << height << endl;
    }

    Domain::~Domain()
    {
        /*
         delete ptrPredator;
         delete ptrPrey;
         delete ptrHunter;
         delete ptrGridWorld;
         */
    }

    void Domain::init(NetworkContainer* espNetwork)
    {
        Position randomPosition = ptrGridWorld->getRandomPosition();
        this->ptrPredator = dynamic_cast<Predator*>(new PredatorEsp(ptrGridWorld, 1, randomPosition,
                espNetwork));

        // initialize prey
        randomPosition = ptrGridWorld->getRandomPosition();
        this->ptrPrey = new Prey(ptrGridWorld, 2, randomPosition, preyMoveProb);
        LOG(INFO) << "Prey move probability is " << preyMoveProb;
        LOG(INFO) << "[CREATED] Prey at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 2 << endl;

        randomPosition = ptrGridWorld->getRandomPosition();
        this->ptrHunter = new Hunter(ptrGridWorld, 3, randomPosition, hunterMoveProb);
        LOG(INFO) << "Hunter move probability is " << hunterMoveProb;
        LOG(INFO) << "[CREATED] Hunter at " << randomPosition.x << ", " << randomPosition.y
                << " with id " << 3 << endl;
    }

    void Domain::enableDisplay(const vector<double>& predatorColour,
            const vector<double>& preyColour, const vector<double>& hunterColour)
    {
        this->displayEnabled = true;
        LOG(INFO) << "Display enabled";
        map<int, vector<double> > idColorMapping = map<int, vector<double> >();
        pair<int, vector<double> > p = pair<int, vector<double> >();

        p = pair<int, vector<double> >(1, predatorColour);
        idColorMapping.insert(p);

        p = pair<int, vector<double> >(2, preyColour);
        idColorMapping.insert(p);

        p = pair<int, vector<double> >(3, hunterColour);
        idColorMapping.insert(p);

        visualizer = Visualizer(idColorMapping, ptrGridWorld->width, ptrGridWorld->height);
    }

    void Domain::disableDisplay()
    {
        this->displayEnabled = false;
    }

    void Domain::step()
    {
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
            preyCaughtIds.push_back(aiPrey.agentId);
            numPreyCaught++;
        }

        if ((aiHunter.position.x == aiPredator.position.x)
                && (aiHunter.position.y == aiPredator.position.y)) {
            LOG(INFO) << "Predator caught by Hunter";
            predatorCaughtIds.push_back(aiPredator.agentId);
            numPredCaught++;
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

    double Domain::run()
    {
        uint noSteps = 0;
        uint prevNumPreyCaught = 0;
        uint prevNumPredCaught = 0;
        numPredCaught = 0;
        numPreyCaught = 0;
        predatorCaughtIds = vector<uint>();
        preyCaughtIds = vector<uint>();

        do {
            VLOG(1) << "step: " << noSteps << endl;
            (void) step();
            if (numPreyCaught > 0 && numPreyCaught > prevNumPreyCaught) {
                LOG(INFO) << "PREY CAUGHT!!!!" << endl;
                VLOG(5)
                        << "Number of prey caught is " << numPreyCaught
                                << " and the total number of prey is " << numPrey;
                prevNumPreyCaught = numPreyCaught;
                if (numPreyCaught == numPrey) {
                    LOG(INFO) << "All prey caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            } else if (numPredCaught > 0 && numPredCaught > prevNumPredCaught) {
                LOG(INFO) << "PREDATOR CAUGHT!!!!" << endl;
                prevNumPredCaught = numPredCaught;
                VLOG(5)
                        << "Number of prey caught is " << numPredCaught
                                << " and the total number of prey is " << numPredators;
                if (numPredCaught == numPredators) {
                    LOG(INFO) << "All predators caught in " << noSteps << " steps!";
                    return calculateFitness(noSteps);
                }
            }
        } while (noSteps++ < maxSteps);

        if (noSteps - 1 == maxSteps) {
            LOG(INFO) << maxSteps << " passed without prey/predator being caught";
        }

        return  calculateFitness(noSteps);
    }

    double Domain::calculateFitness(const uint& stepCurrent ) {
        double fitness = 0.0;
        if ( numPreyCaught != 0 ) { // Yay
          fitness = static_cast<double>(10) * ( maxSteps - stepCurrent ) * numPreyCaught;
          return fitness;
        }
        else if ( numPredCaught != 0 ) { // :-(
          VLOG(2) << "maxSteps - stepCurrent: " << maxSteps - stepCurrent << endl;
          fitness = static_cast<double>(-1) * 10 * ( maxSteps - stepCurrent ) * numPredCaught;
          LOG(INFO) << "FiTness: " << fitness << endl;
          return fitness;
        }
        else {
          // calculate distance from hunter and prey
          Position positionPredator = ptrPredator->getPosition();
          Position positionPrey = ptrPrey->getPosition();
          Position positionHunter = ptrHunter->getPosition();
          uint distancePrey, distanceHunter;
          distancePrey = ptrGridWorld->distance( positionPrey, positionPredator );
          distanceHunter = ptrGridWorld->distance( positionHunter, positionPredator );
          // reward for being close to prey and far away from hunter
          distancePrey = ptrGridWorld->getWidth() + ptrGridWorld->getHeight() - distancePrey;
          // take into account the size of the grid for rewarding later
          fitness = static_cast<double>(distancePrey) + distanceHunter; // if by any chance it reaches here although it won't
          return fitness;
        }
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
