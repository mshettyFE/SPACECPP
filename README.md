# SPACECPP
C++ Implementation of SPACE code (self-consistent parallel algorithm for collective effects) as seen [here](https://journals.aps.org/prab/pdf/10.1103/PhysRevAccelBeams.19.024401).
# Downloading
Just use `git clone` command with HTTPS URL
# Compiling
NOTE: Only tested on Linux.

While MPI integration is currently not completed, it will eventually be necessary due to the intense computational demand of N-body simulations. As such, while this will compile without MPI, it is recommended to download [MPICH](https://www.mpich.org/documentation/guides/) if your organization doesn't already have it installed.

Once MPICH is downloaded, you need to specify the location of the mpiicxx compiler. For NSLS-II, this is located at
`
/nsls2/software/ap/mpich/3.3.2-gcc-9.3.0/bin/mpicxx
`

On Unix-based systems, you should be able to locate your compiler with the command `which mpicxx`.

To set up, just run the following commands in the base directory of the repository
```
chmod +x setup.sh
./setup.sh
```

what the above scripts does is first compile the dependencies: [yaml-cpp](https://github.com/jbeder/yaml-cpp) and [fftw](https://www.fftw.org/). It then creates a build directory for the project and runs cmake and make to compile SPACECPP.
# Making Local Changes
After the first compilation, if you want to make changes, you may need to recompile.
## Modifying Configuration Files
In the `configs` directory, if you modify existing .yaml files, then you don't need to recompile. If you add new .yaml files, then you need to update the source code, which necessitates recompiling.
## Modifying Existing C++ Source/Header Files
If you modify existing source/header files, then you only need to run `make` in the `build` directory in order to rebuild.
## Adding new C++ Source/Header Files
If you add new source files, then you will need to edit `CMakeLists.txt` as needed, then run the commands
```
cmake ..
make
```
in the `build` directory.
### Editing CMakeLists.txt
When adding new source files to the project, you need to append to the `SOURCES` variable in CMakeLists.txt.