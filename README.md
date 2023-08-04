# SPACECPP
C++ Implementation of SPACE code (self-consistent parallel algorithm for collective effects) as seen [here](https://journals.aps.org/prab/pdf/10.1103/PhysRevAccelBeams.19.024401).
# Downloading
Just use `git clone` command with HTTPS URL
# Compiling
NOTE: Only tested on Linux.

This software used [MPICH](https://www.mpich.org/documentation/guides/) as the MPI implementation. Please download if your organization doesn't already have it installed.

Once MPICH is downloaded, you should be able to set everything up just run the following commands in the base directory of the repository:
```
chmod +x setup.sh
chmod +x recompile.sh
./setup.sh
```

what the above scripts does is first compile the dependencies: [yaml-cpp](https://github.com/jbeder/yaml-cpp) and [fftw](https://www.fftw.org/) (additional dependencies like [doc-test](https://github.com/doctest/doctest) and [cereal](https://github.com/USCiLab/cereal) are header only, and thus don't need to be seperately built). It then creates a build directory for the project and runs cmake and make to compile SPACECPP.
# Making Local Changes
After the first compilation, if you want to make changes, you may need to recompile.
## Modifying Configuration Files
In the `configs` directory, if you modify the values of existing parameters in existing .yaml files, then you don't need to recompile. If you add new .yaml files or you add new parameters to existing .yaml files, then you need to update the source code, which necessitates recompiling.
## Modifying Existing C++ Source/Header Files
If you modify existing source/header files, then you only need to run `make` in the `build` directory in order to rebuild.
## Adding new C++ Source/Header Files
If you add new source files, then you will need to edit `CMakeLists.txt` as needed, then run recompile.sh in the build directory:
```
cd build
./recompile.sh
```
### Editing CMakeLists.txt
When adding new source files to the project, you need to append to the `SOURCES` variable in CMakeLists.txt. New header files should automatically be included as long as they are placed in `include` directory.