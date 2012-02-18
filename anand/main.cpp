/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include <iostream>

#include "GridWorld.h"
#include "EspExperiment.h"
#include "EspExperimentSubtask.h"
#include "EspExperimentSubtaskInverted.h"

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
    char* configFilePath = "";
    bool espEx = false;
    bool subTaskEx = false;
    bool subInvTaskEx = false;

    while ((c = getopt(argc, argv, "c:esi")) != -1) {
        switch(c){
            case 'c':
                configFilePath = optarg;
                break;
            case 'e':
                espEx = true;
                break;
            case 's':
                subTaskEx = true;
                break;
            case 'i':
                subInvTaskEx = true;
                break;
            default:
                abort();
                break;
        }
    }

    if(espEx){
        EspExperiment experiment(configFilePath);
        experiment.start();
    } else if (subTaskEx) {
        EspExperimentSubtask experiment(configFilePath);
        experiment.start();
    } else if (subInvTaskEx) {
        EspExperimentSubtaskInverted experiment(configFilePath);
        experiment.start();
    }
    return 0;
}
