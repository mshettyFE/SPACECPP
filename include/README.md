Header files used in SPACECPP.
* cereal: see [here](https://uscilab.github.io/cereal/) for documentation and usage
* Eigen: see [here](https://eigen.tuxfamily.org/index.php?title=Main_Page) for documentation and usage
* tclap: see [here](https://tclap.sourceforge.net/) for documentation and usage
* [ActiveCavity.h](./ActiveCavity.h): Derived class of [Cavity.h](./Cavity.h) which implements active cavity voltage (assumes beam loading compensation)
* [Bunch.h](./Bunch.h): Effectively a vector of particles from [Particle.h](./Particle.h) where you have additional methods that can act on the set of particles (such as moment calculations)
* [Cavity.h](./Cavity.h): Abstract base class (ABC) for [Active](./ActiveCavity.h) and [Passive](./PassiveCavity.h)
* [Constants.h](./Constants.h): Group of constants/enums used elsewhere.
* [Density.h](./Density.h): Turns particle data of Bunch along one Coordinate into a density histogram. Used in Wakefield Calculation.
* [doctest.h](./doctest.h): Unit testing framework.
* [FileParser.h](./FileParser.h): Parser the various config files.
* [Guassian.h](./Gaussian.h): Derived class of [MathFunction](./MathFunction.h). Used to model Gaussian.
* [MathFunction.h](./MathFunction.h): ABC to model initial distributions
* [MPIHelper.h](./MPIHelper.h): Group of helper functions to aid with MPI integration
* [Parameters.h](./Parameters.h): Wrapper around unordered_map that allows on to store ints and doubles as strings. Takes in either FORTRAN or C++ style
* [Particle.h](./Particle.h): Effectively a struct that encapsulates a particle's position in 3D phase spaces
* [PassiveCavity.h](./PassiveCavity.h): Derived class of [Cavity.h](./Cavity.h) which implements passive cavity voltage (assumes uniform filling, which is a good enough approximation)
* [ProbDist.h](./ProbDist.h): Turns [MathFunction](./MathFunction.h) into a valid probability density (ie calculates normalization).
* [Quartic.h](./Quartic.h): Models function of the form $exp(-ax^4)$
* [TimeEvolution.h](./TimeEvolution.h): Bundles cavity data and bunch data as one to perform time evolution.
* [ValidateInput.h](./ValidateInput.h): Validates parameters in configuration files (ensures they are compliant with either Fortran or C++ formats)
* [Wakefield.h](./Wakefield.h): Models wakefield from .csv file.

See [src](../src/) for implementation of non ABCs.