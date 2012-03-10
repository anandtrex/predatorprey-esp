/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "experiments.h"
#include "DomainManyHunters.h"
#include <cstring>

//#define NO_SCREEN_OUT 

// PREDATOR PREY HUNTER CODE BEGINS
void writeChampionPopulation( Population* pop ) {
  
}

//Perform evolution on predator prey hunter, for gens generations
Population *predatorpreyhunter_test(int gens, string pathFileGenome, string namePlot, const int mapWidth, const int mapHeight, const int noHunters ) {
    Population *pop=0;
    Genome *start_genome;
    char curword[20];
    int id;

    //ostringstream *fnamebuf;
    int gen;
    int expcount;

    double championFitness; // store the fitness of the champ of each epoch
    vector<double> vGenerationChamptionFitness;
    vGenerationChamptionFitness.reserve( gens );

    cout<<"START SINGLE PREDATOR PREY HUNTER EVOLUTION"<<endl;
    cout<<"Reading in the start genome"<<endl;
    //string pathFileGenomeStart = "singlepreyhunter_startgenes";
    string pathFileGenomeStart = pathFileGenome; 
    ifstream iFile( pathFileGenomeStart.c_str(), ios::in );
    if ( !iFile.is_open() ) {
      cerr << "I could not open " << pathFileGenomeStart << " for reading the starter genome" << endl;
      throw 1; // throw something meaningful later
    }
    //Read in the start Genome
    iFile>>curword;
    iFile>>id;
    cout<<"Reading in Genome id "<<id<<endl;
    start_genome=new Genome(id,iFile);
    iFile.close();
  
    //Run multiple experiments
    cout << "TOTAL EXPERIMENTS: " << NEAT::num_runs << endl;
    for(expcount=0;expcount<NEAT::num_runs;expcount++) {
      vGenerationChamptionFitness.clear();
      cout<<"EXPERIMENT #"<<expcount<<endl;
      cout<<"Start Genome: "<<start_genome<<endl;
      //Spawn the Population
      cout<<"Spawning Population off Genome"<<endl;
      pop=new Population(start_genome,NEAT::pop_size);
      cout<<"Verifying Spawned Pop"<<endl;
      pop->verify();
      cout << "TOTAL GENERATIONS: " << gens << endl;
      for (gen=1;gen<=gens;gen++) {
        cout<<"Generation "<<gen<<endl;
        //fnamebuf=new ostringstream();
        //(*fnamebuf)<<"gen_"<<gen<<"_"; // ends;  //needs end marker
        //cout<<"name of fname: "<<fnamebuf->str()<<endl;
        char temp[50];
        sprintf (temp, "PositionsGenerationChamp%d", gen);
        championFitness=predatorpreyhunter_epoch(pop,gen,temp,mapWidth,mapHeight,noHunters);
        vGenerationChamptionFitness.push_back( championFitness );
        //fnamebuf->clear();
        //delete fnamebuf;
      } // end of for loop generations
      // write chamption fitness scores of generations to file
      ostringstream sout;
      sout << "PlotExperiment" << expcount << "-" << namePlot << ".txt";
      ofstream fout( sout.str().c_str() );
      if ( !fout.is_open() ) {
        cerr << "Could not open file " << sout.str() << " for writing." << endl;
        throw 1; // throw something meaningful later
      }
      gen = 1;
      for ( vector<double>::const_iterator it = vGenerationChamptionFitness.begin(); it != vGenerationChamptionFitness.end(); ++it ) {
        fout << gen << " " << *it << endl;
        gen++;
      }
      fout.close();
      if ( expcount < NEAT::num_runs - 1 ) delete pop; // delete all but the last population
    } // end of for loop experiments
    return pop;
}

double predatorpreyhunter_epoch(Population *pop,int generation,char *filename, const int mapWidth, const int mapHeight, const int noHunters) {
  vector<Organism*>::iterator curorg;
  vector<Species*>::iterator curspecies;
  //char cfilename[100];
  //strncpy( cfilename, filename.c_str(), 100 );

  //ofstream cfilename(filename.c_str());

  bool win=false;
  int winnernum;

  //Evaluate each organism on a test
  //store the iterator of the organism with the greatest fitness
  curorg=(pop->organisms).begin();
  double fitnessAveragePopulation = 0.0;
  double fitnessGreatest = predatorpreyhunter_evaluate(*curorg, mapWidth, mapHeight, noHunters); // get the fitness of the first organism
  fitnessAveragePopulation = fitnessGreatest;
  vector<Organism*>::iterator itPtrOrgChamp = curorg;
  ++curorg; // move to the next organism
  for( ;curorg!=(pop->organisms).end(); ++curorg) {
    double fitnessOrganism = predatorpreyhunter_evaluate(*curorg, mapWidth, mapHeight, noHunters);
    fitnessAveragePopulation = fitnessAveragePopulation + fitnessOrganism;
    cout << "FITNESS: " << fitnessOrganism << endl;
    if ( fitnessGreatest < fitnessOrganism ) { // if fitness of current organism greater than the best make it the champ
      fitnessGreatest = fitnessOrganism; 
      itPtrOrgChamp = curorg;
    }
  }
  fitnessAveragePopulation = fitnessAveragePopulation / (pop->organisms).size();
  
  // run the evaluation again but just once and store the performance to a file
  if ( itPtrOrgChamp == (pop->organisms).end() ) {
    cerr << "predatorpreyhunter_epoch(): itPtrOrgChamp is null!" << endl;
    throw 1; // throw something meaningful later
  }
  string pathFile = string( filename ) + ".txt";
  double fitnessOrganism = predatorpreyhunter_evaluate_storeperformance( *itPtrOrgChamp, pathFile, mapWidth, mapHeight, noHunters ); 
  ostringstream sout;
  sout << "NetworkGenerationChamp" << generation << ".txt"; 
  ofstream foutGenome( sout.str().c_str() );
  Genome* genomeChamp = ( *itPtrOrgChamp )->gnome;
  genomeChamp->print_to_file( foutGenome );
  // TODO store the champ organism itself to file

  //Average and max their fitnesses for dumping to file and snapshot
  for(curspecies=(pop->species).begin();curspecies!=(pop->species).end();++curspecies) {

    //This experiment control routine issues commands to collect ave
    //and max fitness, as opposed to having the snapshot do it, 
    //because this allows flexibility in terms of what time
    //to observe fitnesses at

    (*curspecies)->compute_average_fitness();
    (*curspecies)->compute_max_fitness();
  }

  //Take a snapshot of the population, so that it can be
  //visualized later on
  //if ((generation%1)==0)
  //  pop->snapshot();

  //Only print to file every print_every generations
  if  ( ( generation % ( NEAT::print_every ) )==0 ) {
    pop->print_to_file_by_species(filename);
  }

  //if (win) {
    //for(curorg=(pop->organisms).begin();curorg!=(pop->organisms).end();++curorg) {
      //if ((*curorg)->winner) {
        //winnernum=((*curorg)->gnome)->genome_id;
        //cout<<"WINNER IS #"<<((*curorg)->gnome)->genome_id<<endl;
      //}
    //}    
  //}

  //Create the next generation
  pop->epoch(generation);

  return fitnessGreatest; // fitnessAveragePopulation; 
}

double predatorpreyhunter_evaluate(Organism *org, const int mapWidth, const int mapHeight, const int noHunters) {
  Network *net;

  int numnodes;  /* Used to figure out how many nodes
		    should be visited during activation */
  int thresh;  /* How many visits will be allowed before giving up 
		  (for loop detection) */

  //  int MAX_STEPS=120000;
  int MAX_STEPS=mapWidth*mapHeight; // grid size will be smaller like 10x10
  
  net=org->net;
  numnodes=((org->gnome)->nodes).size();
  thresh=numnodes*2;  // Max number of visits allowed per activation
  
  double sumFitness= 0.0; // I tried harmonic mean, it's not a good idea for even one negative evaluation ruins the whole thing
  int MAP_WIDTH = mapWidth; // whole code needs to be reorganized
  int MAP_HEIGHT = mapHeight; // whole code needs to be reorganized
  int noTrials = 10; // 10 TRIALS
  for ( int i = 0; i < noTrials; i++ ) {
    PredatorPreyHunter::DomainManyHunters domainManyHunters( MAX_STEPS, MAP_WIDTH, MAP_HEIGHT, net, noHunters );   
    sumFitness += domainManyHunters.run(); 
  }
  org->fitness = sumFitness / noTrials; // arithmetic mean

  cout<<"Org "<<(org->gnome)->genome_id<<" fitness: "<<org->fitness<<endl;

  return org->fitness;
}

double predatorpreyhunter_evaluate_storeperformance( Organism* org, string pathFile, const int mapWidth, const int mapHeight, const int noHunters ) {
  Network *net;

  int numnodes;  /* Used to figure out how many nodes
		    should be visited during activation */
  int thresh;  /* How many visits will be allowed before giving up 
		  (for loop detection) */

  //  int MAX_STEPS=120000;
  int MAX_STEPS=mapWidth*mapHeight; // grid size will be smaller like 10x10
  
  net=org->net;
  numnodes=((org->gnome)->nodes).size();
  thresh=numnodes*2;  //Max number of visits allowed per activation
  
  //Try to balance a pole now
  double fitnessOrganism = 0.0;
  int MAP_WIDTH = mapWidth; // whole code needs to change
  int MAP_HEIGHT = mapHeight; // whole code needs to change
  PredatorPreyHunter::DomainManyHunters domainManyHunters( MAX_STEPS, MAP_WIDTH, MAP_HEIGHT, net, noHunters );   
  fitnessOrganism = domainManyHunters.run( pathFile ); 
  org->fitness = fitnessOrganism; 

  cout<<"Org "<<(org->gnome)->genome_id<<" fitness: "<<org->fitness<<endl;

  return org->fitness;
}

// PREDATOR PREY HUNTER CODE ENDS

