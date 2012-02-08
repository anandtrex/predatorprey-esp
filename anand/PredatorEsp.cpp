/*
 * PredatorEsp.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: anand
 */

#include "PredatorEsp.h"

namespace PredatorPreyHunter
{
    using std::vector;
    using std::endl;
    using std::abs;

    PredatorEsp::PredatorEsp(const GridWorld* ptrGridWorld, const uint& agentId, const Position& p,
            const double& moveProbability, const Experiment& experiment, const uint& numTeamAgents,
            const uint& numHiddenNeurons, const uint& popSize, const uint& netTp,
            const uint& numOtherAgents, const uint& numActions)
            : Predator(ptrGridWorld, agentId, p), moveProbability(moveProbability), experiment(
                    experiment)
    {
        esp = Esp(numTeamAgents, numHiddenNeurons, popSize, netTp, numOtherAgents, numActions,
                experiment);
    }

    Position PredatorEsp::move(const std::vector<AgentInformation>& vAgentInformation)
    {
        // random predator
        Action predatorAction = esp.getAction(this->agentId);
        position = ptrGridWorld->move(this->position, predatorAction);
        return position;
    }

    //////////////////////////////////////////////////////////////////////
    //
    // main
    //
    //////////////////////////////////////////////////////////////////////

    int main(int argc, char *argv[])
    {
        libconfig::Config cfg;
        google::InitGoogleLogging("");

        cfg.readFile("esp_predprey.cfg");

        // Parse command-line arguments.
        int num_hidden_neurons, popSize, seed, seedNet = 0;
        int netType;
        bool analyze = false;

        if (argc == 1) {
            LOG(INFO) << "Enter name of config file when running the command\n";
            LOG(INFO) << "For example: \"./esp-predprey config_file.txt\"\n";
            exit(1);
        }

        //*******************************Reading parameters from config file*************************

        double value;
        string line;
        ifstream config_file(argv[1]);
        vector<double> config_data;
        while (getline(config_file, line)) {
            istringstream iss2(line, istringstream::in);
            iss2 >> value;
            config_data.push_back(value);
        }

        start_generation = (int) config_data[0];     // Specify the generation number from which we start the esp (if greater than one, then should have a corresponding .bin file to load from)
        if_display = (bool) config_data[1];     // 1 - Simulation display is ON, 0 - OFF
        display_frequency = (int) config_data[2];     // Specify after how many generations you want display
        maxGenerations = (int) config_data[3];     // Maximum number of generations
        int num_teams_predator = (int) config_data[4];     // Number of predator teams
        int num_of_predators = (int) config_data[5];     // Number of predators in each team
        if_shared_fitness_predator = (bool) config_data[6];     // 1 - Fitness sharing in predator team, 0 - individual fitness in predator team
        pred_communication = (bool) config_data[7];     // 1 - Direct communication in predator teams, 0 - No direct communication in predator teams
        pred_messaging = (bool) config_data[8];     // 1 - Broadcast Messaging in predator teams, 0 - No Broadcast Messaging in predator teams
        int num_teams_prey = (int) config_data[9];     // Number of prey teams
        int num_of_prey = (int) config_data[10];     // Number of prey in each team
        prey_reappears = (bool) config_data[11];     // 1 - Each Prey reappears once it gets killed, 0 - Prey does not reappear
        if_evolve_prey = (bool) config_data[12];     // 1 - Evolve prey, 0 - Do not evolve prey (fixed behavior prey)
        prey_communication = (bool) config_data[13];     // 1 - Direct communication in prey teams, 0 - No direct communication in prey teams
        prey_messaging = (bool) config_data[14];     // 1 - Broadcast Messaging in prey teams, 0 - No Broadcast Messaging in prey teams
        int count, i;
        for (i = 15; i < 15 + num_teams_prey; i++) {
            reward_prey_team.push_back((int) config_data[i]);     // Reward for prey team 0 (If there are more prey teams, then add more lines below this to specify the prey team reward size)
        }
        count = i;
        for (; i < count + num_teams_prey; i++) {
            prey_move_probability.push_back((double) config_data[i]);     // Prey speed relative to the predator speed (If there are more prey teams, then add more lines below this to specify the prey team speed)
        }

        int num_of_hunters = (int) config_data[i];
        //LOG(INFO) << "Num hunters in main is "<<num_of_hunters<<endl;

        if (if_evolve_prey) {
            LOG(FATAL) << " Prey evolution is not supported now." << endl;
            //exit(0);
        }
        //*******************************END Reading parameters from config file*************************

        setUpCtrlC();
        parseArgs(argc, argv, &num_hidden_neurons, &popSize, &seed, &netType, &analyze, &seedNet);
        checkArgs(num_hidden_neurons, popSize);
        reseed(seed);
        //PredPrey predprey(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
        //        prey_move_probability, num_of_hunters);  // this is our environment.

        LOG(INFO) << "  ESP.C :: number of predator teams:: " << num_teams_predator
                << " number of prey teams:: " << num_teams_prey
                << " number of predators in each team:: " << num_of_predators
                << " num of prey in each team:: " << num_of_prey << " num of hunter teams:: " << 1
                << " num of hunters in each team:: " << num_of_hunters << endl;

        num_hidden_neurons = cfg.lookup("num_hidden_neurons");     //10;  //This is the number of Hidden Neurons in each Network
        popSize = cfg.lookup("pop_size");     //100;

        bool use_monolithic_network =
                (int) cfg.lookup("use_monolithic_network") == 1 ? true : false;
        double hunter_move_probability = (double) cfg.lookup("hunter_move_probability");

        // --- Construct a new ESP ---
        if (start_generation == 1) {
            if (!use_monolithic_network) {
                file_prefix = "st1_";
                // Sub-task 1
                PredPreyST1 *predprey_st1 = new PredPreyST1(num_of_predators, num_of_prey,
                        num_teams_predator, num_teams_prey, prey_move_probability);     // this is our environment.
                Esp *esp1 = new Esp(num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, 0 /*num_teams_hunters*/, 0, num_hidden_neurons /*nPops*/,
                        popSize/*nSize*/, *predprey_st1/*environment*/, netType);
                esp1->evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, 0, 0);     // evolve
                vector<vector<vector<Network*> > > hall_of_fame_pred_1 = esp1->hall_of_fame_pred;
                LOG(INFO) << "Done with subtask one" << endl << endl;

                file_prefix = "st2_";
                // Sub-task 1
                PredPreyST2 *predprey_st2 = new PredPreyST2(num_of_predators, num_teams_predator,
                        num_of_hunters, 1, hunter_move_probability);     // this is our environment.
                Esp *esp2 = new Esp(num_of_predators, 0, num_teams_predator, 0,
                        1 /*num_teams_hunters*/, num_of_hunters, num_hidden_neurons /*nPops*/,
                        popSize/*nSize*/, *predprey_st2/*environment*/, netType);
                esp2->evolve(maxGenerations, num_of_predators, 0, num_teams_predator, 0, 1,
                        num_of_hunters);     // evolve
                vector<vector<vector<Network*> > > hall_of_fame_pred_2 = esp2->hall_of_fame_pred;
                LOG(INFO) << "Done with subtask two" << endl << endl;

                file_prefix = "ot_";
                vector<vector<vector<vector<Network*> > > > hall_of_fame_pred_both = vector<
                        vector<vector<vector<Network*> > > >();
                hall_of_fame_pred_both.push_back(hall_of_fame_pred_1);
                hall_of_fame_pred_both.push_back(hall_of_fame_pred_2);

                // Overall task
                PredPrey *predprey = new PredPrey(num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, prey_move_probability, num_of_hunters,
                        hunter_move_probability);     // this is our environment.
                Esp *esp = new Esp(hall_of_fame_pred_both, num_of_predators, num_of_prey,
                        num_teams_predator, num_teams_prey, 1 /*num_teams_hunters*/, num_of_hunters,
                        num_hidden_neurons /*nPops*/, popSize/*nSize*/, *predprey/*environment*/,
                        netType);
                esp->evolve(100 /*maxGenerations*/, num_of_predators, num_of_prey,
                        num_teams_predator, num_teams_prey, 1, num_of_hunters);     // evolve
                LOG(INFO) << "Done with overall task";
            } else {
                LOG(INFO) << "Using monolithic network";
                PredPrey *predprey = new PredPrey(num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, prey_move_probability, num_of_hunters,
                        hunter_move_probability);     // this is our environment.
                Esp *esp = new Esp(num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, 1 /*num_teams_hunters*/, num_of_hunters,
                        num_hidden_neurons /*nPops*/, popSize/*nSize*/, *predprey/*environment*/,
                        netType);
                esp->evolve(100, num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                        1, num_of_hunters);     // evolve
            }

            /*
             if (analyze) {
             esp.findChampion();
             exit(1);
             }
             */

        }
        // --- Load an old ESP from file ---
        else {
            char filename[50] = "";
            std::string s;
            std::stringstream out;
            out << start_generation;
            s = out.str();
            const char * a = s.c_str();
            strcat(filename, "out_");
            strcat(filename, a);
            strcat(filename, ".bin");
            if (use_monolithic_network) {
                PredPrey predprey(num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                        prey_move_probability, num_of_hunters, hunter_move_probability);     // this is our environment.
                Esp esp(filename, num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                        1 /*num_teams_hunters*/, num_of_hunters, num_hidden_neurons, popSize,
                        predprey, netType);
                esp.evolve(maxGenerations, num_of_predators, num_of_prey, num_teams_predator,
                        num_teams_prey, 1, num_of_hunters);     // evolve
            } else {

            }

            //        if (analyze) {
            //            esp.findChampion();
            //            //exit(1);
            //        }
        }

    }
}

