#!/bin/bash -l

#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

#SBATCH --time=00:01:00


#SBATCH --job-name=test_submission
#SBATCH --output=openmp_test.out
#SBATCH --error=openmp_test.err

#SBATCH --partition=intq

export OMP_NUM_THREADS=1


### openmp executable
./BS.exec
