cmake_minimum_required (VERSION 3.10)

project(mw_parallel_programming)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ggdb -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb --std=c++0x -O3")

# Directories
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB SOURCES "${SOURCES_DIR}/*.cpp")

#MPI Library include/flags
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

#OpenMP library
if(APPLE)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C_FLAGS "-Xclang -fopenmp")
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX_FLAGS "-Xclang -fopenmp")
    endif()
endif()

find_package(OpenMP REQUIRED)

if(NOT TARGET OpenMP::OpenMP_CXX)
    add_library(OpenMP_TARGET INTERFACE)
    add_library(OpenMP::OpenMP_CXX ALIAS OpenMP_TARGET)
    target_compile_options(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
    find_package(Threads REQUIRED)
    target_link_libraries(OpenMP_TARGET INTERFACE Threads::Threads)
    target_link_libraries(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
endif()

# Main executable
add_executable(main ${SOURCES})
include_directories(${INCLUDE_DIR})

target_link_libraries(main OpenMP::OpenMP_CXX  ${MPI_LIBRARIES})
