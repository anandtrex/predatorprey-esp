cmake_minimum_required(VERSION 2.4)

PROJECT( CreateStarterGenomes )

FILE(GLOB neat_sources *.cpp)

ADD_EXECUTABLE( CreateStarterGenomes ${neat_sources} )
