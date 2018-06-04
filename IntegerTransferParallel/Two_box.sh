#!/bin/bash

#SBATCH -N 2
#SBATCH --output=two_box.out
#SBATCH --time=00:30:00

srun pa01
