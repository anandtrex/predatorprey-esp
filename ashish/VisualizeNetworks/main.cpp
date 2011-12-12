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
using std::ofstream;
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
void neat2dot( string pathFileDot, Genome* ptrGenome );
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
  //Organism* ptrOrganism = new Organism( 0, ptrGenome, 1 );
  //cout << "Organism" << endl;
  //ptrOrganism->write_to_file( cout );
  //Network* ptrNetwork = ptrOrganism->net;
  //cout << "Network node count: " << ptrNetwork->nodecount() << endl;
  //string pathFileLinks = pathGenome + ".links.txt";
  //ptrNetwork->print_links_tofile( const_cast<char*> ( pathFileLinks.c_str() ) );
  string pathFileDot = pathGenome + ".dot";
  //ptrNetwork->writeToDotFile( const_cast<char*> ( pathFileDot.c_str() ) );

  cout << "NEAT2DOT" << endl;
  neat2dot( pathFileDot, ptrGenome );
  //delete ptrOrganism; //  genome deleted automatically by organism object. No need for delete ptrGenome;
  delete ptrGenome;
  return 0;
}

// other function definitions
void neat2dot( string pathFileDot, Genome* ptrGenome ) {
	// typedefs
  using NEAT::NNode;
  using NEAT::Trait;
  using NEAT::Link;
  using NEAT::OUTPUT;
  using NEAT::HIDDEN;
  using NEAT::INPUT;
  using NEAT::BIAS;
  typedef vector<NNode*> VPN;
  typedef VPN::iterator VPNI;
  typedef VPN::const_iterator VPNCI;
  typedef vector<Trait*> VPT; 
  typedef VPT::iterator VPTI;
  typedef vector<Gene*> VPG;
  typedef VPG::iterator VPGI;
  ofstream fout( pathFileDot.c_str() );
  if ( !fout.is_open() ) {
    cerr << "Unable to open file " << pathFileDot << " for writing!" << endl;
    throw 1; // throw something meaningful later
  }
	// simply iterate through all nodes of the genome for now
  string indentation = "\t";
  string terminate = ";\n";
	fout << "digraph G {\n";
  fout << indentation << "rankdir=BT" << terminate;
  fout << indentation << "node [shape=circle, weight=bold, fontsize=10, color=black, fontcolor=black]" << terminate;
  fout << indentation << "edge [color=black, fontsize=9, fontcolor=black, decorate = true ]" << terminate;
  VPN& vNodes = ptrGenome->nodes;
  // output nodes
  fout << indentation << "node [ shape = ellipse ]" << terminate;
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( OUTPUT == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << indentation << nodeId << " [ label=\"O" << nodeId << "\" ]" << terminate; 
    }
  }
  // input nodes
  fout << indentation << "node [ shape = box ]" << terminate;
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( INPUT == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << indentation << nodeId << " [ label=\"I" << nodeId << "\" ]" << terminate; 
    }
  }
  // bias nodes
  fout << indentation << "node [ shape = hexagon ]" << terminate;
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( BIAS == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << indentation << nodeId << " [ label=\"B" << nodeId << "\" ]" << terminate; 
    }
  }
  // hidden nodes
  fout << indentation << "node [ shape = circle ]" << terminate;
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( HIDDEN == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << indentation << nodeId << " [ label=\"H" << nodeId << "\" ]" << terminate; 
    }
  }
  // only for output nodes
  fout << indentation << "{ rank=sink; "; 
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( OUTPUT == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << nodeId << "; "; 
    }
  }
  fout << "}" << endl;
  // only for input nodes
  fout << indentation << "{ rank=source; "; 
  for ( VPNI itN = vNodes.begin(); itN != vNodes.end(); ++itN ) {
    if ( INPUT == (*itN)->gen_node_label || BIAS == (*itN)->gen_node_label ) {
      int nodeId = (*itN)->node_id;
      fout << nodeId << "; "; 
    }
  }
  fout << "}" << endl;
  // links
  VPG& vGenes = ptrGenome->genes;
  for ( VPGI itG = vGenes.begin(); itG != vGenes.end(); ++itG ) {
    Gene& gene = **itG;
    Link& link = *( gene.lnk );
    int nodeIdIn, nodeIdOut;
    nodeIdIn = link.in_node->node_id;
    nodeIdOut = link.out_node->node_id;
    fout << indentation << nodeIdIn << " -> " << nodeIdOut; 
    fout << " [ label = " << link.weight << " ]" << terminate;
  }
  fout << "}\n";
  fout.close();
}

