#!/bin/bash

#SBATCH --job-name=Search


#SBATCH --output=/home/fhe2/cs599Project/Search.out

#SBATCH --error=/home/fhe2/cs599Project/Search.err

#SBATCH --time=1:00:00  #1 hour, dependent on number of actuators and step resolutiond

#SBATCH --mem=0          ## all mem
#SBATCH -C epyc
#SBATCH --nodes=1
#SBATCH --account=cs599-spr21

#SBATCH --cpus-per-task=1
#SBATCH --exclusive

module load openmpi

# compiling does not run in job script, must run on comand line
# mpic++ main.cpp -o run  -lm -fopenmp -lssl -lcrypto -O3

# 1M chains
srun --ntasks=1 ./run 1000000 
srun --ntasks=2 ./run 1000000
srun --ntasks=4 ./run 1000000
srun --ntasks=8 ./run 1000000
srun --ntasks=12 ./run 1000000
srun --ntasks=16 ./run 1000000
srun --ntasks=20 ./run 1000000

# 100K chains
srun --ntasks=1 ./run 100000
srun --ntasks=2 ./run 100000
srun --ntasks=4 ./run 100000
srun --ntasks=8 ./run 100000
srun --ntasks=12 ./run 100000
srun --ntasks=16 ./run 100000
srun --ntasks=20 ./run 100000

# 10K chains
srun --ntasks=1 ./run 10000 
srun --ntasks=2 ./run 10000
srun --ntasks=4 ./run 10000
srun --ntasks=8 ./run 10000
srun --ntasks=12 ./run 10000
srun --ntasks=16 ./run 10000
srun --ntasks=20 ./run 10000