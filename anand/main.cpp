/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include <iostream>

#include "GridWorld.h"
#include "Experiment.h"

using namespace PredatorPreyHunter;

int main(int argc, char **argv)
{
    google::InitGoogleLogging("");
    LOG(INFO) << "Main function";

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
    Experiment experiment(configFilePath);
    return experiment.run();
}

