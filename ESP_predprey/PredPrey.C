#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <ctype.h>
#include "PredPrey.h"
#include "Network.h" 
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <iostream>

using namespace std;

extern bool SHOW;
extern int delay_pred_evolution;
extern int prey_slow;
extern int MAXPREDS;
extern bool INCREMENTAL_LEARNING;
extern int TOTAL_EVALUATIONS;  //Total number of evaluations within a generation
extern int EVALTRIALS;  // number of times to call evalNet for a given team

double pred_flee_factor = 1.0;
bool DEBUG = false;
extern bool IS_PREY;
extern int COMBINE;
extern int IS_COMBINER_NW;
FILE* fptr;

//*************************Parameters Read from the Config file**********************
extern bool if_shared_fitness_predator;  // 1 - Fitness sharing in predator team, 0 - individual fitness in predator team
extern bool pred_communication;  // 1 - Direct communication in predator teams, 0 - No direct communication in predator teams
extern bool pred_messaging;  //  1 - Broadcast Messaging in predator teams, 0 - No Broadcast Messaging in predator teams
extern bool prey_reappears;  //  1 - Each Prey reappears once it gets killed, 0 - Prey does not reappear
extern bool prey_messaging;  //  1 - Broadcast Messaging in prey teams, 0 - No Broadcast Messaging in prey teams
extern bool if_evolve_prey;  // 1 - Evolve prey, 0 - Do not evolve prey (fixed behavior prey)
extern vector<int> reward_prey_team;  // Reward for prey team 0 (If there are more prey teams, then add more lines below this to specify the prey team reward size)
extern bool prey_communication;  // 1 - Direct communication in prey teams, 0 - No direct communication in prey teams
//*************************END Parameters Read from the Config file**********************

PredPrey::PredPrey(int num_of_predators, int num_of_prey, int num_teams_predator,
        int num_teams_prey, vector<double> prey_move_probability, int num_of_hunters)
{
    this->num_of_predators = num_of_predators;
    this->num_of_prey = num_of_prey;
    this->num_teams_predator = num_teams_predator;
    this->num_teams_prey = num_teams_prey;
    this->prey_move_probability = prey_move_probability;
    this->num_of_hunters = num_of_hunters;
    cout << "Num hunters is " << this->num_of_hunters << endl;
    this->num_teams_hunters = 1;

    // class Environment variables
    // input and output sizes for EACH predator/prey agent
    inputSize = (num_of_prey * num_teams_prey + num_of_predators * (num_teams_predator - 1)) * 2;  //Number of inputs for all the networks in a single predator
    inputSize_prey = (num_of_predators * num_teams_predator + num_of_prey * (num_teams_prey - 1))
            * 2;  //Number of inputs for all the networks in a single prey

    if (pred_communication == true) {
        inputSize = inputSize + (num_of_predators - 1) * 2;  //Extra inputs for predator direct communication
    }
    if (prey_communication == true) {
        inputSize_prey = inputSize_prey + (num_of_prey - 1) * 2;  //Extra inputs for prey direct communication;
    }
    if (pred_messaging == true) {
        inputSize = inputSize + (num_of_predators - 1) * 2;  //Extra inputs for predator messaging
    }
    if (prey_messaging == true) {
        inputSize_prey = inputSize_prey + (num_of_prey - 1) * 2;  //Extra inputs for prey messaging
    }

    outputSize = 5;  //NUM_OUTPUTS will be assigned to this
    outputSize_prey = 5;  //NUM_OUTPUTS will be assigned to this

    inputSize_pred_combiner = outputSize * (inputSize / 2);
    inputSize_prey_combiner = outputSize_prey * (inputSize_prey / 2);

    outputSize_pred_combiner = 5;
    outputSize_prey_combiner = 5;

    if (pred_messaging == true) {
        outputSize_pred_combiner = outputSize_pred_combiner + 1;  //Extra combiner output for predator messaging
    }

    if (prey_messaging == true) {
        cout << " ERROR:: PREY MESSAGING SHOULD BE FALSE SINCE PREY DO NOT EVOLVE " << endl;
        outputSize_prey_combiner = outputSize_prey_combiner + 1;  //Extra combiner output for prey messaging
    }

    cout << "   inputSize per agent :  " << " Predator:: " << inputSize << " Prey:: "
            << inputSize_prey << "  Predator Combiner:: " << inputSize_pred_combiner
            << "  Prey Combiner::  " << inputSize_prey_combiner << endl;
    cout << "   outputSize per agent:  " << " Predator:: " << outputSize << " Prey:: "
            << outputSize_prey << "  Predator Combiner:: " << outputSize_pred_combiner
            << "  Prey Combiner::  " << outputSize_prey_combiner << endl;

}

//Defined as static to count the number of zebra caught
int PredPrey::previous_generation = 1;
int PredPrey::count_trials = 0;
double PredPrey::count_overall_best_zebra_caught = 0;
double PredPrey::count_generation_best_zebra_caught = 0;
double PredPrey::count_generation_zebra_caught = 0;
double PredPrey::count_zebra_caught_6 = 0;
double PredPrey::count_overall_best_mice_caught = 0;
double PredPrey::count_generation_best_mice_caught = 0;
double PredPrey::count_generation_mice_caught = 0;
double PredPrey::count_mice_caught_6 = 0;

// Fitness = (ave_initial_distance - ave_final_distance) / steps
//           if prey caught then X 10
// If (random) then prey starts at random location
//             else prey starts at (prey_start_x, prey_start_y)
// QUE What does this do?
vector<vector<vector<double> > > PredPrey::evalNet(vector<vector<Network*> >& team,
        vector<vector<Network*> >& team_prey, int generation)  //Added number of prey to arguments ******PADMINI
{

    if (SHOW) {
        //Saving predator-prey locations
        if ((fptr = fopen("pred_prey_location.txt", "at")) == NULL) {
            cout << endl << "Error - cannot open " << "pred_prey_location.txt" << endl;
            exit(1);
        }

        //fprintf (fptr,"%d ", generation);   // int generation
        //fprintf (fptr, " ");   //  character
        //fprintf (fptr,"%d ", simu_trial);   // int simu_trial

        // clear display window
        glClearColor(1.0, 1.0, 1.0, 0.0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "2. Error" << endl;
        }  //ADI

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "3. Error" << endl;
        }  //ADI

        glFlush();
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "4. Error" << endl;
        }  //ADI
    }

    if (SHOW) {  //Added another clear screen here ******PADMINI

        for (int i = 0; i < MAP_LENGTH; i++) {
            for (int j = 0; j < MAP_LENGTH; j++) {
                glBegin(GL_POINTS);
                glColor3f(1.0f, 1.0f, 1.0f);
                glVertex3f(i, j, 0);
                void glEnd(void);
            }
        }

    }
    steps = 0;
    int maxSteps;
    double ave_initial_dist = 0;
    vector<double> ave_final_dist;
    vector<double> result;
    int flip = 0;
    vector<vector<int> > energy;
    vector<vector<int> > energy_prey;

    int i, j;

    vector<vector<double> > output_prey, output;

    //prey_caught.clear(); //To clear the vector so that new values can be loaded in it ******PADMINI
    //prey_lifetime.clear();
    //average_nearest_distance.clear();

    //for(j = 0; j < num_of_prey; j++) { //Initializing the vector prey_caught to all false ******PADMINI
    //  prey_caught.push_back(false);
    //  average_nearest_distance.push_back(10000);
    //  prey_lifetime.push_back(0);
    //}

    //Randomly initialize the positions of predators and prey
    init(true, true, true, generation);

// // calculate average initial distance
// for (int i=0; i<num_of_predators; i++) {
//   for(j = 0; j < num_of_prey; j++) { //Added an inner for loop to account for multiple prey
//     ave_initial_dist += calc_dist (pred_x[i], pred_y[i], prey_x[j], prey_y[j]);
//   }
// }
// 
// ave_initial_dist /= num_of_predators;

    maxSteps = 300;

    bool temp_break_while = false;

    while (steps++ < maxSteps && temp_break_while == false) {

        if (SHOW) {  //Added another clear screen here ******PADMINI

            //Saving Predator-Prey location. Each time step is printed in a newline
            fprintf(fptr, "\n");  //  character
        }

        /************************************PREY*******************************************************/

        IS_PREY = true;
        vector<double> temp_output_prey, temp_combiner_output_prey(NUM_OUTPUT_PREY_COMBINER);
        IS_COMBINER_NW = 0;
        //Commenting the below line because prey is fixed
        //setupInput_complex_prey(num_of_predators, output_prey, team_prey);
        IS_COMBINER_NW = 1;

        int p;
        for (p = 0; p < num_teams_prey; p++) {
            for (i = 0; i < num_of_prey; i++) {
                temp_output_prey.clear();
                if (prey_communication == true) {
                }
                //***************************Commenting below else block because prey is fixed*************
                //else {
                //        for(j = 0; j < (num_teams_predator*num_of_predators)*NUM_OUTPUTS_PREY; j++){
                //                temp_output_prey.push_back(output_prey[p][(num_teams_predator*num_of_predators)*NUM_OUTPUTS_PREY*i + j ]);
                //        }
                //        if (COMBINE == 1) {
                //                team_prey[p][(num_teams_predator*num_of_predators) * num_of_prey  + i]->activate(temp_output_prey, temp_combiner_output_prey, num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
                //        }
                //        else {
                //                temp_combiner_output_prey.clear(); // Clear this to make sure there is size matching
                //                temp_combiner_output_prey = temp_output_prey;
                //        }
                //}
                //***************************END Commenting below else block because prey is fixed*************

                if (prey_caught[p][i] == true) {
                    if (SHOW) {
                        //Saving Predator-Prey location
                        int x = -1;
                        fprintf(fptr, "%d ", x);  // int generation
                        fprintf(fptr, " ");  //  character
                        fprintf(fptr, "%d ", x);  // int generation
                        fprintf(fptr, " ");  //  character
                    }

                }

                else {

                    if (drand48() < prey_move_probability[p]) {
                        performPreyAction_complex(p, i, temp_combiner_output_prey);
                    }

                    else {
                        if (SHOW) {
                            fprintf(fptr, "%d ", prey_x[p][i]);  // int generation
                            fprintf(fptr, " ");  //  character
                            fprintf(fptr, "%d ", prey_y[p][i]);  // int generation
                            fprintf(fptr, " ");  //  character
                        }
                    }
                }
            }
        }

        /************************************PREDATOR************************************************************/
        IS_PREY = false;
        vector<double> temp_output, temp_combiner_output(NUM_OUTPUT_PRED_COMBINER);
        IS_COMBINER_NW = 0;
        setupInput_complex_predator(num_of_prey, output, team);
        IS_COMBINER_NW = 1;

        messages.clear();  //messages
        vector<double> temp_messages;  //messages
        for (p = 0; p < num_teams_predator; p++) {
            temp_messages.clear();
            for (i = 0; i < num_of_predators; i++) {
                temp_output.clear();
                for (j = 0; j < inputSize_pred_combiner; j++) {
                    temp_output.push_back(output[p][inputSize_pred_combiner * i + j]);
                }
                if (COMBINE == 1) {
                    team[p][(inputSize / 2) * num_of_predators + i]->activate(temp_output,
                            temp_combiner_output, num_of_prey, num_of_predators, num_teams_prey,
                            num_teams_predator, inputSize_pred_combiner);
                } else {
                    temp_combiner_output.clear();  // Clear this to make sure there is size matching
                    temp_combiner_output = temp_output;
                }
                //if (temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] < 0.333) { //Discretizing the outputs
                //        temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] = 1;
                //}
                //else if (temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] < 0.666) {
                //        temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] = 2;
                //}
                //else if (temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] <= 1) {
                //        temp_combiner_output[NUM_OUTPUT_PRED_COMBINER-1] = 2;
                //}
                //else {
                //        cout << " ERRRROR " <<endl;
                //}
                if (pred_messaging == true) {
                    temp_messages.push_back(temp_combiner_output[NUM_OUTPUT_PRED_COMBINER - 1]);  //messages
                    temp_combiner_output.pop_back();  //Delete the last element which is the message
                }

                //cout << " 6  ";
                performPredAction_complex(p, i, temp_combiner_output);
                //cout << " 7  ";
            }
            if (pred_messaging == true) {
                messages.push_back(temp_messages);  //messages
            }
        }

        // TODO Add hunter code here
        /************************************HUNTER************************************************************/
        for (p = 0; p < num_teams_hunters; p++) {
            for (i = 0; i < num_of_hunters; i++) {

                /*if (prey_caught[p][i] == true) {
                    if (SHOW) {
                        //Saving Predator-Prey location
                        int x = -1;
                        fprintf(fptr, "%d ", x);  // int generation
                        fprintf(fptr, " ");  //  character
                        fprintf(fptr, "%d ", x);  // int generation
                        fprintf(fptr, " ");  //  character
                    }

                }

                else {*/

                //if (drand48() < prey_move_probability[p]) {
                // NOTE temp_combiner_output_prey doesn't seem to be used inside the function at all. So passing it anyway
                //cout << "Hunter";
                    performHunterAction_complex(p, i, temp_combiner_output_prey);
                /*}

                else {
                    if (SHOW) {
                        fprintf(fptr, "%d ", prey_x[p][i]);  // int generation
                        fprintf(fptr, " ");  //  character
                        fprintf(fptr, "%d ", prey_y[p][i]);  // int generation
                        fprintf(fptr, " ");  //  character
                    }
                }*/
                //}
            }
        }

        if (SHOW) {

            if (!flip) {
                sleep(1.5);
                flip = 1;
            } else if (flip == 1)
                flip = 2;
            else if (flip == 2)
                flip = 3;
            else if (flip == 3)
                flip = 4;
            else if (flip == 4)
                flip = 5;
            else if (flip == 5)
                flip = 0;

            //sleep(1);
        }  // end while

        //While break condition when all predators are killed
        for (p = 0; p < num_teams_predator; p++) {
            if (num_of_pred_kills[p] == num_of_predators) {
                temp_break_while = true;
                break;
            }
        }

        //While break condition when all predators are killed by hunters
        for (p = 0; p < num_teams_predator; p++) {
            if (num_of_pred_hit[p] == num_of_predators) {
                temp_break_while = true;
                break;
            }
        }

        //While break condition when all prey are caught
        int count_prey_caught = 0;
        for (p = 0; p < num_teams_prey; p++) {
            for (i = 0; i < num_of_prey; i++) {
                if (prey_caught[p][i] == true) {
                    count_prey_caught++;
                }
            }
        }

        if (count_prey_caught == num_teams_prey * num_of_prey) {
            break;
        }

    }  //End While

    if (SHOW) {  //Added another clear screen here ******PADMINI

        //Saving Predator-Prey location
        fprintf(fptr, "\n");  //  character
        fclose(fptr);

        for (i = 0; i < MAP_LENGTH; i++) {
            for (j = 0; j < MAP_LENGTH; j++) {
                glBegin(GL_POINTS);
                glColor3f(1.0f, 1.0f, 1.0f);
                glVertex3f(i, j, 0);
                void glEnd(void);
            }
        }

    }

    vector<double> temp_individual_fitness;
    vector<vector<double> > temp_team_fitness;
    vector<vector<vector<double> > > fitness;

    int temp_pred_killed;
    int temp_hit_wall;

    //Predator Fitness calculations
    for (int p = 0; p < num_teams_predator; p++) {

        temp_pred_killed = 0;
        temp_hit_wall = 0;
        temp_individual_fitness.clear();
        for (int i = 0; i < num_of_predators; i++) {
            if (pred_killed[p][i] == true) {
                temp_pred_killed++;
            }
            temp_hit_wall = temp_hit_wall + pred_hit_wall_times[p][i];
            if (num_teams_prey == 1) {
                temp_individual_fitness.push_back(
                        num_of_prey_caught_individually[p][i] * reward_prey_team[0]
                                + num_of_prey_caught[p] * reward_prey_team[0]);  // This is individual fitness for Competing agents
            } else if (num_teams_prey == 2) {
                temp_individual_fitness.push_back(
                        num_of_prey_caught_individually[p][i] * reward_prey_team[0]
                                + num_of_prey_caught[p] * reward_prey_team[1]);  // This is individual fitness for Competing agents
            } else {
                cout << " ERROR: Too many prey teams. Not supported now" << endl;
                exit(0);
            }
        }
        temp_team_fitness.push_back(temp_individual_fitness);
        // temp_fitness.push_back(100 * num_of_prey_caught[p] + 100 * num_of_pred_kills[p] - 100 * temp_pred_killed - temp_hit_wall); //Fitness takes into account both the number of predators you kill and predators in your team that got killed
    }

//Overall Best, Generation Best and Generation Average
    if (generation > previous_generation) {
        if (count_generation_best_zebra_caught > count_overall_best_zebra_caught) {
            count_overall_best_zebra_caught = count_generation_best_zebra_caught;
        }
        if (count_generation_best_mice_caught > count_overall_best_mice_caught) {
            count_overall_best_mice_caught = count_generation_best_mice_caught;
        }
        cout << " Generation Number :: " << previous_generation
                << " Overall Best Number of Zebra Caught :  " << count_overall_best_zebra_caught
                << endl;
        cout << " Generation Number :: " << previous_generation
                << " Generation Best Number of Zebra Caught :  "
                << count_generation_best_zebra_caught << endl;
        cout << " Generation Number :: " << previous_generation
                << " Generation Average Number of Zebra Caught :  "
                << count_generation_zebra_caught / TOTAL_EVALUATIONS << endl;
        cout << " Generation Number :: " << previous_generation
                << " Overall Best Number of Mice Caught :  " << count_overall_best_mice_caught
                << endl;
        cout << " Generation Number :: " << previous_generation
                << " Generation Best Number of Mice Caught :  " << count_generation_best_mice_caught
                << endl;
        cout << " Generation Number :: " << previous_generation
                << " Generation Average Number of Mice Caught :  "
                << count_generation_mice_caught / TOTAL_EVALUATIONS << endl;
        count_generation_best_zebra_caught = 0;
        count_generation_zebra_caught = num_of_prey_caught[0];
        count_generation_best_mice_caught = 0;
        count_generation_mice_caught = num_of_prey_caught_individually[0][0]
                + num_of_prey_caught_individually[0][1] + num_of_prey_caught_individually[0][2];
        previous_generation = generation;
    } else {
        count_generation_zebra_caught = count_generation_zebra_caught + num_of_prey_caught[0];
        count_generation_mice_caught = count_generation_mice_caught
                + num_of_prey_caught_individually[0][0] + num_of_prey_caught_individually[0][1]
                + num_of_prey_caught_individually[0][2];

    }
    if (count_trials == 0) {
        if ((count_zebra_caught_6 / EVALTRIALS) > count_generation_best_zebra_caught) {
            count_generation_best_zebra_caught = count_zebra_caught_6 / EVALTRIALS;
        }
        if ((count_mice_caught_6 / EVALTRIALS) > count_generation_best_mice_caught) {
            count_generation_best_mice_caught = count_mice_caught_6 / EVALTRIALS;
        }
        count_zebra_caught_6 = num_of_prey_caught[0];
        count_mice_caught_6 = num_of_prey_caught_individually[0][0]
                + num_of_prey_caught_individually[0][1] + num_of_prey_caught_individually[0][2];
    } else {
        count_zebra_caught_6 = count_zebra_caught_6 + num_of_prey_caught[0];
        count_mice_caught_6 = count_mice_caught_6 + num_of_prey_caught_individually[0][0]
                + num_of_prey_caught_individually[0][1] + num_of_prey_caught_individually[0][2];
    }
    count_trials++;
    if (count_trials == EVALTRIALS) {
        count_trials = 0;
    }

////Energy based Fitness function 
// for (int p = 0; p < num_teams_predator; p++) {
//
//          temp_fitness.push_back(0);
//          temp_pred_killed = 0;
//          for (int i = 0; i < num_of_predators; i++) {
//                  if (pred_killed[p][i] == true) {
//                          temp_pred_killed++;
//                  }
//                  temp_fitness[p] = temp_fitness[p] + pred_energy[p][i];
//          }
//          temp_fitness[p] = temp_fitness[p] - 1000 * temp_pred_killed; //Lose 1000 energy points if you are killed
// }

    fitness.push_back(temp_team_fitness);
    temp_team_fitness.clear();

    //Prey Fitness calculations
    for (int p = 0; p < num_teams_prey; p++) {

        temp_individual_fitness.clear();
        for (int i = 0; i < num_of_prey; i++) {
            temp_individual_fitness.push_back((double) prey_lifetime[p][i] / maxSteps * 100);  // This is individual fitness for Competing agents
        }
        temp_team_fitness.push_back(temp_individual_fitness);

    }
    fitness.push_back(temp_team_fitness);
    temp_team_fitness.clear();

    return fitness;

}  //End function

// Runs net for number of trials, returns number of times it caught prey
vector<vector<double> > PredPrey::testNet(vector<vector<Network*> >& team,
        vector<vector<Network*> >& team_prey, int trials)
{

    vector<vector<double> > result(num_teams_predator);  //This stores the number of prey caught and number of predators killed by each predator team

    for (int p = 0; p < num_teams_predator; p++) {

        result[p].push_back(0.0);
        result[p].push_back(0.0);
    }

    for (int i = 0; i < trials; i++) {
        evalNet(team, team_prey, 1);
        for (int p = 0; p < num_teams_predator; p++) {

            result[p][0] = result[p][0] + ((double) num_of_prey_caught[p]) / trials;
            result[p][1] = result[p][1] + ((double) num_of_pred_kills[p]) / trials;
        }
    }

    return result;
}

// TODO Separate out into initialization for hunters, predators and prey
void PredPrey::init(bool preyRandom, bool predsRandom, bool huntersRandom, int generation)
{

    int x, y;  //To store the initial random starting positions of the prey *******PADMINI

    vector<int> temp_x;
    vector<int> temp_y;
    vector<int> temp_energy;
    vector<bool> temp_killed;
    vector<int> temp_lifetime;
    vector<int> temp_hit_wall_times;
    vector<int> temp_num_of_prey_caught_individually;

    pred_x.clear();
    pred_y.clear();

    prey_x.clear();  //To clear the previous postions of all the prey ******PADMINI
    prey_y.clear();

    // NOTE Hunters
    hunter_x.clear();
    hunter_y.clear();

    pred_killed.clear();
    prey_caught.clear();

    num_of_pred_kills.clear();
    num_of_prey_caught.clear();

    pred_energy.clear();
    prey_energy.clear();

    prey_lifetime.clear();

    num_of_prey_caught_individually.clear();

    pred_hit_wall_times.clear();

    for (int p = 0; p < num_teams_predator; p++) {

        num_of_pred_kills.push_back(0);  //Initializing the number of opponent predators killed by each team
        num_of_pred_hit.push_back(0);  //Initializing the number of predators killed by hunters
        num_of_prey_caught.push_back(0);  //Initializing the number of prey caught by each team
        for (int i = 0; i < num_of_predators; i++) {

            temp_killed.push_back(false);
            temp_num_of_prey_caught_individually.push_back(0);

        }
        pred_killed.push_back(temp_killed);
        pred_hit.push_back(temp_killed);
        num_of_prey_caught_individually.push_back(temp_num_of_prey_caught_individually);
        temp_killed.clear();
        temp_num_of_prey_caught_individually.clear();
    }


    for (int p = 0; p < num_teams_prey; p++) {
        for (int i = 0; i < num_of_prey; i++) {

            temp_killed.push_back(false);
            temp_lifetime.push_back(0);

        }
        prey_caught.push_back(temp_killed);
        prey_lifetime.push_back(temp_lifetime);
        temp_killed.clear();
        temp_lifetime.clear();
    }

    ////Exception for removing extra zebras
    if (num_teams_prey == 2) {
        prey_caught[1][1] = true;
        prey_caught[1][2] = true;
        prey_caught[1][3] = true;
    }

    if (generation < 300) {
// prey_caught[0][0] = true; 
// prey_caught[0][1] = true; 
// prey_caught[0][2] = true; 
// prey_caught[0][3] = true; 
    }

    // arrange the prey randomly
    if (preyRandom) {
        for (int k = 0; k < num_teams_prey; k++) {
            for (int i = 0; i < num_of_prey; i++) {
                temp_x.push_back((int) (drand48() * 100));
                temp_y.push_back((int) (drand48() * 100));
                temp_energy.push_back(1000);
            }
            prey_x.push_back(temp_x);
            prey_y.push_back(temp_y);
            prey_energy.push_back(temp_energy);
            temp_x.clear();
            temp_y.clear();
            temp_energy.clear();
        }
    }

    // arrange the hunters randomly
    if (huntersRandom) {
        for (int k = 0; k < num_teams_hunters; k++) {
            for (int i = 0; i < num_of_hunters; i++) {
                temp_x.push_back((int) (drand48() * 100));
                temp_y.push_back((int) (drand48() * 100));
            }
            hunter_x.push_back(temp_x);
            hunter_y.push_back(temp_y);
            temp_x.clear();
            temp_y.clear();
        }
    }

    /*
     //Start prey from a fixed location
     else {
     //  for (int k = 0; k < num_teams_prey; k++) {
     //          for(int i = 0; i < num_of_prey; i++) {
     //                  temp_x.push_back(MAP_LENGTH/2);
     //                  temp_y.push_back(MAP_LENGTH/2);
     //                  temp_energy.push_back(1000);
     //          }
     //          prey_x.push_back(temp_x);
     //          prey_y.push_back(temp_y);
     //          prey_energy.push_back(temp_energy);
     //          temp_x.clear();
     //          temp_y.clear();
     //          temp_energy.clear();
     //  }

     //Hardcoding for 4 prey to fixed positions to surround the predators
     for (int k = 0; k < num_teams_prey; k++) {

     //Prey 0
     temp_x.push_back(MAP_LENGTH / 4);
     temp_y.push_back(MAP_LENGTH / 4);
     temp_energy.push_back(1000);

     //Prey 1
     temp_x.push_back(MAP_LENGTH / 4 * 3);
     temp_y.push_back(MAP_LENGTH / 4);
     temp_energy.push_back(1000);

     //Prey 2
     temp_x.push_back(MAP_LENGTH / 4);
     temp_y.push_back(MAP_LENGTH / 4 * 3);
     temp_energy.push_back(1000);

     //Prey 3
     temp_x.push_back(MAP_LENGTH / 4 * 3);
     temp_y.push_back(MAP_LENGTH / 4 * 3);
     temp_energy.push_back(1000);

     prey_x.push_back(temp_x);
     prey_y.push_back(temp_y);
     prey_energy.push_back(temp_energy);
     temp_x.clear();
     temp_y.clear();
     temp_energy.clear();
     }
     }*/

    // arrange the predators randomly
    if (predsRandom) {
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < num_of_predators; i++) {
                temp_x.push_back((int) (drand48() * 100));
                temp_y.push_back((int) (drand48() * 100));
                temp_energy.push_back(1000);
                temp_hit_wall_times.push_back(0);
            }
            pred_x.push_back(temp_x);
            pred_y.push_back(temp_y);
            pred_energy.push_back(temp_energy);
            pred_hit_wall_times.push_back(temp_hit_wall_times);
            temp_hit_wall_times.clear();
            temp_x.clear();
            temp_y.clear();
            temp_energy.clear();
        }
    }

    // arrange the predators in fixed positions
    else {
        int temp_pos = 1;
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < num_of_predators; i++) {
                temp_x.push_back((MAP_LENGTH * temp_pos) * 3.0 / 4);
                temp_y.push_back(MAP_LENGTH / 2);
                temp_energy.push_back(1000);
                temp_hit_wall_times.push_back(0);
            }
            pred_x.push_back(temp_x);
            pred_y.push_back(temp_y);
            pred_energy.push_back(temp_energy);
            pred_hit_wall_times.push_back(temp_hit_wall_times);
            temp_hit_wall_times.clear();
            temp_x.clear();
            temp_y.clear();
            temp_pos = 3;
            temp_energy.clear();
        }
    }
    vector<double> temp_messages;

//Messages initialization
    for (int i = 0; i < num_teams_predator; i++) {
        temp_messages.clear();
        for (int j = 0; j < num_of_predators; j++) {
            temp_messages.push_back(0.0);
        }
        messages.push_back(temp_messages);
    }

}

void PredPrey::performPreyAction_complex(int prey_team, int prey,
        const vector<double>& output_single_prey)
{  //Added the argument int prey to check which prey is being processed ******PADMINI
    //double nearestDist = calc_dist(pred_x[0], pred_y[0], prey_x[prey], prey_y[prey]); //To hold the distances of the prey from its nearest predator
    double nearestDist = MAP_LENGTH * 2 + 1;  //Big number

    int nearestPred = 0;  //To hold the nearest predator to the prey
    int nearestPred_team = 0;

    double dist;

    int x_dist, y_dist, temp;

    int i;  //Counter for loops ******PADMINI

    double preyAction;
    int act = 0;

    int old_prey_x = prey_x[prey_team][prey];
    int old_prey_y = prey_y[prey_team][prey];

    if (prey_caught[prey_team][prey]) {
        preyAction = 4;
    } else {
        //if (delay_pred_evolution == 1) {
        IS_PREY = true;
        ///********Commenting out for now since prey is fixed**********************/
        // if (COMBINE == 1) {
        //         preyAction = getMaxPos(output_single_prey);
        // }
        // else {
        //         preyAction = getMaxPos_complex (output_single_prey);
        // }
        ///********END Commenting out for now since prey is fixed**********************/
        //}
        //else { //Added !prey_caught[i] to calculate stuff for prey only if it's not been caught yet ******PADMINI

        // find nearest predator

        /********Commenting out for now since prey is evolving**********************/
        for (int p = 0; p < num_teams_predator; p++) {
            for (i = 0; i < num_of_predators; i++) {
                dist = calc_dist(pred_x[p][i], pred_y[p][i], prey_x[prey_team][prey],
                        prey_y[prey_team][prey]);
                if (dist < nearestDist) {
                    nearestDist = dist;
                    nearestPred = i;
                    nearestPred_team = p;
                }
            }
        }

        // calculate predator offset with wraparound
        x_dist = pred_x[nearestPred_team][nearestPred] - prey_x[prey_team][prey];

        if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
            temp = x_dist;
            x_dist = MAP_LENGTH - abs(x_dist);
            if (temp > 0)
                x_dist = 0 - x_dist;
        }

        y_dist = pred_y[nearestPred_team][nearestPred] - prey_y[prey_team][prey];

        if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
            temp = y_dist;
            y_dist = MAP_HEIGHT - abs(y_dist);
            if (temp > 0)
                y_dist = 0 - y_dist;
        }

        if (y_dist < 0 && (abs(y_dist) >= abs(x_dist))) {
            //prey_y[prey]++;
            preyAction = 0;
        }

        else if (x_dist < 0 && (abs(x_dist) >= abs(y_dist))) {
            //prey_x[prey]++;
            preyAction = 1;
        }

        else if (y_dist > 0 && (abs(y_dist) >= abs(x_dist))) {
            //prey_y[prey]--;
            preyAction = 2;
        }

        else if (x_dist > 0 && (abs(x_dist) >= abs(y_dist))) {
            //prey_x[prey]--;
            preyAction = 3;
        }

        else {
            preyAction = 4;
        }
        //  }
    }
    /********END Commenting out for now since prey is evolving**********************/

    //preyAction=4; // Making the prey fixed for now (It is acting as a food)
    if (preyAction == 0) {
        prey_y[prey_team][prey]++;
    }

    else if (preyAction == 1) {
        prey_x[prey_team][prey]++;
    }

    else if (preyAction == 2) {
        prey_y[prey_team][prey]--;
    }

    else if (preyAction == 3) {
        prey_x[prey_team][prey]--;
    }

    else if (preyAction == 4) {
    }

    if (prey_x[prey_team][prey] > MAP_LENGTH)
        prey_x[prey_team][prey] -= MAP_LENGTH;
    if (prey_y[prey_team][prey] > MAP_HEIGHT)
        prey_y[prey_team][prey] -= MAP_HEIGHT;
    if (prey_x[prey_team][prey] < 0)
        prey_x[prey_team][prey] += MAP_LENGTH;
    if (prey_y[prey_team][prey] < 0)
        prey_y[prey_team][prey] += MAP_HEIGHT;

    if (SHOW) {

        //Saving Predator-Prey location
        if (prey_caught[prey_team][prey] == false) {
            fprintf(fptr, "%d ", prey_x[prey_team][prey]);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", prey_y[prey_team][prey]);  // int generation
            fprintf(fptr, " ");  //  character
        } else {
            int x = -1;
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
        }

        // erase old prey point
        glBegin(GL_POINTS);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "15. Error" << endl;
        }  //ADI

        glColor3f(1.0f, 1.0f, 1.0f);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "14. Error" << endl;
        }  //ADI

        glVertex3f(old_prey_x, old_prey_y, 0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "16. Error" << endl;
        }  //ADI

        void glEnd(void);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "17. Error" << endl;
        }  //ADI

        if (prey_caught[prey_team][prey] == false) {  //Added this if-else condition so that caught prey are drawn in white ******PADMINI

            // draw new prey point
            glBegin(GL_POINTS);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "19. Error" << endl;
            }  //ADI

            if (prey_team == 0) {
                glColor3f(1.0f, 0.0f, 1.0f);  //trying to change the colors of different prey ******PADMINI
            }
            if (prey_team == 1) {
                glColor3f(0.0f, 0.9f, 1.0f);  //trying to change the colors of different prey ******PADMINI
            }

            glVertex3f(prey_x[prey_team][prey], prey_y[prey_team][prey], 0);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "20. Error" << endl;
            }  //ADI

            glEnd();
            //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
        }

        else {
            // draw new prey point
            glBegin(GL_POINTS);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "19. Error" << endl;
            }  //ADI

            glColor3f(1.0f, 1.0f, 1.0f);  //white color for the prey that've already been caught, i.e. they vanish ******PADMINI
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "18. Error" << endl;
            }  //ADI

            glVertex3f(prey_x[prey_team][prey], prey_y[prey_team][prey], 0);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "20. Error" << endl;
            }  //ADI

            glEnd();
            //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
        }

        glFlush();
        //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "22. Error" <<endl; } //ADI
    }
}

void PredPrey::reset_prey_position(int prey_team, int prey)
{
    prey_x[prey_team][prey] = ((int) (drand48() * 100));
    prey_y[prey_team][prey] = ((int) (drand48() * 100));

}

//void PredPrey::setupInput_complex_prey(int num_of_predators, vector <vector<double> >&output_prey, vector < vector<Network*> >& team_prey)
//{
//        IS_PREY = true; 
//	//Changed these to vectors to store the x- and y-distances of the predators from the multiple prey ******PADMINI
//	int x_dist;
//	int y_dist;
//	vector <double> temp_output_prey (NUM_OUTPUTS_PREY);
//	vector <double> temp_input_prey (NUM_INPUTS_PREY);
//
//	int temp = 0;
//	
//	int i, j; //Counters for loops ******PADMINI
//	
//	//Changed this whole block to account for multiple prey ******PADMINI
//        output_prey.clear();
//        for(i = 0; i < num_of_prey; i++) { //Loop through the different prey
//                for(j = 0; j < num_of_predators; j++) { //EVOLVE_PREY
//                    temp_input_prey.clear();
//                    //temp_output_prey.clear();
//		    x_dist = (prey_x[i] - pred_x[j]); //The distance in x-direction between predator and prey
//		    
//		    if ((abs(x_dist)) > (MAP_LENGTH/2)) {
//		    	temp = x_dist;
//		    	x_dist = MAP_LENGTH  - abs(x_dist);
//
//		    	if (temp > 0)
//		    		x_dist = 0 - x_dist;
//		    }
//
//		    y_dist = (prey_y[i] - pred_y[j]); //The distance in y-direction between predator and prey
//
//		    if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
//		    	temp = y_dist;
//		    	y_dist = MAP_HEIGHT  - abs(y_dist);
//
//		    	if (temp > 0)
//		    		y_dist = 0 - y_dist; 
//		    }
//                    temp_input_prey.push_back(x_dist);
//                    temp_input_prey.push_back(y_dist);
//                    team_prey[i*num_of_predators + j]->activate(temp_input_prey, temp_output_prey, num_of_prey, num_of_predators, num_teams_prey, num_teams_predator);
//                    for(int k = 0; k < NUM_OUTPUTS_PREY; k++) {                   
//                            output_prey.push_back(temp_output_prey[k]);
//                    }
//                }
//
//                //Aditya : March 03 - Communication - Adding the following for Prey Communication
//                if (prey_communication == true) {
//                        for(j = num_of_predators; (j < num_of_prey+num_of_predators) && (i!=j-num_of_predators); j++) { //EVOLVE_PREY
//                              temp_input_prey.clear();
//		              x_dist = (prey_x[i] - prey_x[j-num_of_predators]); //The distance in x-direction between predator and prey
//		              
//		              if ((abs(x_dist)) > (MAP_LENGTH/2)) {
//		              	temp = x_dist;
//		              	x_dist = MAP_LENGTH  - abs(x_dist);
//
//		              	if (temp > 0)
//		              		x_dist = 0 - x_dist;
//		              }
//		              
//                              y_dist = (prey_y[i] - prey_y[j-num_of_predators]); //The distance in x-direction between predator and prey
//
//		              if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
//		              	temp = y_dist;
//		              	y_dist = MAP_HEIGHT  - abs(y_dist);
//
//		              	if (temp > 0)
//		              		y_dist = 0 - y_dist; 
//		              }
//                              temp_input_prey.push_back(x_dist);
//                              temp_input_prey.push_back(y_dist);
//                              //team_prey[i*(num_of_prey+num_of_predators-1) + j]->activate(temp_input_prey, temp_output_prey, num_of_prey, num_of_predators, num_teams_prey, num_teams_predator);
//                              for(int k = 0; k < NUM_OUTPUTS_PREY; k++) {                   
//                                      output_prey.push_back(temp_output_prey[k]);
//                              }
//                        }
//                }
//	}

//}
/**********************************************************************************************/
void PredPrey::setupInput_complex_prey(int num_of_predators, vector<vector<double> >& output_prey,
        vector<vector<Network*> >& team_prey)
{
    //Changed these to vectors to store the x- and y-distances of the predators from the multiple prey ******PADMINI
    int x_dist;
    int y_dist;
    int energy_diff;
    vector<double> temp_output_prey(NUM_OUTPUTS_PREY);
    vector<double> temp_input_prey(NUM_INPUTS_PREY);
    vector<double> output_per_team_prey;

    int temp = 0;

    int i, j, p, q;  //Counters for loops ******PADMINI

    output_prey.clear();
    output_per_team_prey.clear();
    int count = 0;

    IS_PREY = true;
    for (p = 0; p < num_teams_prey; p++) {
        count = 0;
        for (i = 0; i < num_of_prey && !prey_caught[p][i]; i++) {
            for (q = 0; q < num_teams_predator; q++) {
                for (j = 0; j < num_of_predators; j++) {
                    temp_input_prey.clear();
                    //temp_output.clear();
                    if (!pred_killed[q][j]) {
                        x_dist = abs(prey_x[p][i] - pred_x[q][j]);  //The distance in x-direction between predator and prey
                        y_dist = abs(prey_y[p][i] - pred_y[q][j]);  //The distance in y-direction between predator and prey
                    } else {
                        x_dist = 0;
                        y_dist = 0;
                    }

                    //Commenting this out for sensing the wall
                    if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
                        temp = x_dist;
                        x_dist = MAP_LENGTH - abs(x_dist);

                        if (temp > 0)
                            x_dist = 0 - x_dist;
                    }

                    if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
                        temp = y_dist;
                        y_dist = MAP_HEIGHT - abs(y_dist);

                        if (temp > 0)
                            y_dist = 0 - y_dist;
                    }
                    temp_input_prey.push_back(x_dist);
                    temp_input_prey.push_back(y_dist);
                    //temp_input.push_back(0);  //Energy difference with Prey is zero
                    //temp_input.push_back(0);  //Dummy input
                    team_prey[p][count]->activate(temp_input_prey, temp_output_prey,
                            num_of_predators, num_of_prey, num_teams_predator, num_teams_prey,
                            inputSize_prey_combiner);
                    count++;
                    for (int k = 0; k < NUM_OUTPUTS_PREY; k++) {
                        output_per_team_prey.push_back(temp_output_prey[k]);
                    }
                }
            }
            //******************************Commenting this so that Prey teams do not track each other*******************************
            // for (q=0; q< num_teams_prey; q++) {
            //         for(j = 0; j < num_of_prey && q!=p; j++) {
            //                 temp_input_prey.clear();
            //                 //temp_output.clear();
            //                 if (!prey_caught[q][j]) {
            //                         x_dist = (prey_x[p][i] - prey_x[q][j]); //The distance in x-direction between predator and prey
            //                         y_dist = (prey_y[p][i] - prey_y[q][j]); //The distance in y-direction between predator and prey
            //                         energy_diff = (prey_energy[p][i] - prey_energy[q][j]); //Calculating energy difference
            //                 }
            //                 else {
            //                         x_dist = 0;
            //                         y_dist = 0;
            //                         energy_diff = 0;
            //                 }

            //                 //Commenting this out for sensing the wall
            //                 if ((abs(x_dist)) > (MAP_LENGTH/2)) {
            //                 	temp = x_dist;
            //                 	x_dist = MAP_LENGTH  - abs(x_dist);

            //                 	if (temp > 0)
            //                 		x_dist = 0 - x_dist;
            //                 }

            //                 if ((abs(y_dist)) > (MAP_HEIGHT/2)) {
            //                 	temp = y_dist;
            //                 	y_dist = MAP_HEIGHT  - abs(y_dist);

            //                 	if (temp > 0)
            //                 		y_dist = 0 - y_dist;
            //                 }
            //                     temp_input_prey.push_back(x_dist);
            //                     temp_input_prey.push_back(y_dist);
            //                     //temp_input.push_back(energy_diff);  //Energy difference with Prey is zero
            //                     //temp_input.push_back(0);  //Dummy input
            //                     team_prey[p][count]->activate(temp_input_prey, temp_output_prey, num_of_predators, num_of_prey, num_teams_predator, num_teams_prey);
            //                     count++;
            //                     for(int k = 0; k < NUM_OUTPUTS_PREY; k++) {
            //                             output_per_team_prey.push_back(temp_output_prey[k]);
            //                     }
            //                 }
            // }
            //******************************END Commenting this so that Prey teams do not track each other*******************************

            ////Adding extra neural network for sensing walls
            //temp_input.clear();
            //temp_input.push_back(abs(pred_x[p][i])); //X coordinate of own position
            //temp_input.push_back(abs(pred_y[p][i])); //Y coordinate of own position
            //temp_input.push_back(abs(pred_x[p][i] - MAP_LENGTH)); //
            //temp_input.push_back(abs(pred_y[p][i] - MAP_LENGTH)); //
            //team[p][count]->activate(temp_input, temp_output, num_of_prey, num_of_predators, num_teams_prey, num_teams_predator);
            //for(int k = 0; k < NUM_OUTPUTS; k++) {
            //        output_per_team.push_back(temp_output[k]);
            //}

        }
        output_prey.push_back(output_per_team_prey);
        output_per_team_prey.clear();
    }
}

/**********************************************************************************************/
void PredPrey::setupInput_complex_predator(int num_of_prey, vector<vector<double> >& output,
        vector<vector<Network*> >& team)
{
    //Changed these to vectors to store the x- and y-distances of the predators from the multiple prey ******PADMINI
    int x_dist;
    int y_dist;
    int energy_diff;
    vector<double> temp_output(NUM_OUTPUTS);
    vector<double> temp_input(NUM_INPUTS);
    vector<double> output_per_team;

    int temp = 0;

    int i, j, p, q;  //Counters for loops ******PADMINI

    output.clear();
    output_per_team.clear();
    int count = 0;

    IS_PREY = false;
    for (p = 0; p < num_teams_predator; p++) {
        count = 0;
        for (i = 0; i < num_of_predators && !pred_killed[p][i]; i++) {
            for (q = 0; q < num_teams_prey; q++) {
                for (j = 0; j < num_of_prey; j++) {
                    temp_input.clear();
                    //temp_output.clear();
                    if (!prey_caught[q][j]) {
                        x_dist = abs(pred_x[p][i] - prey_x[q][j]);  //The distance in x-direction between predator and prey
                        y_dist = abs(pred_y[p][i] - prey_y[q][j]);  //The distance in y-direction between predator and prey
                    } else {
                        x_dist = 0;
                        y_dist = 0;
                    }

                    //Commenting this out for sensing the wall
                    if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
                        temp = x_dist;
                        x_dist = MAP_LENGTH - abs(x_dist);

                        if (temp > 0)
                            x_dist = 0 - x_dist;
                    }

                    if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
                        temp = y_dist;
                        y_dist = MAP_HEIGHT - abs(y_dist);

                        if (temp > 0)
                            y_dist = 0 - y_dist;
                    }
                    temp_input.push_back(x_dist);
                    temp_input.push_back(y_dist);
                    //temp_input.push_back(0);  //Energy difference with Prey is zero
                    //temp_input.push_back(0);  //Dummy input
                    team[p][count]->activate(temp_input, temp_output, num_of_prey, num_of_predators,
                            num_teams_prey, num_teams_predator, inputSize_pred_combiner);
                    count++;
                    for (int k = 0; k < NUM_OUTPUTS; k++) {
                        output_per_team.push_back(temp_output[k]);
                    }
                }
            }

            //Detecting other predators (own teammates in case of direct communication, other teams in case of multiple teams)
            for (q = 0; q < num_teams_predator; q++) {
                for (j = 0; j < num_of_predators; j++) {
                    if (pred_communication == true && q == p && i == j) {  //This for Predator communication case
                        continue;
                    } else if (pred_communication == false && q == p) {
                        continue;
                    }
                    temp_input.clear();
                    //temp_output.clear();
                    if (!pred_killed[q][j]) {
                        x_dist = (pred_x[p][i] - pred_x[q][j]);  //The distance in x-direction between predator and prey
                        y_dist = (pred_y[p][i] - pred_y[q][j]);  //The distance in y-direction between predator and prey
                        energy_diff = (pred_energy[p][i] - pred_energy[q][j]);  //Calculating energy difference
                    } else {
                        x_dist = 0;
                        y_dist = 0;
                        energy_diff = 0;
                    }

                    //Commenting this out for sensing the wall
                    if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
                        temp = x_dist;
                        x_dist = MAP_LENGTH - abs(x_dist);

                        if (temp > 0)
                            x_dist = 0 - x_dist;
                    }

                    if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
                        temp = y_dist;
                        y_dist = MAP_HEIGHT - abs(y_dist);

                        if (temp > 0)
                            y_dist = 0 - y_dist;
                    }
                    temp_input.push_back(x_dist);
                    temp_input.push_back(y_dist);
                    team[p][count]->activate(temp_input, temp_output, num_of_prey, num_of_predators,
                            num_teams_prey, num_teams_predator, inputSize_pred_combiner);
                    count++;
                    for (int k = 0; k < NUM_OUTPUTS; k++) {
                        output_per_team.push_back(temp_output[k]);
                    }
                }
            }

            //Detecting messages from predator teammates
            for (q = 0; q < num_teams_predator; q++) {
                for (j = 0; j < num_of_predators; j++) {
                    if (pred_messaging == true && q == p && i == j) {  //Don't receive one's own message
                        continue;
                    } else if (pred_messaging == true && q != p) {  //Don't receive messages from the members of other teams
                        continue;
                    } else if (pred_messaging == false) {
                        break;
                    }
                    temp_input.clear();

                    if (!pred_killed[q][j]) {
                        temp_input.push_back(messages[q][j]);  //messages : sensing the message of team-mate
                    } else {
                        temp_input.push_back(0.0);  //messages : sensing the message of team-mate
                    }
                    temp_input.push_back(0);  //messages : sensing the message of team-mate
                    team[p][count]->activate(temp_input, temp_output, num_of_prey, num_of_predators,
                            num_teams_prey, num_teams_predator, inputSize_pred_combiner);
                    count++;
                    for (int k = 0; k < NUM_OUTPUTS; k++) {
                        output_per_team.push_back(temp_output[k]);
                    }
                }
            }
            ////Adding extra neural network for sensing walls
            //temp_input.clear();
            //temp_input.push_back(abs(pred_x[p][i])); //X coordinate of own position
            //temp_input.push_back(abs(pred_y[p][i])); //Y coordinate of own position
            //temp_input.push_back(abs(pred_x[p][i] - MAP_LENGTH)); //
            //temp_input.push_back(abs(pred_y[p][i] - MAP_LENGTH)); //
            //team[p][count]->activate(temp_input, temp_output, num_of_prey, num_of_predators, num_teams_prey, num_teams_predator);
            //for(int k = 0; k < NUM_OUTPUTS; k++) {
            //        output_per_team.push_back(temp_output[k]);
            //}

        }
        output.push_back(output_per_team);
        output_per_team.clear();
    }
}

// TODO Add check to see if predator hit by hunter
void PredPrey::performPredAction_complex(int pred_team, int pred,
        const vector<double>& output_single_predator)
{
    int predAction = 0;
    //bool finished = false; Commented this variable because it is never used ******PADMINI
    int old_pred_x = pred_x[pred_team][pred];
    int old_pred_y = pred_y[pred_team][pred];

    int x_dist, y_dist, temp;
    int i, j;  //Counter for loops ******PADMINI

    IS_PREY = false;
    if (COMBINE == 0) {
        predAction = getMaxPos_complex(output_single_predator);
    } else {
        predAction = getMaxPos(output_single_predator);
    }

    if (pred_killed[pred_team][pred] == true || pred_hit[pred_team][pred == true]) {
        predAction = 4;  // Remain Idle once you are killed

    }

    pred_flee_factor = 1.0;

    if (drand48() <= pred_flee_factor) {

        if (predAction == 0) {
            //if(pred_y[pred_team][pred] != MAP_LENGTH) {
            pred_y[pred_team][pred]++;
            //}
            //else {
            //      pred_hit_wall_times[pred_team][pred] = pred_hit_wall_times[pred_team][pred] +5;   //Hitting wall leads to loss of 5 fitness points
            //}
            //pred_energy[pred_team][pred]--;
        }

        else if (predAction == 1) {
            //if(pred_x[pred_team][pred] != MAP_LENGTH) {
            pred_x[pred_team][pred]++;
            //}
            //else {
            //      pred_hit_wall_times[pred_team][pred] = pred_hit_wall_times[pred_team][pred] +5;     //Hitting wall leads to loss of 5 fitness points
            //}
            //pred_energy[pred_team][pred]--;
        }

        else if (predAction == 2) {
            //if(pred_y[pred_team][pred] != 0) {
            pred_y[pred_team][pred]--;
            //}
            //else {
            //      pred_hit_wall_times[pred_team][pred] = pred_hit_wall_times[pred_team][pred] +5;     //Hitting wall leads to loss of 5 fitness points
            //}
            //pred_energy[pred_team][pred]--;
        }

        else if (predAction == 3) {
            //if(pred_x[pred_team][pred] != 0) {
            pred_x[pred_team][pred]--;
            //}
            //else {
            //      pred_hit_wall_times[pred_team][pred] = pred_hit_wall_times[pred_team][pred] +5;     //Hitting wall leads to loss of 5 fitness points
            //}
            //pred_energy[pred_team][pred]--;
        }

        else if (predAction == 4 && pred_killed[pred_team][pred] == false) {  // Energy based fitness:: Predators lose energy even during idle (but don't once they are killed)
            //pred_hit_wall_times[pred_team][pred]++; // Idle state leads to loss of 1 fitness points
            //pred_energy[pred_team][pred]--;
        }
    }

    if (pred_x[pred_team][pred] > MAP_LENGTH)
        pred_x[pred_team][pred] -= MAP_LENGTH;
    if (pred_y[pred_team][pred] > MAP_HEIGHT)
        pred_y[pred_team][pred] -= MAP_HEIGHT;
    if (pred_x[pred_team][pred] < 0)
        pred_x[pred_team][pred] += MAP_LENGTH;
    if (pred_y[pred_team][pred] < 0)
        pred_y[pred_team][pred] += MAP_HEIGHT;

    double color1 = 0;
    double color2 = 0;
    double color3 = 0;

    if (SHOW) {
        //Saving Predator-Prey location
        if (pred_killed[pred_team][pred] == false) {
            fprintf(fptr, "%d ", pred_x[pred_team][pred]);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", pred_y[pred_team][pred]);  // int generation
            fprintf(fptr, " ");  //  character
        } else {
            int x = -1;
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
        }

        // erase old pred point
        glBegin(GL_POINTS);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "6. Error" << endl;
        }  //ADI

        glColor3f(1.0f, 1.0f, 1.0f);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "5. Error" << endl;
        }  //ADI

        glVertex3f(old_pred_x, old_pred_y, 0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "7. Error" << endl;
        }  //ADI

        void glEnd(void);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "8. Error" << endl;
        }  //ADI

        // draw new pred point
        if (pred_killed[pred_team][pred] == false) {
            if (pred_team == 0 && pred == 0) {
                color1 = 1;
            } else if (pred_team == 0 && pred == 1) {
                color2 = 1;
            } else if (pred_team == 0 && pred == 2) {
                color3 = 1;
            } else if (pred_team == 1 && pred == 0) {
                color2 = 1;
            } else if (pred_team == 1 && pred == 1) {
                color3 = 0;
            } else if (pred_team == 1 && pred == 2) {
                color3 = 0;
            }

            glBegin(GL_POINTS);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "10. Error" << endl;
            }  //ADI

            glColor3f(color1, color2, color3);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "9. Error" << endl;
            }  //ADI

            glVertex3f(pred_x[pred_team][pred], pred_y[pred_team][pred], 0);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "11. Error" << endl;
            }  //ADI

            void glEnd(void);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "12. Error" << endl;
            }  //ADI

            glFlush();
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "13. Error" << endl;
            }  //ADI
        } else {
            glBegin(GL_POINTS);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "10. Error" << endl;
            }  //ADI

            glColor3f(1.0f, 1.0f, 1.0f);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "9. Error" << endl;
            }  //ADI

            glVertex3f(pred_x[pred_team][pred], pred_y[pred_team][pred], 0);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "11. Error" << endl;
            }  //ADI

            void glEnd(void);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "12. Error" << endl;
            }  //ADI

            glFlush();
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "13. Error" << endl;
            }  //ADI
        }
    }

    if (DEBUG) {
        cout << "Predator " << pred << " at \t(" << pred_x[pred_team][pred] << ", "
                << pred_y[pred_team][pred] << "), \tOutput: ";

        for (int i = 0; i < (int) output_single_predator.size(); i++)
            cout << output_single_predator[i] << " ";

        cout << endl << "Action: " << predAction << endl;
    }

    // check if pred caught prey
    /* if (pred_x[pred]==prey_x && pred_y[pred]==prey_y)
     caught=true; */

    //Changed the above to say that predators have only finished catching the prey
    //if they've caught all the prey. Also, the same predator needn't have caught all the prey. ******PADMINI
    for (int p = 0; p < num_teams_prey; p++) {
        for (i = 0; i < num_of_prey; i++) {
            if (prey_caught[p][i] == false && pred_killed[pred_team][pred] == false && pred_hit[pred_team][pred] == false
                    && pred_x[pred_team][pred] == prey_x[p][i]
                    && pred_y[pred_team][pred] == prey_y[p][i]) {
                if (prey_reappears == true) {
                    reset_prey_position(p, i);
                } else {
                    prey_caught[p][i] = true;
                    prey_lifetime[p][i] = steps;
                    cout << "Bingo! Prey caught!!" << endl;
                }

                pred_energy[pred_team][pred] = pred_energy[pred_team][pred] + 100;
                if (p == 1 || (if_shared_fitness_predator && (num_teams_prey == 1))) {  //Adding override for fitness sharing experiment
                    num_of_prey_caught[pred_team]++;  //Adding exception for zebra. The whole team gets fitness on catching a zebra
                } else {
                    num_of_prey_caught_individually[pred_team][pred]++;
                }
            }
        }
    }

    /* Doesn't seem to do anything. So commented out -- Anand
    for (int p = 0; p < num_teams_predator; p++) {
        for (i = 0; i < num_of_predators && p != pred_team; i++) {
            if (pred_killed[p][i] == false && pred_killed[pred_team][pred] == false
                    && pred_x[pred_team][pred] == pred_x[p][i]
                    && pred_y[pred_team][pred] == pred_y[p][i]) {
                if (pred_energy[pred_team][pred] > pred_energy[p][i]) {
                    //pred_killed[p][i] = true;
                    //num_of_pred_kills[pred_team]++;
                } else {
                    //pred_killed[pred_team][pred] = true;
                    //num_of_pred_kills[p]++;
                }
            }
        }
    }*/

    // Checks if the predator was killed by the hunter
    for (int p = 0; p < num_teams_hunters; p++) {
        for (i = 0; i < num_of_hunters; i++) {
            if (pred_hit[pred_team][pred] == false
                    && pred_x[pred_team][pred] == hunter_x[p][i]
                    && pred_y[pred_team][pred] == hunter_y[p][i]) {
                pred_hit[pred_team][pred] = true;
                num_of_pred_hit[p]++;
                cout << "Bingo! Hunter hit!!" << endl;
            }
        }
    }

    //int sum = 0; //To count the number of prey caught ******PADMINI
    //for(int j = 0; j < num_of_prey; j++) {
    //        if(prey_caught[j])
    //                sum++;
    //}
    //
    //if(sum == num_of_prey) { //If all the prey are caught, the boolean 'caught' becomes true ******PADMINI
    //	caught = true;
    //	//num_of_preyCaught = 0;
    //}

    ////Adding temporary variable to keep track of the closest distance the predator team was to each of the prey
    //for (i=0; i<num_of_prey; i++) {
    //        average_nearest_distance[i]+= calc_dist (pred_x[pred], pred_y[pred], prey_x[i], prey_y[i]);
    //}
}

void PredPrey::performHunterAction_complex(int hunter_team, int hunter,
        const vector<double>& output_single_prey)
{  //Added the argument int prey to check which prey is being processed ******PADMINI
    //double nearestDist = calc_dist(pred_x[0], pred_y[0], prey_x[prey], prey_y[prey]); //To hold the distances of the prey from its nearest predator
    double nearestDist = MAP_LENGTH * 2 + 1;  //Big number

    int nearestPred = 0;  //To hold the nearest predator to the prey
    int nearestPred_team = 0;

    double dist;

    int x_dist, y_dist, temp;

    int i;  //Counter for loops ******PADMINI

    int hunterAction;
    int act = 0;

    int old_hunter_x = hunter_x[hunter_team][hunter];
    int old_hunter_y = hunter_y[hunter_team][hunter];

    if (prey_caught[hunter_team][hunter]) {
        hunterAction = 4;
    } else {
        IS_PREY = true;
        for (int p = 0; p < num_teams_predator; p++) {
            for (i = 0; i < num_of_predators; i++) {
                dist = calc_dist(pred_x[p][i], pred_y[p][i], hunter_x[hunter_team][hunter],
                        hunter_y[hunter_team][hunter]);
                if (dist < nearestDist) {
                    nearestDist = dist;
                    nearestPred = i;
                    nearestPred_team = p;
                }
            }
        }

        // calculate predator offset with wraparound
        x_dist = pred_x[nearestPred_team][nearestPred] - hunter_x[hunter_team][hunter];

        if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
            temp = x_dist;
            x_dist = MAP_LENGTH - abs(x_dist);
            if (temp > 0)
                x_dist = 0 - x_dist;
        }

        y_dist = pred_y[nearestPred_team][nearestPred] - hunter_y[hunter_team][hunter];

        if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
            temp = y_dist;
            y_dist = MAP_HEIGHT - abs(y_dist);
            if (temp > 0)
                y_dist = 0 - y_dist;
        }

        hunterAction = (int) (drand48() * 4.0);
        cout << "Hunter action is "<< hunterAction << endl;
        /* NOTE: Can use this to make hunter move towards predators (with modifications)
        if (y_dist < 0 && (abs(y_dist) >= abs(x_dist))) {
            //prey_y[prey]++;
            hunterAction = 0;
        }

        else if (x_dist < 0 && (abs(x_dist) >= abs(y_dist))) {
            //prey_x[prey]++;
            hunterAction = 1;
        }

        else if (y_dist > 0 && (abs(y_dist) >= abs(x_dist))) {
            //prey_y[prey]--;
            hunterAction = 2;
        }

        else if (x_dist > 0 && (abs(x_dist) >= abs(y_dist))) {
            //prey_x[prey]--;
            hunterAction = 3;
        }

        else {
            hunterAction = 4;
        }*/
        //	}
    }

    //preyAction=4; // Making the prey fixed for now (It is acting as a food)
    if (hunterAction == 0) {
        hunter_y[hunter_team][hunter]++;
    }

    else if (hunterAction == 1) {
        hunter_x[hunter_team][hunter]++;
    }

    else if (hunterAction == 2) {
        hunter_y[hunter_team][hunter]--;
    }

    else if (hunterAction == 3) {
        hunter_x[hunter_team][hunter]--;
    }

    else if (hunterAction == 4) {
    }

    if (hunter_x[hunter_team][hunter] > MAP_LENGTH)
        hunter_x[hunter_team][hunter] -= MAP_LENGTH;
    if (hunter_y[hunter_team][hunter] > MAP_HEIGHT)
        hunter_y[hunter_team][hunter] -= MAP_HEIGHT;
    if (hunter_x[hunter_team][hunter] < 0)
        hunter_x[hunter_team][hunter] += MAP_LENGTH;
    if (hunter_y[hunter_team][hunter] < 0)
        hunter_y[hunter_team][hunter] += MAP_HEIGHT;

    // Code for showing simulation in UI
    if (SHOW) {
        /*
        if (prey_caught[hunter_team][hunter] == false) {
            fprintf(fptr, "%d ", prey_x[hunter_team][hunter]);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", prey_y[hunter_team][hunter]);  // int generation
            fprintf(fptr, " ");  //  character
        } else {
            int x = -1;
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
            fprintf(fptr, "%d ", x);  // int generation
            fprintf(fptr, " ");  //  character
        }*/
        // Saving hunter location
        fprintf(fptr, "%d ", hunter_x[hunter_team][hunter]);  // int generation
        fprintf(fptr, " ");  //  character
        fprintf(fptr, "%d ", hunter_y[hunter_team][hunter]);  // int generation
        fprintf(fptr, " ");  //  character

        // erase old prey point
        glBegin(GL_POINTS);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "15. Error" << endl;
        }  //ADI

        glColor3f(1.0f, 1.0f, 1.0f);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "14. Error" << endl;
        }  //ADI

        glVertex3f(old_hunter_x, old_hunter_y, 0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "16. Error" << endl;
        }  //ADI

        void glEnd(void);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "17. Error" << endl;
        }  //ADI

        //if (prey_caught[hunter_team][hunter] == false) {  //Added this if-else condition so that caught prey are drawn in white ******PADMINI

        // draw new prey point
        glBegin(GL_POINTS);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "19. Error" << endl;
        }  //ADI

        if (hunter_team == 0) {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        /* N/A
         if (hunter_team == 1) {
         glColor3f(0.0f, 0.9f, 1.0f);  //trying to change the colors of different prey ******PADMINI
         }*/

        glVertex3f(hunter_x[hunter_team][hunter], hunter_y[hunter_team][hunter], 0);
        {
            GLint err = glGetError();
            if (err != GL_NO_ERROR)
                cout << "20. Error" << endl;
        }  //ADI

        glEnd();
        //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
        /*}

        else {
            // draw new prey point
            glBegin(GL_POINTS);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "19. Error" << endl;
            }  //ADI

            glColor3f(1.0f, 1.0f, 1.0f);  //white color for the prey that've already been caught, i.e. they vanish ******PADMINI
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "18. Error" << endl;
            }  //ADI

            glVertex3f(prey_x[hunter_team][hunter], prey_y[hunter_team][hunter], 0);
            {
                GLint err = glGetError();
                if (err != GL_NO_ERROR)
                    cout << "20. Error" << endl;
            }  //ADI

            glEnd();
            //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
        }*/

        glFlush();
        //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "22. Error" <<endl; } //ADI
    }
}

// Make task harder
void PredPrey::nextTask()
{

    switch (task_number) {
    case 0:
        prey_move_factor = 0.45;
        prey_flee_factor = 0.45;
        task_number++;
        break;
    case 1:
        prey_move_factor = 0.63;
        prey_flee_factor = 0.63;
        task_number++;
        break;
    case 2:
        prey_move_factor = 0.77;
        prey_flee_factor = 0.77;
        task_number++;
        break;
    case 3:
        prey_move_factor = 0.89;
        prey_flee_factor = 0.89;
        task_number++;
        break;
    case 4:
        prey_move_factor = 1;
        prey_flee_factor = 1;
        task_number++;
        break;
    case 5:
        prey_move_factor = 1;
        prey_flee_factor = 1;
        break;
    }

    cout << endl << "Next task. prey_move_factor: " << prey_move_factor << " , prey_flee_factor: "
            << prey_flee_factor << endl << endl;

}

// Make task simpler
void PredPrey::simplifyTask()
{

    switch (task_number) {
    case 0:
        prey_move_factor = 0;
        prey_flee_factor = 0;
        break;
    case 1:
        prey_move_factor = 0;
        prey_flee_factor = 0;
        task_number--;
        break;
    case 2:
        prey_move_factor = 0.45;
        prey_flee_factor = 0.45;
        task_number--;
        break;
    case 3:
        prey_move_factor = 0.63;
        prey_flee_factor = 0.63;
        task_number--;
        break;
    case 4:
        prey_move_factor = 0.77;
        prey_flee_factor = 0.77;
        task_number--;
        break;
    case 5:
        prey_move_factor = 0.89;
        prey_flee_factor = 0.89;
        task_number--;
        break;

    }

    cout << endl << "Simplify task. prey_move_factor: " << prey_move_factor
            << " , prey_flee_factor: " << prey_flee_factor << endl << endl;

}

void PredPrey::getTaskState(double* move, double* flee, int* num)
{
    *move = prey_move_factor;
    *flee = prey_flee_factor;
    *num = task_number;
}

void PredPrey::setTaskState(double* move, double* flee, int* num)
{
    prey_move_factor = *move;
    prey_flee_factor = *flee;
    task_number = *num;
}

bool PredPrey::hardestTask()
{
    return (task_number == 5);
}

// Calculates distance (actual distance, using pythag. thm)
// Takes shortest distance, ie. will wrap around edges
double PredPrey::calc_dist(int x1, int y1, int x2, int y2)
{

    int x_dist = 0;
    int y_dist = 0;

    x_dist = abs(x1 - x2);
    if (x_dist > (MAP_LENGTH / 2))
        x_dist = MAP_LENGTH - x_dist;

    y_dist = abs(y1 - y2);
    if (y_dist > (MAP_HEIGHT / 2))
        y_dist = MAP_HEIGHT - y_dist;

    return (x_dist + y_dist);

}

// returns the position in the vector with the highest number
int PredPrey::getMaxPos(const vector<double>& vec)
{

    double max = vec[0];
    int result = 0;
    int vecsize = vec.size();

    for (int i = 0; i < vecsize; i++) {
        if (vec[i] > max) {
            max = vec[i];
            result = i;
        }
    }

    return result;

}

// returns the position in the vector with the highest number
int PredPrey::getMaxPos_complex(const vector<double>& vec)
{

    vector<double> temp_sum_outputs;

    if (IS_PREY) {
        if (prey_communication == true) {  //Aditya : March 03 - Communication
            for (int i = 0; i < NUM_OUTPUTS_PREY; i++) {
                temp_sum_outputs.push_back(0.0);
                for (int j = 0; j < num_of_predators + num_of_prey - 1; j++) {
                    temp_sum_outputs[i] = temp_sum_outputs[i] + vec[NUM_OUTPUTS_PREY * j + i];
                }
            }
        } else {
            for (int i = 0; i < NUM_OUTPUTS_PREY; i++) {
                temp_sum_outputs.push_back(0.0);
                for (int j = 0; j < num_of_predators; j++) {
                    temp_sum_outputs[i] = temp_sum_outputs[i] + vec[NUM_OUTPUTS_PREY * j + i];
                }
            }

        }
    } else {
        if (pred_communication == true) {  //Aditya : March 03 - Communication
            for (int i = 0; i < NUM_OUTPUTS; i++) {
                temp_sum_outputs.push_back(0.0);
                for (int j = 0; j < num_of_prey + num_of_predators - 1; j++) {
                    //if (!prey_caught[j]) {
                    temp_sum_outputs[i] = temp_sum_outputs[i] + vec[NUM_OUTPUTS * j + i];
                    //}
                }
            }
        } else {
            for (int i = 0; i < NUM_OUTPUTS; i++) {
                temp_sum_outputs.push_back(0.0);
                for (int j = 0; j < num_of_prey; j++) {
                    //if (!prey_caught[j]) {
                    temp_sum_outputs[i] = temp_sum_outputs[i] + vec[NUM_OUTPUTS * j + i];
                    //}
                }
            }
        }
    }

    double max = temp_sum_outputs[0];
    int result = 0;
    int vecsize = temp_sum_outputs.size();

    for (int i = 0; i < vecsize; i++) {
        if (temp_sum_outputs[i] > max) {
            max = temp_sum_outputs[i];
            result = i;
        }
    }

    return result;

}

