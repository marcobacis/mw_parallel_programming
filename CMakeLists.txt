cmake_minimum_required (VERSION 3.5)

set(CMAKE_C_COMPILER "/opt/local/bin/gcc")
set(CMAKE_CXX_COMPILER "/opt/local/bin/g++")

project(mw_parallel_programming)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -Wall -ggdb -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wall -ggdb --std=c++0x -O3")

# Directories
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB SOURCES "${SOURCES_DIR}/*.cpp")

#MPI Library include/flags
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

find_package(OpenMP REQUIRED)

# Main executable
add_executable(main ${SOURCES})
include_directories(${INCLUDE_DIR})

target_link_libraries(main PUBLIC stdc++fs PRIVATE OpenMP::OpenMP_CXX)

if(MPI_COMPILE_FLAGS)
  set_target_properties(main PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(main PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
