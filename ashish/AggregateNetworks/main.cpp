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
using std::make_pair;
// NEAT
using NEAT::Genome;
using NEAT::Gene;
using NEAT::Organism;
using NEAT::Network;
using NEAT::NNode;
// typedefs
typedef unsigned int uint;
// forced to use int here as node_id in NEAT code uses int
// note although I am storing a pointer in the Pair and using it as a key later
// you can switch to stroing genome_id instead i.e. type int
typedef pair< Genome*, int > PairPtrGenomeNodeId; // stands for Genome and Node Id
typedef pair< int, PairPtrGenomeNodeId > PairIndex;
typedef map< int, PairPtrGenomeNodeId > Index;
typedef Index::iterator IndexI;
typedef Index::const_iterator IndexCI;
typedef pair< PairPtrGenomeNodeId, NNode* > PairInvertedIndex;
typedef map< PairPtrGenomeNodeId, NNode* > InvertedIndex;
typedef InvertedIndex::iterator InvertedIndexI;
typedef InvertedIndex::const_iterator InvertedIndexCI;
// declarations
void convertToGraphVizFormat( Genome* ptrGenome );
Genome* extractGenome( const string& pathFileGenome );
bool areCompatible( vector<Genome*>& vPtrGenome );
Genome* aggregateGenomes( vector<Genome*>& vPtrGenome );
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
  
  // TESTING compatible
  areCompatible( vPtrGenome );
  
  // TESTING aggregateNetworks
  aggregateGenomes( vPtrGenome );
  
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
  typedef VPN::const_iterator VPNCI;
  typedef vector<Trait*> VPT; 
  typedef VPT::iterator VPTI;
  typedef vector<Gene*> VPGene;
  typedef VPGene::iterator VPGeneI;
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
  Index index; index.clear(); 
  InvertedIndex invertedIndex; invertedIndex.clear();
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
      if ( OUTPUT == ptrNNodeNew->gen_node_label  ) {
        ptrNNodeNew->gen_node_label = HIDDEN;
      }
      // insert NNode inside vPtrNodesNew which will be added to the new Genome
      vPtrNodesNew.push_back( ptrNNodeNew ); 
      // update index
      PairPtrGenomeNodeId pairPtrGenomeNodeId = make_pair( *itG, (*itN)->node_id );
      PairIndex pairIndex = make_pair( ptrNNodeNew->node_id, pairPtrGenomeNodeId );
      index.insert( pairIndex );
      // update invertedIndex
      PairInvertedIndex pairInvertedIndex = make_pair( pairPtrGenomeNodeId, ptrNNodeNew ); 
      invertedIndex.insert( pairInvertedIndex );
      counter++;
    }
  }

  // TESTING
  // display all nodes in vPtrNodesNew
  cout << "TESTING VPN " << endl;
  for ( VPNI it = vPtrNodesNew.begin(); it != vPtrNodesNew.end(); ++it ) {
    (*it)->print_to_file( cout );
  }

  // display index
  cout << "TESTING Index " << endl;
  for ( IndexI it = index.begin(); it != index.end(); ++it ) {
    cout << it->first << ", <" << it->second.first->genome_id << ", ";
    cout << it->second.second << ">" << endl;
  }

  // display invertedIndex
  cout << "TESTING InvertedIndex " << endl;
  for ( InvertedIndexI it = invertedIndex.begin(); it != invertedIndex.end(); ++it ) {
    cout << "<" << it->first.first->genome_id << ", " << it->first.second << ">, ";
    cout << it->second->node_id << endl;
  }

  // Algorithm
  // 2. {
  //  foreach Genome g : Genomes
  //    foreach Link l : Links
  //      query invertedIndex and get new node id for incoming node
  //      query invertedIndex and get new node id for outgoing node
  //      create new link using new node ids
  //      insert new link inside New Genome
  
  VPGene vPtrGenesNew; vPtrGenesNew.clear();
  counter = 1; // note: counter defined and used earlier
  for ( VPGI itG = vPtrGenome.begin(); itG != vPtrGenome.end(); ++itG ) { 
    VPGene& vGenes = (*itG)->genes;
    for ( VPGeneI itGene = vGenes.begin(); itGene != vGenes.end(); ++itGene ) {
      Gene& gene = **itGene;
      Link& link = *( gene.lnk );
      // get new node id for incoming node from invertedIndex
      PairInvertedIndex pairII = make_pair( (*itG), link.in_node->node_id ); 
      InvertedIndexI itII = invertedIndex.find( pairII ); 
      if ( invertedIndex.end() == itII ) {
        cerr << "Houston we have a problem :P" << endl;
        cerr << "<Genome: " << (*itG)->genome_id << ", ";
        cerr << "Node Id: " << link.in_node->node_id;
        cerr << "> being searched for not found in inverted index" << endl;
        throw 1; // throw something meaningful later
      }
      NNode* ptrNodeIn = itII->second;
      // get new node id for outgoing node from invertedIndex
      pairII = make_pair( (*itG), link.out_node->node_id ); 
      itII = invertedIndex.find( pairII ); 
      if ( invertedIndex.end() == itII ) {
        cerr << "Houston we have a problem :P" << endl;
        cerr << "<Genome: " << (*itG)->genome_id << ", ";
        cerr << "Node Id: " << link.out_node->node_id;
        cerr << "> being searched for not found in inverted index" << endl;
        throw 1; // throw something meaningful later
      }
      NNode* ptrNodeOut = itII->second;
      // create new gene
      Gene* ptrGeneNew = new Gene( *itGene, ptrTraitNew, ptrNodeIn, ptrNodeOut );
      vPtrGenesNew.push_back( ptrGeneNew );
    }
  }

  // TEST vPtrGenes
  cout << "Test vPtrGenes " << endl;
  for ( VPGeneI itGene = vPtrGenesNew.begin(); itGene != vPtrGenesNew.end(); ++itGene ) {
    (*itGene)->print_to_file( cout );
  }

  //Genome* ptrGenomeNew = new Genome( 1, vTraits, vNodes, vLinks );
  //Genome* ptrGenomeNew = new Genome( 1, vTraits, vNodes, vGenes );
  return NULL; // FIXME change to proper variable later
}

