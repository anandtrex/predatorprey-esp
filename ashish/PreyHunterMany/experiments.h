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
#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"

using namespace std;

using namespace NEAT;

// PredatorPreyHunter routines ***************************************
Population* predatorpreyhunter_test( int gens, string pathFileGenome, string namePlot, const int mapWidth, const int mapHeight, const int noHunters );
double predatorpreyhunter_epoch( Population* pop, int maxGens, int generation, char* filename, const int mapWidth, const int mapHeight, const int noHunters );
double predatorpreyhunter_evaluate( Organism* org, const int mapWidth, const int mapHeight, const int noHunters );
double predatorpreyhunter_evaluate_storeperformance( Organism* org, string pathFile, const int mapWidth, const int mapHeight, const int noHunters );
void writeChampionPopulation( Population* pop );

#endif

