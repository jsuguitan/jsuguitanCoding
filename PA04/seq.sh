#!/bin/bash

#SBATCH -n 1
#SBATCH --tasks-per-node=8
#SBATCH --output=seqOut.out
#SBATCH --time=00:10:00

srun sequence matrixA matrixB
