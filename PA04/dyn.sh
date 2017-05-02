#!/bin/bash

#SBATCH -n 0
#SBATCH --tasks-per-node=8
#SBATCH --time=00:10:00
#SBATCH --output=dynamicT1.out

srun dynamic int
