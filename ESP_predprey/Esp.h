//////////////////////////////////////////////////////////////////////
//
// SANE definitions
//
//////////////////////////////////////////////////////////////////////

#include <vector>
//#include <deque.h>
#include <fstream>

#ifndef _ESP_
#define _ESP_

extern int MIN;

#define FF 0
#define SRN 1
#define SCDORDER 2
#define FR 3

//typedef enum { FF, SRN, SCDORDER } NType;

/*
 typedef struct {
 int numPops;
 int popSize;
 int seed;
 //  NType *netType;
 } espParams;
 */

class neuron
{
public:
    // weight[0..n-1] are input weights, for n inputs
    // weight[n..n+k-1] are output weights, for k outputs
    vector<double> weight;
//  vector<double> delta;
    double fitness;  //neuron's fitness value
    int tests;  //holds the total # of networks participated in
    double activation;  //neuron's activation level
    // QUE What does lesioned do?
    bool lesioned;

    //constructor
    neuron();
    ~neuron();
    neuron operator=(const neuron &);
    void create();
    void perturb(const neuron &n);
    void perturb(const neuron &n, double(*randFn)(double), double);
    neuron* perturb();
};

//////////////////////////////////////////////////////////////////////
//
// Encapsulate the neuron sub population structure
//
//////////////////////////////////////////////////////////////////////

class Network;
class subPop
{

public:
    //protected:
    vector<neuron*> pop;

    // Second Hidden layer
    vector<neuron*> pop2;

//public:
    int numNeurons;
    int numNeurons2;

    subPop(int size);
    ~subPop();
    //  subPop(const subPop &s);
    //void addHiddenLayer(int size);
    void create();  // creates a random subpopulation of neurons
    void evalReset();
    neuron *selectNeuron();
    void average();
    void qsortNeurons();
    void recombine();
    void recombine_hall_of_fame(int pred_or_prey_team, int pred_or_prey_number, int pop_number);  //EVOLVE_PREY
    void burstMutate();
    void mutate();
    void deltify(neuron *bestNeuron);
    void save(char *fname);
    void addConnection(int locus);
    void removeConnection(int locus);
    void saveBin(char *fname);
    void saveText(char *fname);
    int load(char *filename);
    void print();
    void printWeight(FILE *file);
    void printDelta(FILE *file);

    bool evolvable;

private:
    int numBreed;  //number of neurons to be mated in subpop
    void crossover(const vector<double> &parent1, const vector<double> &parent2,
            vector<double> &child1, vector<double> &child2);
    int findMate(int num);
};

class Esp
{
public:

    // constructor for new Esp
    Esp(int nPreds, int nPreys, int num_teams_predator, int num_teams_prey, int num_teams_hunteres,
            int num_of_hunters, int nPops, int nSize, Environment &e, int netTp);

    // for two subtasks, it's two hall_of_fame_pred vectors
    Esp(vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred,
            int nPreds, int nPreys, int num_teams_predator, int num_teams_prey, int num_teams_hunteres,
            int num_of_hunters, int nPops, int nSize, Environment &e, int netTp);

    // constructor to load an old Esp from file
    Esp(char* fname, int nPreds, int nPreys, int num_teams_predator, int num_teams_prey,
            int num_teams_hunters, int num_of_hunters, int nPops, int nSize, Environment &e,
            int netTp);

    ~Esp();

    void create(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey);  // creates a random population of neurons
    void evolve(int cycles, int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey, int num_teams_hunters, int num_of_hunters);
    void evalPop(int num_of_predators, int num_of_prey, int num_teams_predator, int num_teams_prey, int num_teams_hunters, int num_of_hunters);
    void findChampion();
    void loadSeedNet(char *);
    void addSubPop(int pred, int num_of_predators, int num_of_prey);

    void endEvolution();

    static vector<vector<vector<Network*> > > hall_of_fame_pred;  // EVOLVE_PREY vector of vector of best networks
    //static vector<vector<vector<Network*> > > hall_of_fame_prey;  // EVOLVE_PREY vector of vector of best networks

private:
    int total_predator_networks_per_team;
    //int total_prey_networks_per_team;
    vector<int> num_hidden_neurons;  // was called nPops

    vector<vector<vector<subPop*> > > hidden_neuron_populations;  // num teams x num networks x num hidden neurons per network
    //vector<vector<vector<subPop*> > > hidden_neuron_populations_prey;

    // sub networks from previous sub-tasks
    vector<vector<vector<vector<Network*> > > > sub_hall_of_fame_pred;

    int numTrials;
    int netType;
    //deque<double> perfQ;
    Environment &Envt;
    vector<vector<Network*> > overall_best_teams;
    vector<double> overall_best_teams_sum_individual_fitness;  //Sum of individual network fitnesses
    //vector<vector<Network*> > overall_best_teams_prey;
    //vector<double> overall_best_teams_prey_sum_individual_fitness;  //Sum of individual network fitnesses
    vector<vector<Network*> > bestNetwork;
    //vector<vector<Network*> > bestNetwork_prey;
    int generation;

    ofstream fout_champfitness;

    Esp(const Esp &);  // just for safety
    Esp &operator=(const Esp &);

    void save(char* fname, int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey);
    subPop* load(char *fname);
    void setupNetDimensions(int num_of_predators, int num_of_prey);
    Network* generateNetwork(int networkType, int num_hidden_neurons);
    void evalReset(int num_teams_predator, int num_teams_prey);
    void performEval(int num_of_predators, int num_of_prey, int num_teams_predator,
            int num_teams_prey, int num_teams_hunters, int num_of_hunters);
    void average(int num_teams_predator, int num_teams_prey);
    void printNeurons();
    void handleStagnation(int num_of_predators, int num_of_prey);
    void newDeltaPhase(int pred);
    int removeSubPop(int sp, int pred, int num_of_predators, int num_of_prey);
    int removeSubPop(vector<Network*>& team, int pred, int num_of_predators, int num_of_prey);
    void addConnections();
    void removeConnections(int sp);
    void printStats();
};
//  class NeuroEvolution {
//    int numPops;  
//    int totalNumNeurons;
//    Environment &Envt;
//    vector<subPop*> pops;
//   			// neurons 
//    // constructor
//    NeuroEvolution(int nPops, int nSize, Environment &e, int netTp);

//    void create(); // creates a random population of neurons
//    void virtual evolve(int cycles) = 0;
//    void virtual evalPop() = 0;
//    //void findChampion();
//    //void loadSeedNet(char *);
//    //void addSubPop();
//    //Network *bestNetwork;//make private after debug
//    //Network *overall_best_teams;  
//    //void endEvolution();
//  };

#endif

