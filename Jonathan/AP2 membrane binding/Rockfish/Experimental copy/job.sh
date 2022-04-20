#!/bin/bash
#SBATCH --job-name=experiment
#SBATCH --time=1:0:0
#SBATCH --partition=defq
#SBATCH --nodes=1
# number of tasks (processes) per node
#SBATCH --ntasks-per-node=1

./nerdss -f parms.inp > OUT-log 

