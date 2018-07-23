cmake_minimum_required (VERSION 3.5)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project (mw_parallel_programming)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -Wall -ggdb -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wall -ggdb --std=c++0x -O3")

# Directories
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB SOURCES "${SOURCES_DIR}/*.cpp")

#MPI Library include/flags
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# Main executable
add_executable(main ${SOURCES})
include_directories(${INCLUDE_DIR})

#if(MPI_COMPILE_FLAGS)
#  set_target_properties(match PROPERTIES
#    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
#endif()

#if(MPI_LINK_FLAGS)
#  set_target_properties(match PROPERTIES
#    LINK_FLAGS "${MPI_LINK_FLAGS}")
#endif()
