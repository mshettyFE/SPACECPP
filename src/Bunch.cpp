#include "Bunch.h"
#include "Particle.h"
#include <vector>
Bunch::Bunch(int nparticles){
    if(nparticles <=0){
      std::runtime_error("Bunch instantiated with fewer than less than 0");
    }
    for(int i=0; i<nparticles; ++i){
      sim_parts.push_back(Particle());
    }
}