/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include <iostream>

#include "common.h"
#include "GridWorld.h"
#include "Experiment.h"
#include "EspExperiment.h"

using namespace PredatorPreyHunter;
using namespace EspPredPreyHunter;

void reseed(int val)
{
    unsigned short seed[3];

    seed[0] = val;
    seed[1] = val + 1;
    seed[2] = val + 2;
    seed48(seed);
    srand48(val);
}

int main(int argc, char **argv)
{
    google::InitGoogleLogging("");
    LOG(INFO) << "Main function";

    reseed(getpid());

    int c;
    char* configFilePath;
    bool isEsp = false;

    while ((c = getopt(argc, argv, "c:e")) != -1) {
        switch(c){
            case 'c':
                configFilePath = optarg;
                break;
            case 'e':
                isEsp = true;
                break;
            default:
                abort();
                break;
        }
    }

    if(isEsp){
        EspExperiment espExperiment(configFilePath);
        espExperiment.start();
    }
    else {
        Experiment experiment(configFilePath);
        return experiment.start();
    }
}
