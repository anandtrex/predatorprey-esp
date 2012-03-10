/* 
 * CreateStarterGenome
 *
 * Author: Ashish Jain
 * Date  : March 09, 2012
 *
 */

/*!
 * NOTE: The following program has not been optimized because number of nodes
 * and genes is rather small
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

enum NodeType { NEURON = 0, SENSOR = 1 };
enum NodePlace { HIDDEN = 0, INPUT = 1, OUTPUT = 2, BIAS = 3 };

  
struct Node {
  int nodeId;
  int traitNum;
  NodeType type;
  NodePlace place;
};

Node factoryNode( int & nodeId, NodePlace place ) {
  int traitNum = 0;
  Node node;
  node.nodeId = ++nodeId;
  node.traitNum = traitNum; 
  node.place = place;
  switch( place ) {
    case OUTPUT:
    case HIDDEN:
      node.type = NEURON;
      break;
    default:
      node.type = SENSOR;
  }
  return node;
}

struct Gene {
  int traitNum;
  int nodeIdIn;
  int nodeIdOut;
  double weight;
  int recur;
  int innovationNum;
  int mutationNum;
  int enable;
};

Gene factoryGene( int & traitNum, int & innovationNum, Node nodeIn, Node nodeOut ) {
  Gene gene;
  gene.traitNum = traitNum + 1;
  traitNum = ( traitNum + 1 ) % 3;
  gene.nodeIdIn = nodeIn.nodeId;
  gene.nodeIdOut = nodeOut.nodeId;
  gene.weight = 0.0;
  gene.recur = 0;
  gene.innovationNum = ++innovationNum;
  gene.mutationNum = 0;
  gene.enable = 1;
  return gene;
}

struct Nodes {
  vector<Node> vBias;
  vector<Node> vInput;
  vector<Node> vOutput;
};

struct Genome {
  Nodes nodes;
  vector<Gene> vGene;
};

Nodes createNodesMonolithic( int noOtherAgents ) {
  int nodeId = 0;
  Nodes nodes;
  nodes.vBias.push_back( factoryNode( nodeId, BIAS ) );
  for ( int i = 0; i < noOtherAgents; i++ ) {
    for ( int j = 0; j < 3; j++ ) {
      nodes.vInput.push_back( factoryNode( nodeId, INPUT ) );
    }
  }
  for ( int i = 0; i < 5; i++ ) {
    nodes.vOutput.push_back( factoryNode( nodeId, OUTPUT ) );
  }
  return nodes;
}

Nodes createNodesSelection( int noOtherAgents ) {
  int nodeId = 0;
  Nodes nodes;
  nodes.vBias.push_back( factoryNode( nodeId, BIAS ) );
  for ( int i = 0; i < noOtherAgents; i++ ) {
    for ( int j = 0; j < 3; j++ ) {
      nodes.vInput.push_back( factoryNode( nodeId, INPUT ) );
    }
  }
  for ( int i = 0; i < noOtherAgents; i++ ) {
    nodes.vOutput.push_back( factoryNode( nodeId, OUTPUT ) );
  }
  return nodes;
}

vector<Gene> createGenes( Nodes nodes ) {
  int traitNum = 0;
  int innovationNum = 0;
  typedef vector<Node>::iterator Iterator;
  vector<Gene> vGene;
  for ( Iterator it = nodes.vBias.begin(); it != nodes.vBias.end(); ++it ) {
    for ( Iterator jt = nodes.vOutput.begin(); jt != nodes.vOutput.end(); ++jt ) {
      vGene.push_back( factoryGene( traitNum, innovationNum, *it, *jt ) );
    }
  }
  for ( Iterator it = nodes.vInput.begin(); it != nodes.vInput.end(); ++it ) {
    for ( Iterator jt = nodes.vOutput.begin(); jt != nodes.vOutput.end(); ++jt ) {
      vGene.push_back( factoryGene( traitNum, innovationNum, *it, *jt ) );
    }
  }
  return vGene;
}

void write( ofstream& fout, const Node& node ) {
  fout << "node " << node.nodeId << " " << node.traitNum << " " << node.type << " " << node.place << endl;
}

void write( ofstream& fout, const Gene& gene ) {
  fout << "gene " << gene.traitNum << " " << gene.nodeIdIn << " " << gene.nodeIdOut << " ";
  fout << gene.weight << " " << gene.recur << " " << gene.innovationNum << " " << gene.mutationNum << " ";
  fout << gene.enable << endl;
}

void write( string pathFileOut, Genome genome ) {
  typedef vector<Node>::iterator IteratorNode;
  typedef vector<Gene>::iterator IteratorGene;
  ofstream fout( pathFileOut.c_str() );
  fout << "genomestart 1" << endl;
  fout << "trait 1 0.1 0 0 0 0 0 0 0" << endl;
  fout << "trait 2 0.1 0 0 0 0 0 0 0" << endl;
  fout << "trait 3 0.3 0 0 0 0 0 0 0" << endl;
  // nodes
  for ( IteratorNode it = genome.nodes.vBias.begin(); it != genome.nodes.vBias.end(); ++it ) {
    write( fout, *it );    
  }
  for ( IteratorNode it = genome.nodes.vInput.begin(); it != genome.nodes.vInput.end(); ++it ) {
    write( fout, *it );
  }
  for ( IteratorNode it = genome.nodes.vOutput.begin(); it != genome.nodes.vOutput.end(); ++it ) {
    write( fout, *it );
  }
  // genes
  for ( IteratorGene it = genome.vGene.begin(); it != genome.vGene.end(); ++it ) {
    write( fout, *it );
  }
  fout << "genomeend 1" << endl;
  fout.close();
}

int main( int argc, char* argv[] ) {
  if ( argc < 3 ) {
    cerr << "Expecting arguments ... " << endl;
    cerr << "1. Number of other agents" << endl;
    cerr << "2. Path of output file for monolithic starter genome" << endl;
  }
  int noOtherAgents;
  istringstream sin( argv[1] );
  sin >> noOtherAgents;
  string pathFileOutMonolithic( string( argv[2] ) + "Monolithic" + "NumOtherAgents" + argv[1] );
  string pathFileOutSelection( string( argv[2] ) + "Selection" + "NumOtherAgents" + argv[1] );
  // monolithic network
  Genome genomeMonolithic;
  genomeMonolithic.nodes = createNodesMonolithic( noOtherAgents );
  genomeMonolithic.vGene = createGenes( genomeMonolithic.nodes );
  write( pathFileOutMonolithic, genomeMonolithic );
  // selection network
  Genome genomeSelection;
  genomeSelection.nodes = createNodesSelection( noOtherAgents );
  genomeSelection.vGene = createGenes( genomeSelection.nodes );
  write( pathFileOutSelection, genomeSelection );
  return 0;
}

