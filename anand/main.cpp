/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include <iostream>

#include "GridWorld.h"
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

    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch(c){
            case 'c':
                configFilePath = optarg;
                break;
            default:
                abort();
                break;
        }
    }

    EspExperiment experiment(configFilePath);
    experiment.start();
    return 0;
}
