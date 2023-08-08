#!/bin/bash
# Sets up environment variables to be used with sbatch
# To run, input the command `sbatch submit.sh`
#SBATCH --job-name=SPACE
#SBATCH --error=%x.%J.err 
#SBATCH --output=%x.%J.out
#SBATCH --ntasks=66
#SBATCH --time=0-10:00:00
#SBATCH --qos=normal

srun ./SPACE -d
