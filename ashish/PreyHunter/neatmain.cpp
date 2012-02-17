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
#include <vector>
#include "neat.h"
#include "population.h"
#include "experiments.h"
using namespace std;

int main(int argc, char *argv[]) {

  //list<NNode*> nodelist;

  int pause;

  NEAT::Population *p=0;

  //***********RANDOM SETUP***************//
  /* Seed the random-number generator with current time so that
      the numbers will be different every time we run.    */
  srand( (unsigned)time( NULL ) );

  if (argc < 4) {
    cerr << "A NEAT parameters file (.ne file) is required to run the experiments!" << endl;
    cerr << "path of starter genome file is expected" << endl;
    cerr << "name for plotting is expected" << endl;
    return -1;
  }

  //Load in the params
  NEAT::load_neat_params(argv[1],true);

  cout<<"loaded"<<endl;

  //Here is an example of how to run an experiment directly from main
  //and then visualize the speciation that took place
  //p=xor_test(100);  //100 generation XOR experiment

  cout << "Beginnning predatorpreyhunter_test";
  p = predatorpreyhunter_test( 100, argv[2], argv[3] ); // running for 100 generations

  if (p)
    delete p;

  return(0);
 
}

