#include <mpi.h>
#include <stdio.h>

int main (int argc, char* argv[]){
  int rank; // store the MPI identifier of the process
  int npes; // store the number of MPI processes

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &npes);

  int prev, next; //auxiliar variables for the ring
  int sum; //variable to store the result

  //Message information
  int sendbuf, recvbuf;
  MPI_Request send;
  MPI_Status status;

  prev = rank - 1; //rank of the previous
  if (prev < 0) prev = prev + npes; //take the n-1 (in prev we have -1) ->  case of the first element in the ring
  next = (rank + 1) % npes; //rank of the next

  //Initialize sum and messagge (sendbuf)
  sum = 0;
  sendbuf = rank;

  for (int i = 0; i < npes; i++) {
    MPI_Isend(&sendbuf, 1, MPI_INT, next, 101, MPI_COMM_WORLD, &send);
    MPI_Recv(&recvbuf, 1, MPI_INT, prev, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    sum = sum + recvbuf;

    MPI_Wait(&send, &status);//waiting the end of the send
    sendbuf = recvbuf;
  }

  printf("Rank %d, result = %d\n",rank, sum);
  MPI_Finalize();

  return 0;

}
