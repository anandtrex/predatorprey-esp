// includes
// neat
#include "genome.h"
#include "gene.h"
#include "organism.h"
#include "network.h"
#include "nnode.h"
#include "trait.h"
#include "link.h"
// std
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
// using
// std
using std::ifstream;
using std::string;
using std::cerr;
using std::endl;
using std::cout;
using std::vector;
using std::pair;
using std::map;
// NEAT
using NEAT::Genome;
using NEAT::Gene;
using NEAT::Organism;
using NEAT::Network;
// typedefs
typedef unsigned int uint;
// forced to use int here as node_id in NEAT code uses int
typedef pair< Genome, int > GenomeNodeId; // stands for Genome and Node Id
typedef map< int, GenomeNodeId > Index;
typedef map< GenomeNodeId, int > InvertedIndex;
// declarations
void convertToGraphVizFormat( Genome* ptrGenome );
Genome* extractGenome( const string& pathFileGenome );
bool areCompatible( vector<Genome*>& vPtrGenome );
void aggregateGenomes( 
    vector<Genome*>& vPtrGenome,
    Genome** ptrDblGenome,
    Index** ptrDblIndex,
    InvertedIndex** ptrDblInvertedIndex
  );
// main
int main( int argc, char* argv[] ) {
  if ( argc < 2 ) {
    cerr << "Expected file names of genome files as argument!" << endl;
    return -1;
  }
  // displaying arguments
  cout << "Arguments supplied: " << endl;
  for ( int i = 0; i < argc; i++ ) {
    cout << i+1 << ". " << argv[i] << endl;
  }
  vector<string> vPathFileGenome( argc );
  vPathFileGenome.clear(); // strangely, I have to specify this otherwise there are undefined elements in the vector
  for ( int i = 1; i < argc; i++ ) {
    vPathFileGenome.push_back( argv[i] );
  }
  // display list pathFileGenome
  cout << "Path of genome files: " << endl;
  int count = 1;
  for ( vector<string>::const_iterator it = vPathFileGenome.begin(); it != vPathFileGenome.end(); ++it ) {
    cout << count++ << ". " << *it << endl;
  }
  vector<Genome*> vPtrGenome( argc );
  vPtrGenome.clear(); // I shouldn't be doing this. Damn. What's wrong.
  for ( vector<string>::const_iterator it = vPathFileGenome.begin(); it != vPathFileGenome.end(); ++it ) {
    Genome* ptrGenome;
    ptrGenome = extractGenome( *it ); 
    vPtrGenome.push_back( ptrGenome );
  }
  // display the genomes
  cout << "Displaying genomes: " << endl;
  for ( vector<Genome*>::iterator it = vPtrGenome.begin(); it != vPtrGenome.end(); ++it ) {
    Genome* ptrGenome = *it;
    cout << "Displaying Genome: " << ptrGenome->genome_id << endl; 
    ptrGenome->print_to_file( cout ); // print to console
  }
  areCompatible( vPtrGenome );
  //Organism* ptrOrganism = new Organism( 0, ptrGenome, 1 );
  //cout << "Organism" << endl;
  //ptrOrganism->write_to_file( cout );
  //Network* ptrNetwork = ptrOrganism->net;
  //cout << "Network node count: " << ptrNetwork->nodecount() << endl;
  //string pathFileLinks = pathGenome + ".links.txt";
  //ptrNetwork->print_links_tofile( const_cast<char*> ( pathFileLinks.c_str() ) );
  //string pathFileDot = pathGenome + ".dot";
  //ptrNetwork->writeToDotFile( const_cast<char*> ( pathFileDot.c_str() ) );
  //delete ptrOrganism; //  genome deleted automatically by organism object. No need for delete ptrGenome;
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

Genome* extractGenome( const string& pathFileGenome ) {
  ifstream fin( pathFileGenome.c_str() );
  if ( !fin ) {
	  cerr << "Could not open file " << pathFileGenome << endl;
    throw -1; // throw something meaningful later 
  }
  string word;
  int genomeId;
  fin >> word;
  fin >> genomeId;
  cout << "Extracting ";
  cout << word << " " << genomeId << endl;
  Genome* ptrGenome = new Genome( genomeId, fin );
  fin.close();
  return ptrGenome;
}

uint noOutputNodes( Genome* ptrGenome ) {
  using NEAT::NNode;
  using NEAT::OUTPUT;
  typedef vector<NNode*> V;
  typedef V::iterator VI;
  if ( ptrGenome == NULL ) {
    cerr << "you passed a NULL pointer. What the fuck man?" << endl;
    throw 1; // throw something meaningful later
  }
  uint count = 0;
  for ( VI it = ptrGenome->nodes.begin(); it != ptrGenome->nodes.end(); ++it ) {
    if ( (*it)->gen_node_label == OUTPUT ) {
      count++;
    }
  }
  return count;
}

bool areCompatible( vector<Genome*>& vPtrGenome ) {
  typedef vector<Genome*> V;
  typedef V::iterator VI;
  if ( vPtrGenome.size() < 1 ) {
    cerr << "received an empty vector!" << endl;
    throw 1; // throw something meaningful later
  }
  VI it = vPtrGenome.begin();
  uint noOutputsFirstGenome = noOutputNodes( *it );  
  for ( ; it != vPtrGenome.end(); ++it ) {
    uint noOutputsOtherGenomes = noOutputNodes( *it );
    if ( noOutputsFirstGenome != noOutputsOtherGenomes ) {
      return false;
    }
  }
  return true;
}

/*
void populateInvertedIndex( InvertedIndex* ptrInvertedIndex, Genome* ptrGenome ) {

}

void populateIndex( InvertedIndex* ptrIndex, Genome* ptrGenome ) {

}
*/

//void aggregateGenomes( 
    //vector<Genome*>& vPtrGenome,
    //Genome** ptrDblGenome,
    //Index** ptrDblIndex,
    //InvertedIndex** ptrDblInvertedIndex
  //) {
Genome* aggregateGenomes( vector<Genome*>& vPtrGenome ) {
  using NEAT::NNode;
  using NEAT::OUTPUT;
  using NEAT::HIDDEN;
  using NEAT::Trait;
  using NEAT::Link;
  typedef vector<Genome*> VPG;
  typedef VPG::iterator VPGI;
  typedef vector<NNode*> VPN;
  typedef VPN::iterator VPNI;
  typedef vector<Trait*> VPT; 
  typedef VPT::iterator VPTI;
  typedef vector<Link*> VPL;
  typedef VPL::iterator VPLI;
  if ( vPtrGenome.size() < 1 ) {
    cerr << "You gave me an empty vector dumb ass." << endl;
    throw 1; // throw something meaningful later
  }

  // 0. check compatibility of genomes
  
  bool compatible = areCompatible( vPtrGenome ); 
  if ( !compatible ) {
    cerr << "genome files are not compatible" << endl;
    throw 1; // throw something meaningful later
  }

  // Algorithm
  // 1. {
  //  Genome NG; # new genome 
  //  foreach Genome g : Genomes
  //    foreach Node n : Nodes {
  //      nn = n
  //      if n is output node then convert to hidden node
  //      NG.insert( nn )
  //      update index and invertedIndex
  //    }
  // }

  // create indices for referring to the corresponding old node and new node 
  Index index; 
  InvertedIndex InvertedIndex;
  // create a vector of traits for creating new genome
	// create a dummy trait
	Trait* ptrTraitNew = new Trait( 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ); // note: trait_id is 1
  VPT vPtrTraitsNew; vPtrTraitsNew.clear();
	vPtrTraitsNew.push_back( ptrTraitNew );
  // create a vector of nodes for creating new genome
  VPN vPtrNodesNew; vPtrNodesNew.clear();
  int counter = 1;
  for ( VPGI itG = vPtrGenome.begin(); itG != vPtrGenome.end(); ++itG ) { 
    VPN& vNodes = (*itG)->nodes;
    for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
      // node pointed to by ptrNNodeNew will be ultimately deleted by the Genome pointed to by ptrGenomeNew
      NNode* ptrNNodeNew = new NNode( *itN, ptrTraitNew );
      ptrNNodeNew->node_id = counter;
      if ( ptrNNodeNew->gen_node_label = OUTPUT ) {
        ptrNNodeNew->gen_node_label = HIDDEN;
      }
      // insert NNode inside vPtrNodesNew which will be added to the new Genome
      vPtrNodesNew.push_back( ptrNNodeNew ); 
      // update index
      // update invertedIndex
      counter++;
    }
  }
  //Genome* ptrGenomeNew = new Genome( 1, vTraits, vNodes, vLinks );
  //Genome* ptrGenomeNew = new Genome( 1, vTraits, vNodes, vGenes );
}

