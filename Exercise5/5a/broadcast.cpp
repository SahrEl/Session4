#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     
    int world_size;
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    double bcast = 4.5;

    /* broadcast the value of data of rank 0 to all ranks */
     if (rank == 0) {
        // If we are the root process, send our data to everyone
        int i;
        for (i = 1; i < world_size; i++) 
        {
            MPI_Send(&bcast, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
      } else {
        // If we are a receiver process, receive the data from the root
        MPI_Recv(&bcast, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
         printf("I am rank %i and the value is %f\n", rank, bcast);      
     }

    // Waits till all processes finished
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
