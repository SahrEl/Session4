/**
 Programming SS 2019 - Problem Set 4
 Exercise 5a
 Authors: Elena Pfefferlé, Pascal Schenk, Álvaro Morales
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // to get the quantity of processes available
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Hard coded double precision value
    double bcast = 4.5;
    /* broadcast the value of data of rank 0 to all ranks */
     if (rank == 0) {
        // If we are the root process
        int i;
        for (i = 1; i < world_size; i++)
        {
            // send data to to process i
            MPI_Send(&bcast, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
      } else {
        // if not root process, receives data from process 0
        MPI_Recv(&bcast, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
         printf("I am rank %i and the value is %f\n", rank, bcast);
     }

    // Waits till all processes finished
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
