#include "Environment.h"

//#define MAP_LENGTH 100
//#define MAP_HEIGHT 100

/**
 * Defines the Predator-Prey environment
 * Subtask 2 : Only predators and hunters. No Prey.
 */
class PredPreyST2: public Environment
{
public:
    PredPreyST2(int num_of_predators, int num_teams_predator, int num_of_hunters);
    PredPreyST2(int num_of_predators, int num_teams_predator, int num_of_hunters,
            int num_teams_hunters);
    int num_of_predators;
    int num_teams_predator;

    int num_of_hunters;
    int num_teams_hunters;  // Hack to make code editing easier. Ideally, there are no teams of hunters, and this variable shouldn't exist

    int steps;
    vector<vector<double> > messages;  //messages

    virtual vector<vector<vector<double> > > evalNet(vector<vector<Network*> >& team,
            int generation);
    virtual vector<vector<double> > testNet(vector<vector<Network*> >& team,
             int trials);

    virtual void nextTask();
    virtual void simplifyTask();
    virtual void getTaskState(double*, double*, int*);
    virtual void setTaskState(double*, double*, int*);
    virtual bool hardestTask();

private:

    static int count_trials;
    static int previous_generation;

    double pred_flee_factor;

    double calc_dist(int x1, int y1, int x2, int y2);
    int getMaxPos(const vector<double>&);
    int getMaxPos_complex(const vector<double>&);

    void init(bool predsRandom, bool huntersRandom, int generation);
    void setupInput(int, vector<double>&);
    void setupInput_complex_predator(/*int, */vector<vector<double> >&,
            vector<vector<Network*> >& team);
    void performPredAction(int pred, const vector<double>&);
    void performPredAction_complex(int pred_team, int pred, const vector<double>&);
    void performHunterAction_complex(int hunter_team, int hunter);

    // Display functions
    void showHunter(int hunter_team, int hunter, int old_hunter_x, int old_hunter_y);
    void showPred(int pred_team, int pred, int old_pred_x, int old_pred_y);

//    virtual vector<vector<vector<double> > > evalNet(vector<vector<Network*> >& team,
//            int generation);
//    virtual vector<vector<double> > testNet(vector<vector<Network*> >& team, int trials);

    // ---- variables ----

    // variables used for incremental learning
    int task_number;

    // --- The following are set and used during evaluations
    int numPreds;

    vector<vector<int> > pred_x;
    vector<vector<int> > pred_y;

    vector<vector<int> > hunter_x;
    vector<vector<int> > hunter_y;

    vector<vector<int> > pred_energy;

    vector<vector<int> > pred_hit_wall_times;

    // Predators hit by hunter
    vector<vector<bool> > pred_hit;
    vector<int> num_of_pred_hit;
    // the number of opponent predators killed by each team
    vector<vector<bool> > pred_killed;
    vector<int> num_of_pred_kills;
};

