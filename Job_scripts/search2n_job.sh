#!/bin/bash

#SBATCH --job-name=Search2n


#SBATCH --output=/home/fhe2/cs599Project/Search2n.out

#SBATCH --error=/home/fhe2/cs599Project/Search2n.err

#SBATCH --time=30:00  

#SBATCH --mem=0          ## all mem
#SBATCH -C epyc
#SBATCH --nodes=1
#SBATCH --account=cs599-spr21

#SBATCH --cpus-per-task=1
#SBATCH --exclusive

module load openmpi

# mpic++ main.cpp -o run  -lm -fopenmp -lssl -lcrypto -O3

srun --ntasks=2 ./run 100000