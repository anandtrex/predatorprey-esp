#include "Environment.h"

#define MAP_LENGTH 100
#define MAP_HEIGHT 100

/**
 * Defines the Predator-Prey environment
 */
class PredPrey: public Environment {
public:
	PredPrey(int num_of_predators, int num_of_prey, int num_teams_predator,
			int num_teams_prey, vector<double> prey_move_probability, int num_of_hunters);
	PredPrey(int num_of_predators, int num_of_prey, int num_teams_predator,
	            int num_teams_prey, vector<double> prey_move_probability);
	PredPrey(int num_of_predators, int num_teams_predator, int num_of_hunters, int num_teams_hunters);
	int num_of_predators;
	int num_of_prey;
	int num_teams_predator;
	int num_teams_prey;

	int num_of_hunters;
	int num_teams_hunters; // Hack to make code editing easier. Ideally, there are no teams of hunters, and this variable shouldn't exist

	int time_to_catch_prey;
	int steps;
	vector<double> prey_move_probability;
	vector<vector<double> > messages; //messages

	virtual vector<vector<vector<double> > > evalNet(
			vector<vector<Network*> >& team,
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
	static double count_overall_best_zebra_caught;
	static double count_generation_best_zebra_caught;
	static double count_generation_zebra_caught;
	static double count_zebra_caught_6;
	static double count_overall_best_mice_caught;
	static double count_generation_best_mice_caught;
	static double count_generation_mice_caught;
	static double count_mice_caught_6;
	static int previous_generation;

	double pred_flee_factor;


	double calc_dist(int x1, int y1, int x2, int y2);
	int getMaxPos(const vector<double>&);
	int getMaxPos_complex(const vector<double>&);

	void init(bool preyRandom, bool predsRandom, bool huntersRandom, int generation);
	void init(bool preyRandom, bool predsRandom, int generation);
	void reset_prey_position(int prey_team, int prey);
	void setupInput(int, vector<double>&);
//	void setupInput_complex_prey(int, vector<vector<double> >&,
//			vector<vector<Network*> >& team_prey);
	void setupInput_complex_predator(/*int, */vector<vector<double> >&,
			vector<vector<Network*> >& team);
	void performPredAction(int pred, const vector<double>&);
	void performPredAction_complex(int pred_team, int pred,
			const vector<double>&);
	void performPreyAction(int prey, const vector<double>&);
	void performPreyAction_complex(int prey_team, int prey);
    void performHunterAction_complex(int hunter_team, int hunter);

    // Display functions
    void showHunter(int hunter_team, int hunter, int old_hunter_x, int old_hunter_y);
    void showPred(int pred_team, int pred, int old_pred_x, int old_pred_y);
    void showPrey(int prey_team, int prey, int old_prey_x, int old_prey_y);

	// ---- variables ----

	// variables used for incremental learning
	int task_number;
	double prey_move_factor; // possibility prey moves
	double prey_flee_factor; // possibility prey moves away from pred
							 // otherwise random

							 // --- The following are set and used during evaluations
	int numPreds;

	int numPrey; //To store the number of prey in the world ******PADMINI

	// prey_x[prey_team][prey]
	vector<vector<int> > pred_x;
	vector<vector<int> > pred_y;

	vector<vector<int> > prey_x;
	vector<vector<int> > prey_y;

	vector<vector<int> > hunter_x;
	vector<vector<int> > hunter_y;

	vector<vector<int> > prey_lifetime;

	vector<vector<int> > pred_energy;
	vector<vector<int> > prey_energy;

	vector<vector<int> > num_of_prey_caught_individually;

	vector<vector<int> > pred_hit_wall_times;

	vector<vector<bool> > prey_caught;
	// Predators hit by hunter
	vector<vector<bool> > pred_hit;
	vector<int> num_of_pred_hit;
	// the number of opponent predators killed by each team
	vector<vector<bool> > pred_killed;
	vector<int> num_of_pred_kills;
	vector<int> num_of_prey_caught;

};

