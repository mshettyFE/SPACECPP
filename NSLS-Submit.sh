#!/bin/bash
#SBATCH --mem=4G
#SBATCH --job-name=NSLS-SPACECPP
#SBATCH --error=job.%J.err
#SBATCH --output=job.%J.out
#SBATCH --ntasks=66
#SBATCH --qos=long
#SBATCH --time=0-10:00:00

srun ./SPACE ../HHCParameters.yaml 0
