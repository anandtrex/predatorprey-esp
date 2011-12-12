CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
SET ( project_name "PreyHunterAP" ) 

FIND_PACKAGE(GLUT)

# searches for all .cpp and .h files in the current directory and add them 
# to the current project
FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# create the project
ADD_EXECUTABLE(${project_name} ${folder_source} ${folder_header})
 
# add include directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
 
# link opencv libraries 
TARGET_LINK_LIBRARIES(${project_name} ${GLUT_LIBRARIES})

