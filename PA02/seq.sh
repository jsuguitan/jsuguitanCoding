#!/bin/bash

#SBATCH -n 1
#SBATCH --output=seqOut.out
#SBATCH --time=00:10:00

srun sequence
