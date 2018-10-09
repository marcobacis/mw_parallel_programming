# Middleware OpenMP Project: soccer ball possession computation

OpenMP project for the "Middleware Technologies for Distributed Systems" course at Polimi, consisting of an application to compute the ball possession time for a soccer match.

## Abstract

The goal of the project is to develop a system to provide analytics over high velocity sensor data originating from a soccer game. The goal of the analysis is to report and update the ball possession time for each player and team periodically during the game. The data used comes from the DEBS2013 challenge, in which a number of wireless sensors embedded in the shoes and a ball were used during a soccer match, spanning the whole duration of the game The main requirements is to use OpenMP and/or MPI to compute the real-time statistics.

## How to build and run

This project requires a version of GCC that supports OpenMP 3.0 and the C++11 standard. The project was tested on GCC 7.
It is advised to use GCC even on Apple platforms because CMake does not completely support configuring OpenMP on non-GCC compilers.

### Step 1: configure

First create a build directory, then move inside it and configure the project through CMake:

```
cmake ${project_root} -DCMAKE_BUILD_TYPE=Release
```

where `${project_root}` stands for the name of the directory where `CMakeLists.txt` is found.

The project will be automatically configured with OpenMP enabled. To disable OpenMP, also specify the command line option `-DUSE_OpenMP=OFF`.

If CMake fails to configure OpenMP, try to manually specify a compatible compiler using `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER`.

### Step 2: build

Once the project has been built, compile it. The exact command depends if you specified a CMake generator other than the default. If you used the default generator (for example if you just copy-pasted the command line given above) just run:

```
make
```

### Step 3: run

The built executable is named `ball-possession` and is located in the build directory. The two parameters K (ball possession distance) and T (time step duration in seconds) are specified on the command line. 

The data directory path must be specified as the third parameter. For the program to work, you must add the `full-game.csv` file to the data directory (which is not included because it is approximately 3.96 GiB in size), which can be found  at [http://www2.iis.fraunhofer.de/sports-analytics/full-game.gz](http://www2.iis.fraunhofer.de/sports-analytics/full-game.gz).

For example, if the data directory is located in the same directory of the executable and K and T are respectively 1 and 60:

```
./ball-possession 1 60 ./data/
```

(note: the final slash in the data directory path is mandatory)

