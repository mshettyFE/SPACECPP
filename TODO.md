# TODO
While the main functionality of SPACECPP is done, such as:
* Arbitrary initialization of Cavity data
* Distributed generation of initial particle density
* Distributed computation of symplectic integrator for Hamiltonian and Fokker-Planck dynamics

work still needs to be done in order to implement the wakefield calculation. This includes the following:
* Cubic spline interplolation of [Wakefield](./include/Wakefield.h) function (Eigen has spline implementation which can be used)
  * Resampling of Wakefield to match spacing of [Density](./include/Density.h)
* Using FFTW to calculate short-range wakefield with gaussian smoothing (see [here](https://journals.aps.org/prab/pdf/10.1103/PhysRevAccelBeams.19.024401))
* Enable arbitrary calculation of Wakefield function via higher order finite central difference schemes
* Create Entry class to store n-th moments along each coordinate for each particle on a per bunch basis at the end of each time step
* Create History class (implemented as a fixed length queue of Entry objects) in order to enable calculation of long range wakefield

Additional work can also be done to automatically set optimal conditions on each of the cavities, as well as examine other types of cavities with less stringent assumptions (ie. no beam loading for active case and non-uniform filling for passive case)