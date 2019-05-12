#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank==0) {
        printf("enter a value:\n");
        fflush(stdout);
        scanf ("%d",&data);
    }

    /* broadcast the value of data of rank 0 to all ranks */
    if (rank == 0) {
          MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else if (rank != 0) {
        MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
         printf("I am rank %i and the value is %i\n", rank, data);
    }


    MPI_Finalize();
    return 0;
}
