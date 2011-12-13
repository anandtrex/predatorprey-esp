#include "Domain.h"
using namespace PredatorPreyHunter;
int main() {
  Domain domain( 200, 100, 100 );  
  domain.run( "AgentPositions.txt" );
  return 0;
}

