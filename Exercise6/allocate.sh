# load mpi modules
module load gcc/6.3.0

module load intel/mpi/64/2017/6.256

# allocates interactive resources
salloc -n2 --nodelist=gpu01 --partition=intq srun --pty bash