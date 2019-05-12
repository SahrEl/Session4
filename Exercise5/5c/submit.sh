#!/bin/bash

# a sample job submission script to submit an MPI job

# please change the --partition option if you want to use another partition


#SBATCH --job-name=ps4_exercise5a
#SBATCH --output=output/ps4_exercise5c_8.out
#SBATCH --error=bs-mpi.err
#SBATCH --mail-type=BEGIN,END,DAIL

# this job requests 2 cores. Cores can be selected from various nodes.

#SBATCH --ntasks=8


# there are many partitions on Alphacruncher and it is important to specify which

# partition you want to run your job on. Not having the following option, the

#SBATCH --partition=intq

module load gcc/6.3.0

module load intel/mpi/64/2017/6.256



# Run the process with mpirun. Notice -n is not required. mpirun will

# automatically figure out how many processes to run from the slurm options

mpirun ./scatter.exec
