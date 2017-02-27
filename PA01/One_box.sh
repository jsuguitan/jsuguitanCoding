#!/bin/bash

#SBATCH -n 2
#SBATCH --output=one_box.out
#SBATCH --time=00:30:00

srun pa01
