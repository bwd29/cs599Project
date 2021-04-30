#!/bin/bash

#SBATCH --job-name=BF


#SBATCH --output=/home/fhe2/cs599Project/BF.out

#SBATCH --error=/home/fhe2/cs599Project/BF.err

#SBATCH --time=30:00  

#SBATCH --mem=4000          ## 4G
#SBATCH -C epyc
#SBATCH --nodes=1
#SBATCH --account=cs599-spr21

#SBATCH --cpus-per-task=1
#SBATCH --exclusive

module load openmpi

# mpic++ bruteForce.cpp -o test -lm -fopenmp -lssl -lcrypto -O3

# srun --ntasks=1 ./test
srun --ntasks=2 ./test 
# srun --ntasks=4 ./test 
# srun --ntasks=8 ./test 
# srun --ntasks=12 ./test
# srun --ntasks=16 ./test
# srun --ntasks=20 ./test 

