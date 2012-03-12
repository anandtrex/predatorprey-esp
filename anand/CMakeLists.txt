cmake_minimum_required (VERSION 2.8)
project (ESP_predprey)

find_package(GLUT)
find_package(OpenGL)
find_package(X11)
find_package(Doxygen)
if(DOXYGEN_FOUND)
 configure_file(${CMAKE_SOURCE_DIR}/doxy_config ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
 add_custom_target(doc
 ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
 COMMENT "Generating API documentation with Doxygen" VERBATIM
 )
endif(DOXYGEN_FOUND)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${X11_LIBRARY} m)
set(CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_BINARY_DIR}/glog/lib -L${CMAKE_BINARY_DIR}/libconfig/lib -L${CMAKE_BINARY_DIR}/gflags/lib")

include(ExternalProject)
if(NOT EXISTS ${CMAKE_BINARY_DIR}/glog)
ExternalProject_Add(
	gflags-2.0
	BUILD_IN_SOURCE 1
	URL http://gflags.googlecode.com/files/gflags-2.0.tar.gz
	CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${CMAKE_BINARY_DIR}/gflags
	BUILD_COMMAND make
)
ExternalProject_Add(
	glog-0.3.2
	DEPENDS gflags-2.0
	URL http://google-glog.googlecode.com/files/glog-0.3.2.tar.gz
	CONFIGURE_COMMAND "CXXFLAGS=-I${CMAKE_BINARY_DIR}/gflags/include" <SOURCE_DIR>/configure --prefix=${CMAKE_BINARY_DIR}/glog 
	BUILD_COMMAND make
)
endif()
if(NOT EXISTS ${CMAKE_BINARY_DIR}/libconfig)
ExternalProject_Add(
	libconfig-1.4.8
	BUILD_IN_SOURCE 1
	URL http://www.hyperrealm.com/libconfig/libconfig-1.4.8.tar.gz
	CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${CMAKE_BINARY_DIR}/libconfig
	BUILD_COMMAND make
)
endif()

include_directories(/usr/include/x86_64-linux-gnu) # for clang

include_directories(${CMAKE_BINARY_DIR}/glog/include)
include_directories(${CMAKE_BINARY_DIR}/libconfig/include)
include_directories(${CMAKE_BINARY_DIR}/gflags/include)

#file(GLOB SRCS "*.cpp")
set(SRCS
main.cpp 
common.cpp
Domain/Agent.cpp 
Domain/DomainOne.cpp
Domain/DomainTotal.cpp
Domain/PredatorEsp.cpp
Domain/Prey.cpp
Domain/GridWorld.cpp
Domain/Hunter.cpp
Domain/Visualizer.cpp
Esp/FeedForwardNetwork.cpp
Esp/NetworkContainerEsp.cpp
Esp/NetworkContainerCombiner.cpp
Esp/NetworkContainerSelection.cpp
Esp/Network.cpp
Esp/Neuron.cpp
Esp/SubPop.cpp
Experiment/EspExperiment.cpp
Experiment/EspExperimentSubtask.cpp
)

add_executable(esp-predprey ${SRCS})
#target_link_libraries(esp-predprey ${CORELIBS} ${CMAKE_BINARY_DIR}/glog/lib/glog.so ${CMAKE_BINARY_DIR}/libconfig/lib/libconfig++.so)
target_link_libraries(esp-predprey ${CORELIBS} gflags glog config++)

ADD_DEPENDENCIES(esp-predprey gflags-2.0)
ADD_DEPENDENCIES(esp-predprey glog-0.3.2)
ADD_DEPENDENCIES(esp-predprey libconfig-1.4.8)

configure_file(${CMAKE_SOURCE_DIR}/esp_predprey.cfg
    ${CMAKE_BINARY_DIR}/esp_predprey.cfg COPYONLY)