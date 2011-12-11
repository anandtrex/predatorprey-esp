// includes
// neat
#include "genome.h"
#include "gene.h"
#include "organism.h"
#include "network.h"
// std
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// using
// std
using std::ifstream;
using std::string;
using std::cerr;
using std::endl;
using std::cout;
using std::vector;
// NEAT
using NEAT::Genome;
using NEAT::Gene;
using NEAT::Organism;
using NEAT::Network;
// declarations
void convertToGraphVizFormat( Genome* ptrGenome );
// main
int main( int argc, char* argv[] ) {
  if ( argc < 2 ) {
    cerr << "Expected file name of genome file as argument!" << endl;
    return -1;
  }
  string pathGenome( argv[1] );
  ifstream fin( pathGenome.c_str() );
  if ( !fin ) {
	  cerr << "Could not open file " << argv[1] << endl;
	  return -1;
  }
  string word;
  int genomeId;
  fin >> word;
  fin >> genomeId;
  cout << word << " " << genomeId << " [works]" << endl;
  Genome* ptrGenome = new Genome( genomeId, fin );
  fin.close();
  ptrGenome->print_to_file( cout ); // print to console
  Organism* ptrOrganism = new Organism( 0, ptrGenome, 1 );
  cout << "Organism" << endl;
  ptrOrganism->write_to_file( cout );
  Network* ptrNetwork = ptrOrganism->net;
  cout << "Network node count: " << ptrNetwork->nodecount() << endl;
  string pathFileLinks = pathGenome + ".links.txt";
  ptrNetwork->print_links_tofile( const_cast<char*> ( pathFileLinks.c_str() ) );
  string pathFileDot = pathGenome + ".dot";
  ptrNetwork->writeToDotFile( const_cast<char*> ( pathFileDot.c_str() ) );
  delete ptrOrganism; //  genome deleted automatically by organism object. No need for delete ptrGenome;
  return 0;
}

// other function definitions
void convertToGraphVizFormat( Genome* ptrGenome ) {
	// typedefs
	typedef vector<Gene*> VG;
	typedef VG::const_iterator VGCI;
	// simply iterate through all nodes of the genome for now
	VG &vg = ptrGenome->genes;
	for ( VGCI it = vg.begin(); it != vg.end(); ++it )
		;
}

