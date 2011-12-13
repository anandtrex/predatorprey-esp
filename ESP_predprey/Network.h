#ifndef _NET_
#define _NET_

#include "Esp.h"
extern int MIN;

class Network: public subPop
{
protected:
    vector<double> activation;

public:
    double fitness;

    Network(int size) :
            subPop(size), activation(size)
    {
        if (MIN)
            fitness = 10000000;
        else
            fitness = 0.0;
    }

    virtual ~Network();

    void addHiddenLayer(/*int size, */vector<neuron*> pop)
    {
        numNeurons2 = pop.size();
        //pop2 = new vector<neuron*>(size)
        pop2 = pop;
    }

    void operator=(Network &n);
    void resetActivation();
    void setNeuron(neuron *n, int position);
    void setNetwork(Network *n);

    virtual void addNeuron() = 0;
    virtual void removeNeuron(int sp) = 0;

    int lesion(Environment &e, vector<Network*>& team, int num_of_predators, int num_of_prey);
    void addFitness();
    void incrementTests();
    virtual void activate(vector<double> &input, vector<double> &output, /*int num_of_prey,
            int num_of_predators, int num_teams_prey, int num_teams_predator,*/
            int inputSize_combiner) = 0;

    virtual void save(char *filename) = 0;
};

class FeedForwardNetwork: public Network
{
public:
    FeedForwardNetwork(int size) :
            Network(size)
    {
        ;
    }

    void activate(vector<double> &input, vector<double> &output,/* int num_of_prey,
            int num_of_predators, int num_teams_prey, int num_teams_predator,*/
            int inputSize_combiner);
    void addNeuron();
    void removeNeuron(int sp);
    void save(char *filename);
};

class FullyRecurrentNetwork: public Network
{
public:
    FullyRecurrentNetwork(int size, int r = 2) :
            Network(size)
    {
        relax = r;
    }

    void activate(vector<double> &input, vector<double> &output);
    void addNeuron();
    void removeNeuron(int sp);
    void save(char *filename);
private:
    int relax;
};

class SimpleRecurrentNetwork: public Network
{
public:
    SimpleRecurrentNetwork(int size) :
            Network(size)
    {
        ;
    }

    void activate(vector<double> &input, vector<double> &output);
    void addNeuron();
    void removeNeuron(int sp);
    void save(char *filename);
};

class SecondOrderRecurrentNetwork: public Network
{
public:
    SecondOrderRecurrentNetwork(int size);

    void activate(vector<double> &input, vector<double> &output);
    void addNeuron();
    void removeNeuron(int sp);
    void save(char *filename);
private:
    int outOffset;
    vector<vector<double> > newWeights;
};

#endif
