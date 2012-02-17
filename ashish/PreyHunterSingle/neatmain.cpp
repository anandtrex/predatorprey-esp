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
#include <iostream>
#include <sstream>
#include <vector>
#include "neat.h"
#include "population.h"
#include "experiments.h"
using namespace std;

int extract( string s ) {
  istringstream sin( s );
  int x;
  sin >> x;
  return x;
}

int main(int argc, char *argv[]) {

  //list<NNode*> nodelist;

  int pause;

  NEAT::Population *p=0;

  //***********RANDOM SETUP***************//
  /* Seed the random-number generator with current time so that
      the numbers will be different every time we run.    */
  srand( (unsigned)time( NULL ) );

  if (argc < 7) {
    cerr << "1. A NEAT parameters file (.ne file) is required to run the experiments!" << endl; // ARG 1
    cerr << "2. number of generations is expected" << endl; // ARG 2
    cerr << "3. map width is expected" << endl; // ARG 3
    cerr << "4. map height is expected" << endl; // ARG 4
    cerr << "5. path of starter genome file is expected" << endl; // ARG 5
    cerr << "6. name for plotting is expected" << endl; // ARG 6
    return -1;
  }

  //Load in the params
  NEAT::load_neat_params(argv[1],true);

  cout<<"loaded"<<endl;

  //Here is an example of how to run an experiment directly from main
  //and then visualize the speciation that took place
  //p=xor_test(100);  //100 generation XOR experiment

  // get the required parameters
  istringstream sin;
  int gens = extract( argv[2] ); 
  int mapWidth = extract( argv[3] );
  int mapHeight = extract( argv[4] );
  string pathStarterGenomeFile( argv[5] );
  string namePlotFile( argv[6] );
  cout << "Beginnning predatorpreyhunter_test";
  p = predatorpreyhunter_test( gens, pathStarterGenomeFile, namePlotFile, mapWidth, mapHeight ); // running for 100 generations

  if (p)
    delete p;

  return(0);
 
}

