# SPACECPP
C++ Implementation of SPACE code (self-consistent parallel algorithm for collective effects) as seen [here](https://journals.aps.org/prab/pdf/10.1103/PhysRevAccelBeams.19.024401).
## High Level Overview
The program currently does the following:
* Reads in configuration files and stores in global parameters data structure
* Generates an arbitrary number of active/passive RF cavities
* Generates initial particle distribution (from a user specified function) in each RF bucket
  * Each processor is then assigned some number of bunches in order to balance the load equally amongst all available processors
* Performs time evolution in parallel on each set of bunches (Hamiltonian and Fokker-Planck updates)
  * At the end of each turn for each bunch, the position of each particle in phase space gets written to a data directory as .csv files

The intention of the code was to study the effects of higher harmonic cavities on bunch length.
# Downloading
Just use `git clone` command with HTTPS URL (https://github.com/mshettyFE/SPACECPP.git). If you want to make your own changes (or implement the features in [TODO.md](./TODO.md)), then please fork the repository.
# Compiling
NOTE: Only tested on Linux.

This software used [MPICH](https://www.mpich.org/documentation/guides/) as the MPI implementation. Please download if your organization doesn't already have it installed.

Once MPICH is downloaded, and assuming you already have the cluster setup, you should be able to set everything up just run the following commands in the base directory of the repository (works at NSLS-II cluster):
```
module load accelerator
chmod +x setup.sh
chmod +x recompile.sh
chmod +x submit.sh
./setup.sh
```

On the off chance that the above doesn't work, the sticking point is probably that cmake cannot locate the mpicxx compiler for some reason. In this case, you need to edit `recompile.sh` such that the `-DCMAKE_CXX_COMPILER=` flag of cmake points to the correct location of your mpicxx compiler. At NSLS-II, this probably means that you forgot to run `module load accelerator` on your submit node.
## Building Your Own Cluster
If you need to build your own cluster, then the following will help you setup one on Amazon EC2: [OpenMPI tutorial](https://mpitutorial.com/tutorials/launching-an-amazon-ec2-mpi-cluster/).
## Dependencies
What the above scripts do is first compile the external dependencies that need to be built:
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) (version 0.7.0)
  * Used to parse .yaml configuration files.
* [fftw](https://www.fftw.org/) (version 3.3.10)
  * Performs FFT on wakefield forces and density distributions (since you really shouldn't be implementing your own FFT algorithm)
* [mpich](https://github.com/pmodels/mpich) (version 4.1.2)
  * For parallelization since N-body simulations are very computationally expensive.
Additional external dependencies are header file only libraries:
* [cereal](https://github.com/USCiLab/cereal) (version 1.3.2)
  * A serialization library used to make MPI play nice with C++ objects.
* [doc-test](https://github.com/doctest/doctest) (version 2.4.11)
  * A commonly used unit testing library.
* [eigen3](https://gitlab.com/libeigen/eigen/tree/master) (version 3.4.0)
  * A matrix/vector algebra library (since you REALLY shouldn't be implementing your own matrix library)
* [TCLAP](https://github.com/mirror/tclap) (version 1.4)
  * A widely utilized command line parser.

The scripts then run [cmake](https://cmake.org/download/) (minimum version 3.19), the build system used in order to make integrating all the above easier.
# Running Code
Run the following command in the `build` directory that was created in the compilation process:

```sbatch submit.sh```

 Make sure you run `module load accelerator` if you forgot to (or the equivalent on your environment. You need to be able to load in, in addition to the dependencies for the other libraries, the mpicxx compiler and cmake.).

You can modify `submit.sh` if you to want to change the input file command line parameters (default submit.sh uses default .yaml files in [configs](./configs/) directory.)

You can also modify the .yaml files in [configs](./configs/), such as increasing the number of simulation particles in [BunchParameters](./configs/BunchParameters.yaml).
 ## Checking Progress
 You can check your progress with the `squeue` command. You can cancel jobs by running `squeue`, taking note of the JOBID, and running `scancel JOBID`.
 ## Running Debug Mode
Unit tests were used to verify that modules worked as intended. In order to enable unit testing, comment out `#define DOCTEST_CONFIG_DISABLE` in `main.cpp`. You can then run `sbatch` with a single processor.
# Making Local Changes
After the first compilation, if you want to make changes, you may need to recompile.
## Modifying Configuration Files
In the [configs](./configs/) directory, if you modify the values of existing parameters in existing .yaml files, then you don't need to recompile. If you add new .yaml files or you add new parameters to existing .yaml files, then you need to update the source code, which necessitates recompiling.
## Modifying Existing C++ Source/Header Files
If you modify existing source/header files, then you only need to run `make` in the `build` directory in order to rebuild.
## Adding new C++ Source/Header Files
If you add new source files, then you will need to edit `CMakeLists.txt` as needed, then run recompile.sh in the build directory:
```
cd build
./recompile.sh
```
what this does is rerun cmake and make on the entire project.
### Editing CMakeLists.txt
When adding new source files to the project, you need to append to the `SOURCES` variable in CMakeLists.txt. New header files should automatically be included as long as they are placed in `include` directory.

# TODO
Please see [TODO.md](./TODO.md) for full list.