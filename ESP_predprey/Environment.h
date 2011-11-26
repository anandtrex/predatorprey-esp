//////////////////////////////////////////////////////////////////////
//
// Environment
//
// Virtual Class for modelling a specific environment/task.
// 
//////////////////////////////////////////////////////////////////////

// The only function that really needs to do anything when implemented
// is evalNet. It must take a Network and return a double.  That is 
// evalNet is not restricted in any way in what it does with the 
// Network (except alter its wieghts) as long as it returns 
// a double (fitness).

#ifndef _ENVT_
#define _ENVT_
#include <vector>
using namespace std;

extern int NUM_OUTPUT_PRED_COMBINER;
extern int NUM_OUTPUT_PREY_COMBINER;
extern int NUM_INPUTS_PRED_COMBINER;
extern int NUM_INPUTS_PREY_COMBINER;
extern int NUM_INPUTS;
extern int NUM_INPUTS_PREY;
extern int NUM_OUTPUTS;
extern int NUM_OUTPUTS_PREY;

class Network;
class Environment {

public:
	Environment() {
		;
	}

	virtual ~Environment() {
		;
	}

	// evaluate a team
	virtual vector<vector<vector<double> > > evalNet(
			vector<vector<Network*> >& team,
			vector<vector<Network*> >& team_prey, int generation) = 0;

	// test a team for numTrials times, return num succeeded
	virtual vector<vector<double> > testNet(vector<vector<Network*> >& team,
			vector<vector<Network*> >& team_prey, int trials) = 0;

	virtual void nextTask() = 0; // transition to a harder task
	virtual void simplifyTask() = 0; // make the current task easier
	virtual void getTaskState(double*, double*, int*) =0;
	virtual void setTaskState(double*, double*, int*) =0;
	virtual bool hardestTask() = 0;

	int inputSize; // input space dimension; must be set in constructor
	int inputSize_prey; // input space dimension; must be set in constructor
	int inputSize_prey_combiner; // input space dimension; must be set in constructor
	int inputSize_pred_combiner; // input space dimension; must be set in constructor
	int outputSize_prey_combiner; // input space dimension; must be set in constructor
	int outputSize_pred_combiner; // input space dimension; must be set in constructor
	int outputSize; // output space dimension;          "
	int outputSize_prey; // output space dimension;          "
	double maxFitness;

};

#endif
