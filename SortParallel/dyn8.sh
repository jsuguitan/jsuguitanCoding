#!/bin/bash

#SBATCH -n 8
#SBATCH --tasks-per-node=8
#SBATCH --time=00:10:00
#SBATCH --output=dynamicTiming.out

srun dynamic int1500000 8
