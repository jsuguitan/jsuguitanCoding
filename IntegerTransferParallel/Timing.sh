#!/bin/bash

#SBATCH -N 2
#SBATCH --output=timing.out
#SBATCH --time=00:30:00

srun pa01p3
