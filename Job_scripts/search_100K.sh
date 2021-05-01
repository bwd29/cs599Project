#!/bin/bash

#SBATCH --job-name=Search100K


#SBATCH --output=/home/fhe2/cs599Project/Search100K.out

#SBATCH --error=/home/fhe2/cs599Project/Search100K.err

#SBATCH --time=30:00  

#SBATCH --mem=0          ## all mem
#SBATCH -C epyc
#SBATCH --nodes=1
#SBATCH --account=cs599-spr21

#SBATCH --cpus-per-task=1
#SBATCH --exclusive

module load openmpi

# mpic++ main.cpp -o run  -lm -fopenmp -lssl -lcrypto -O3

srun --ntasks=1 ./run 1000000
srun --ntasks=2 ./run 1000000
srun --ntasks=4 ./run 1000000
srun --ntasks=8 ./run 1000000
srun --ntasks=12 ./run 1000000
srun --ntasks=16 ./run 1000000
srun --ntasks=20 ./run 1000000