#!/bin/bash
# run for x generations on prey network
# PREY
pathBasePrey="/home/ashish/Data/courses/cs394n/project/actual/cs394n-project/ashish/Prey/build"
pathFileParametersNEATPrey="$pathBasePrey/predatorpreyhunter.ne" 
pathExecPrey="$pathBasePrey/neat"
$pathExecPrey $pathFileParametersNEATPrey $1 $1
# run for x generations on hunter network"
# HUNTER
pathBaseHunter="/home/ashish/Data/courses/cs394n/project/actual/cs394n-project/ashish/Hunter/build"
pathFileParametersNEATHunter="$pathBaseHunter/predatorpreyhunter.ne" 
pathExecHunter="$pathBaseHunter/neat"
$pathExecHunter $pathFileParametersNEATHunter $1 $1
# combine the two networks
pathCWD="/home/ashish/Data/courses/cs394n/project/actual/cs394n-project/ashish/scripts" 
pathGenome1="$pathCWD/GenerationChampPrey$1.txt"
pathGenome2="$pathCWD/GenerationChampHunter$1.txt"
pathBaseAggregate="/home/ashish/Data/courses/cs394n/project/actual/cs394n-project/ashish/AggregateNetworks/build"
pathExecAggregate="$pathBaseAggregate/AggregateNetworks" 
ls -l $pathGenome1
ls -l $pathGenome2
$pathExecAggregate $pathGenome1 $pathGenome2
# run for 100 generations on preyhunter network
# PREY HUNTER
pathBasePreyHunter="/home/ashish/Data/courses/cs394n/project/actual/cs394n-project/ashish/PreyHunter/build"
pathFileParametersNEATPreyHunter="$pathBasePreyHunter/predatorpreyhunter.ne" 
pathFileStarterGenomePreyHunter="$pathCWD/CombinedChamp.txt" 
pathExecPreyHunter="$pathBasePreyHunter/neat"
echo "RUNNING PREY HUNTER"
ls -l $pathFileParametersNEATPreyHunter
echo "ARGUMENTS"
echo $pathFileParametersNEATPreyHunter
echo $pathFileStarterGenomePreyHunter
echo $1
$pathExecPreyHunter $pathFileParametersNEATPreyHunter $pathFileStarterGenomePreyHunter $1
#./neat predatorpreyhunter.ne singlepreyhunter_startgenes $1 
