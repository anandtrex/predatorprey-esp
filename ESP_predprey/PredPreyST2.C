/*
 * Hunters-only environment
 */

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <ctype.h>
#include "PredPreyST2.h"
#include "Network.h" 
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <iostream>
#include <glog/logging.h>

using namespace std;

extern bool SHOW;

extern int EVALTRIALS;  // number of times to call evalNet for a given team
extern bool IS_PREY;

//extern double pred_flee_factor = 1.0;
//extern bool DEBUG = false;
extern int COMBINE;
extern int IS_COMBINER_NW;
extern FILE* fptr;

//*************************Parameters Read from the Config file**********************
extern bool pred_communication;  // 1 - Direct communication in predator teams, 0 - No direct communication in predator teams
extern bool pred_messaging;  //  1 - Broadcast Messaging in predator teams, 0 - No Broadcast Messaging in predator teams
//*************************END Parameters Read from the Config file**********************

PredPreyST2::PredPreyST2(int num_of_predators, int num_teams_predator, int num_of_hunters,
        int num_teams_hunters, double hunter_move_probability)
{
    this->num_of_predators = num_of_predators;
    this->num_teams_predator = num_teams_predator;
    this->num_of_hunters = num_of_hunters;
    this->num_teams_hunters = num_teams_hunters;
    this->hunter_move_probability = hunter_move_probability;

    inputSize_prey = 0;
    inputSize_prey_combiner = 0;
    outputSize_prey_combiner = 0;
    outputSize_prey = 0;

    // class Environment variables
    // input and output sizes for EACH predator/prey agent
    inputSize = (num_of_hunters * num_teams_hunters + num_of_predators * (num_teams_predator - 1))
            * 2;  //Number of inputs for all the networks in a single predator

    if (pred_communication == true) {
        inputSize = inputSize + (num_of_predators - 1) * 2;  //Extra inputs for predator direct communication
    }
    if (pred_messaging == true) {
        inputSize = inputSize + (num_of_predators - 1) * 2;  //Extra inputs for predator messaging
    }

    outputSize = 5;  //NUM_OUTPUTS will be assigned to this

    inputSize_pred_combiner = outputSize * (inputSize / 2);

    outputSize_pred_combiner = 5;

    if (pred_messaging == true) {
        outputSize_pred_combiner = outputSize_pred_combiner + 1;  //Extra combiner output for predator messaging
    }

    LOG(INFO) << "   inputSize per agent :  " << " Predator:: " << inputSize
            << "  Predator Combiner:: " << inputSize_pred_combiner << endl;
    LOG(INFO) << "   outputSize per agent:  " << " Predator:: " << outputSize
            << "  Predator Combiner:: " << outputSize_pred_combiner << endl;

}

int PredPreyST2::previous_generation = 1;
int PredPreyST2::count_trials = 0;

/**
 * To implement the Environment interface
 * @param team
 * @param team_prey
 * @param generation
 * @return
 */
//vector<vector<vector<double> > > PredPreyST2::evalNet(vector<vector<Network*> >& team,
//        /*vector<vector<Network*> >& team_prey,*/ int generation)
//{
//    return evalNet(team, generation);
//}
/**
 * To implement the Environment interface
 * @param team
 * @param team_prey
 * @param trials
 * @return
 */
//vector<vector<double> > PredPreyST2::testNet(vector<vector<Network*> >& team,
//        vector<vector<Network*> >& team_prey, int trials)
//{
//    return testNet(team, trials);
//}
// Fitness = (ave_initial_distance - ave_final_distance) / steps
//           if prey caught then X 10
// If (random) then prey starts at random location
//             else prey starts at (prey_start_x, prey_start_y)
// QUE What does this do?
vector<vector<vector<double> > > PredPreyST2::evalNet(vector<vector<Network*> >& team,
        int generation)  //Added number of prey to arguments ******PADMINI
{
    IS_PREY = false;

    //cout << "Starting evalNet" << endl;

    if (SHOW) {
        //Saving predator-prey locations
        if ((fptr = fopen("pred_prey_location.txt", "at")) == NULL) {
            cout << endl << "Error - cannot open " << "pred_prey_location.txt" << endl;
            exit(1);
        }

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
    vector<double> ave_final_dist;
    vector<double> result;
    int flip = 0;
    vector<vector<int> > energy;

    int i, j;

    vector<vector<double> > output;

    //Randomly initialize the positions of predators and prey
    init(true, true, generation);

    maxSteps = 300;

    bool temp_break_while = false;

    while (steps++ < maxSteps && temp_break_while == false) {

        if (SHOW) {  //Added another clear screen here ******PADMINI

            //Saving Predator-Prey location. Each time step is printed in a newline
            fprintf(fptr, "\n");  //  character
        }

        /************************************PREDATOR************************************************************/

        vector<double> temp_output, temp_combiner_output(NUM_OUTPUT_PRED_COMBINER);
        IS_COMBINER_NW = 0;
        setupInput_complex_predator(output, team);
        IS_COMBINER_NW = 1;

        messages.clear();  //messages
        vector<double> temp_messages;  //messages
        for (int p = 0; p < num_teams_predator; p++) {
            temp_messages.clear();
            for (i = 0; i < num_of_predators; i++) {
                //cout << "COMBINE" << COMBINE <<  endl;
                temp_output.clear();
                for (j = 0; j < inputSize_pred_combiner; j++) {
                    temp_output.push_back(output[p][inputSize_pred_combiner * i + j]);
                }
                if (COMBINE == 1) {
                    team[p][(inputSize / 2) * num_of_predators + i]->activate(temp_output,
                            temp_combiner_output, /*num_of_prey, num_of_predators, num_teams_prey,
                             num_teams_predator,*/inputSize_pred_combiner);
                } else {
                    temp_combiner_output.clear();  // Clear this to make sure there is size matching
                    temp_combiner_output = temp_output;
                }
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

        /************************************HUNTER************************************************************/
        for (int p = 0; p < num_teams_hunters; p++) {
            for (i = 0; i < num_of_hunters; i++) {
                performHunterAction_complex(p, i);
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
        }  // end while

        //While break condition when all predators are killed
        for (int p = 0; p < num_teams_predator; p++) {
            if (num_of_pred_kills[p] == num_of_predators) {
                cout << "All predators killed" << endl;
                temp_break_while = true;
                break;
            }
        }

        //While break condition when all predators are killed by hunters
        for (int p = 0; p < num_teams_predator; p++) {
            if (num_of_pred_hit[p] == num_of_predators) {
                VLOG(1) << "All predators killed by hunters" << endl;
                temp_break_while = true;
                break;
            }
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
            // NOTE Fitness value decreased if predator is hit by hunter
            if (pred_hit[p][i] == true) {
                double individual_fitness_value = -10 * (maxSteps - steps);  //-300.0;
                //VLOG(1) << "Fitness decreased" << endl;
                temp_individual_fitness.push_back(individual_fitness_value);  // This is individual fitness for Competing agents
                //LOG(INFO) << "temp_individual_fitness[0] is " << temp_individual_fitness[0];
            } else {
                temp_individual_fitness.push_back(0.0);  // This is individual fitness for Competing agents
                //LOG(INFO) << "temp_individual_fitness[0] is " << temp_individual_fitness[0];
            }
        }
        temp_team_fitness.push_back(temp_individual_fitness);
        // temp_fitness.push_back(100 * num_of_prey_caught[p] + 100 * num_of_pred_kills[p] - 100 * temp_pred_killed - temp_hit_wall); //Fitness takes into account both the number of predators you kill and predators in your team that got killed
    }

//Overall Best, Generation Best and Generation Average
    if (generation > previous_generation) {
        // Assuming there are only one team of predators
//        LOG(INFO) << "No. hunter hits :: " << num_of_pred_hit[0] << endl;
    }
    count_trials++;
    if (count_trials == EVALTRIALS) {
        count_trials = 0;
    }

    fitness.push_back(temp_team_fitness);
    //LOG(INFO) << "temp_team_fitness[0][0] is " << temp_team_fitness[0][0];
    temp_team_fitness.clear();

    //cout << "Ending evalNet with prey caught ::  " << num_of_prey_caught[0] << " and predators hit :: " << num_of_pred_hit[0] << endl;
    //LOG(INFO) << "Size of fitness vector is " << fitness.size();

    //LOG(INFO) << "fitness[0][0][0] is " << fitness[0][0][0];
    return fitness;

}  //End function

// Runs net for number of trials, returns number of times it caught prey
vector<vector<double> > PredPreyST2::testNet(vector<vector<Network*> >& team, int trials)
{

    vector<vector<double> > result(num_teams_predator);  //This stores the number of prey caught and number of predators killed by each predator team

    for (int p = 0; p < num_teams_predator; p++) {
        result[p].push_back(0.0);
        result[p].push_back(0.0);
        result[p].push_back(0.0);
    }

    for (int i = 0; i < trials; i++) {
        evalNet(team, 1);
        for (int p = 0; p < num_teams_predator; p++) {
            result[p][1] = result[p][1] + ((double) num_of_pred_kills[p]) / trials;
            result[p][2] = result[p][2] + ((double) num_of_pred_hit[p]) / trials;
        }
    }

    return result;
}

// TODO Separate out into initialization for hunters, predators and prey
void PredPreyST2::init(bool predsRandom, bool huntersRandom, int generation)
{

    vector<int> temp_x;
    vector<int> temp_y;
    vector<int> temp_energy;
    vector<bool> temp_killed;
    vector<int> temp_lifetime;
    vector<int> temp_hit_wall_times;

    pred_x.clear();
    pred_y.clear();

    // NOTE Hunters
    hunter_x.clear();
    hunter_y.clear();

    pred_killed.clear();
    pred_hit.clear();

    num_of_pred_kills.clear();
    num_of_pred_hit.clear();

    pred_energy.clear();

    pred_hit_wall_times.clear();

    for (int p = 0; p < num_teams_predator; p++) {
        num_of_pred_kills.push_back(0);  //Initializing the number of opponent predators killed by each team
        num_of_pred_hit.push_back(0);  //Initializing the number of predators killed by hunters
        for (int i = 0; i < num_of_predators; i++) {
            temp_killed.push_back(false);

        }
        pred_killed.push_back(temp_killed);
        pred_hit.push_back(temp_killed);
        temp_killed.clear();
    }

    // arrange the hunters randomly
    if (huntersRandom) {
        for (int k = 0; k < num_teams_hunters; k++) {
            for (int i = 0; i < num_of_hunters; i++) {
                temp_x.push_back((int) (drand48() * MAP_LENGTH));
                temp_y.push_back((int) (drand48() * MAP_HEIGHT));
            }
            hunter_x.push_back(temp_x);
            hunter_y.push_back(temp_y);
            temp_x.clear();
            temp_y.clear();
        }
    }

    // arrange the predators randomly
    if (predsRandom) {
        for (int k = 0; k < num_teams_predator; k++) {
            for (int i = 0; i < num_of_predators; i++) {
                temp_x.push_back((int) (drand48() * MAP_LENGTH));
                temp_y.push_back((int) (drand48() * MAP_HEIGHT));
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

/**********************************************************************************************/
void PredPreyST2::setupInput_complex_predator(vector<vector<double> >& output,
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

    for (p = 0; p < num_teams_predator; p++) {
        count = 0;
        for (i = 0; i < num_of_predators && !pred_killed[p][i]; i++) {
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
                    team[p][count]->activate(temp_input, temp_output, /*num_of_prey, num_of_predators,
                     num_teams_prey, num_teams_predator,*/inputSize_pred_combiner);
                    count++;
                    output_per_team.insert(output_per_team.end(), temp_output.begin(),
                            temp_output.end());
                }
            }

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
                        x_dist = (pred_x[p][i] - pred_x[q][j]);  //The distance in x-direction between predator and and other predator
                        y_dist = (pred_y[p][i] - pred_y[q][j]);  //The distance in y-direction between predator and and other predator
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
                    team[p][count]->activate(temp_input, temp_output, /*num_of_prey, num_of_predators,
                     num_teams_prey, num_teams_predator,*/inputSize_pred_combiner);
                    count++;
                    output_per_team.insert(output_per_team.end(), temp_output.begin(),
                            temp_output.end());
                }
            }

            //Detecting hunters!
            for (q = 0; q < num_teams_hunters; q++) {
                for (j = 0; j < num_of_hunters; j++) {
                    temp_input.clear();
                    //temp_output.clear();
                    x_dist = abs(pred_x[p][i] - hunter_x[q][j]);  //The distance in x-direction between predator and hunter
                    y_dist = abs(pred_y[p][i] - hunter_y[q][j]);  //The distance in y-direction between predator and hunter

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
                    team[p][count]->activate(temp_input, temp_output, /*num_of_prey, num_of_predators,
                     num_teams_prey, num_teams_predator,*/inputSize_pred_combiner);
                    count++;
                    output_per_team.insert(output_per_team.end(), temp_output.begin(),
                            temp_output.end());
                }
            }

        }
        output.push_back(output_per_team);
        output_per_team.clear();
    }
}

void PredPreyST2::performPredAction_complex(int pred_team, int pred,
        const vector<double>& output_single_predator)
{
    //cout << "Performing pred action" << endl;
    int predAction = 0;
    //bool finished = false; Commented this variable because it is never used ******PADMINI
    int old_pred_x = pred_x[pred_team][pred];
    int old_pred_y = pred_y[pred_team][pred];

    int i;  //Counter for loops ******PADMINI

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
            pred_y[pred_team][pred]++;
        }

        else if (predAction == 1) {
            pred_x[pred_team][pred]++;
        }

        else if (predAction == 2) {
            pred_y[pred_team][pred]--;
        }

        else if (predAction == 3) {
            pred_x[pred_team][pred]--;
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

    if (SHOW) {
        showPred(pred_team, pred, old_pred_x, old_pred_y);
    }  // end show

    VLOG(1)
            << "Predator " << pred << " at \t(" << pred_x[pred_team][pred] << ", "
                    << pred_y[pred_team][pred] << "), \tOutput: ";

    for (int i = 0; i < (int) output_single_predator.size(); i++)
        VLOG(1) << output_single_predator[i] << " ";

    VLOG(1) << endl << "Action: " << predAction << endl;

    // Checks if the predator was killed by the hunter
    for (int p = 0; p < num_teams_hunters; p++) {
        for (i = 0; i < num_of_hunters; i++) {
            if (pred_hit[pred_team][pred] == false && pred_x[pred_team][pred] == hunter_x[p][i]
                    && pred_y[pred_team][pred] == hunter_y[p][i]) {
                pred_hit[pred_team][pred] = true;
                num_of_pred_hit[p]++;
                VLOG(1) << "Predator hit by hunter" << endl;
                VLOG(1)
                        << "Predator was at " << pred_x[pred_team][pred] << ", "
                                << pred_y[pred_team][pred] << endl;
                VLOG(1) << "Hunter was at " << hunter_x[p][i] << ", " << hunter_y[p][i] << endl;
            }
        }
    }
}

void PredPreyST2::showPred(int pred_team, int pred, int old_pred_x, int old_pred_y)
{
    double color1 = 0;
    double color2 = 0;
    double color3 = 0;

    fprintf(fptr, "Predator");

    //Saving Predator-Prey location
    if (pred_killed[pred_team][pred] == false || pred_hit[pred_team][pred_team] == false) {
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

void PredPreyST2::performHunterAction_complex(int hunter_team, int hunter)
{  //Added the argument int prey to check which prey is being processed ******PADMINI
    double nearestDist = MAP_LENGTH * 2 + 1;  //Big number

//    int nearestPred = 0;  //To hold the nearest predator to the prey
//    int nearestPred_team = 0;

    //double dist;

    int x_dist, y_dist, temp;

    //int i;  //Counter for loops ******PADMINI

    int hunterAction;
    //int act = 0;

    int old_hunter_x = hunter_x[hunter_team][hunter];
    int old_hunter_y = hunter_y[hunter_team][hunter];

    /*if (prey_caught[hunter_team][hunter]) {
     hunterAction = 4;
     } else {*/
    //IS_PREY = true;
    /*
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
     }*/

    // calculate predator offset with wraparound
    x_dist = pred_x[0][0] - hunter_x[0][0];

    if ((abs(x_dist)) > (MAP_LENGTH / 2)) {
        temp = x_dist;
        x_dist = MAP_LENGTH - abs(x_dist);
        if (temp > 0)
            x_dist = 0 - x_dist;
    }

    y_dist = pred_y[0][0] - hunter_y[0][0];

    if ((abs(y_dist)) > (MAP_HEIGHT / 2)) {
        temp = y_dist;
        y_dist = MAP_HEIGHT - abs(y_dist);
        if (temp > 0)
            y_dist = 0 - y_dist;
    }

    //DLOG(INFO) << "Distances are " << x_dist << ", " << y_dist;
    //DLOG(INFO) << "hunter_move_probability is " << hunter_move_probability;

    if (drand48() > hunter_move_probability) {
        hunterAction = (int) (drand48() * 4.0);
    } else {
        if (y_dist < 0 && (abs(y_dist) >= abs(x_dist))) {
            hunterAction = 2;
        }
        else if (x_dist < 0 && (abs(x_dist) >= abs(y_dist))) {
            hunterAction = 3;
        }

        else if (y_dist > 0 && (abs(y_dist) >= abs(x_dist))) {
            hunterAction = 0;
        }

        else if (x_dist > 0 && (abs(x_dist) >= abs(y_dist))) {
            hunterAction = 1;
        }

        else {
            hunterAction = 4;
        }
    }

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
        hunter_x[hunter_team][hunter] = 0; // -= MAP_LENGTH;
    if (hunter_y[hunter_team][hunter] > MAP_HEIGHT)
        hunter_y[hunter_team][hunter] = 0; // -= MAP_HEIGHT;
    if (hunter_x[hunter_team][hunter] < 0)
        hunter_x[hunter_team][hunter] = 0; // += MAP_LENGTH;
    if (hunter_y[hunter_team][hunter] < 0)
        hunter_y[hunter_team][hunter] = 0; // += MAP_HEIGHT;

    // Code for showing simulation in UI
    if (SHOW) {
        showHunter(hunter_team, hunter, old_hunter_x, old_hunter_y);
    }
}

void PredPreyST2::showHunter(int team, int individual, int old_x, int old_y)
{
    fprintf(fptr, "Hunter");

    // Saving hunter location
    fprintf(fptr, "%d ", hunter_x[team][individual]);  // int generation
    fprintf(fptr, " ");  //  character
    fprintf(fptr, "%d ", hunter_y[team][individual]);  // int generation
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

    glVertex3f(old_x, old_y, 0);
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

    // draw new prey point
    glBegin(GL_POINTS);
    {
        GLint err = glGetError();
        if (err != GL_NO_ERROR)
            cout << "19. Error" << endl;
    }  //ADI

    if (team == 0) {
        glColor3f(0.0f, 0.0f, 0.0f);
    }

    glVertex3f(hunter_x[team][individual], hunter_y[team][individual], 0);
    {
        GLint err = glGetError();
        if (err != GL_NO_ERROR)
            cout << "20. Error" << endl;
    }  //ADI

    glEnd();
    //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "21. Error" <<err<<endl; } //ADI : DONT know why this is not going even after changing gl end to void
    //}

    glFlush();
    //{ GLint err = glGetError(); if (err != GL_NO_ERROR) cout<< "22. Error" <<endl; } //ADI
}

// Make task harder
void PredPreyST2::nextTask()
{
    /*
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
     << prey_flee_factor << endl << endl;*/

}

// Make task simpler
void PredPreyST2::simplifyTask()
{
    /*
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
     << " , prey_flee_factor: " << prey_flee_factor << endl << endl;*/

}

void PredPreyST2::getTaskState(double* move, double* flee, int* num)
{
    /*
     *move = prey_move_factor;
     *flee = prey_flee_factor;
     *num = task_number;*/
}

void PredPreyST2::setTaskState(double* move, double* flee, int* num)
{
    /*
     prey_move_factor = *move;
     prey_flee_factor = *flee;
     task_number = *num;*/
}

bool PredPreyST2::hardestTask()
{
    return (task_number == 5);
}

// Calculates distance (actual distance, using pythag. thm)
// Takes shortest distance, ie. will wrap around edges
double PredPreyST2::calc_dist(int x1, int y1, int x2, int y2)
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
int PredPreyST2::getMaxPos(const vector<double>& vec)
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
int PredPreyST2::getMaxPos_complex(const vector<double>& vec)
{

    vector<double> temp_sum_outputs;

    if (pred_communication == true) {  //Aditya : March 03 - Communication
        for (int i = 0; i < NUM_OUTPUTS; i++) {
            temp_sum_outputs.push_back(0.0);
            for (int j = 0; j < num_of_predators - 1; j++) {
                //if (!prey_caught[j]) {
                temp_sum_outputs[i] = temp_sum_outputs[i] + vec[NUM_OUTPUTS * j + i];
                //}
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

