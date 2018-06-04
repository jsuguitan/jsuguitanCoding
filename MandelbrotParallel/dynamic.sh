#!/bin/bash

#SBATCH -n 24
#SBATCH --time=00:10:00
#SBATCH --output=dynamicTiming.out

srun dynamic 
