/*
 * main.cpp
 *
 *  Created on: Jan 27, 2012
 *      Author: anand
 */

#include <iostream>

#include <gflags/gflags.h>

#include "Domain/GridWorld.h"
#include "Experiment/EspExperiment.h"
#include "Experiment/EspExperimentSubtask.h"
#include "Esp/NetworkContainerCombiner.h"
#include "Esp/NetworkContainerSelection.h"

DEFINE_string(c, "", "Path for the config file");
DEFINE_bool(e, false, "Set to true to run experiment with monolithic network");
DEFINE_bool(s, false, "Set to true to run experiment with subtask combiner network");
DEFINE_bool(i, false, "Set to true to run experiment with subtask selection network");

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
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging("");
    LOG(INFO) << "Main function";

    reseed(getpid());

    if (FLAGS_e) {
        EspExperiment experiment(FLAGS_c);
        experiment.start();
    } else if (FLAGS_s) {
        EspExperimentSubtask<NetworkContainerCombiner> experiment(FLAGS_c);
        experiment.start();
    } else if (FLAGS_i) {
        EspExperimentSubtask<NetworkContainerSelection> experiment(FLAGS_c);
        experiment.start();
    }
    return 0;
}
