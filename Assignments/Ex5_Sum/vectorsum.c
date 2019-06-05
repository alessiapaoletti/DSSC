#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void swap(int** a, int** b){
    int* t = *a;
    *a = *b;
    *b = t;
}

void copy (int *a, int *b, int N){
  for (int i = 0; i < N; i++) {
    a[i] = b[i];
  }
}

void print_vector(int *v,int N){
  for (int i = 0; i < N; i++)
    printf("%d\t",v[i] );
  printf("\n");
}

void sum_vector(int *v1, int* v2, int N){
  for (int i = 0; i < N; i++)
    v1[i]+=v2[i];
}

int main (int argc, char* argv[]){
  int rank; // store the MPI identifier of the process
  int npes; // store the number of MPI processes

  MPI_Init(&argc,&argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  const int N = (argc >= 2 ? atoi(argv[1]): 4); //default size of the vector
  int prev, next; //auxiliar variables for the ring
  int* sum = (int*)malloc(sizeof(int)*N);

  //Message information
  int* sendbuf = (int*)malloc(sizeof(int)*N);
  int* recvbuf = (int*)malloc(sizeof(int)*N);
  MPI_Request send;
  MPI_Status status;

  prev = rank - 1; //rank of the previous
  if (prev < 0) prev = prev + npes; //take the n-1 (in prev we have -1) ->  case of the first element in the ring
  next = (rank + 1) % npes; //rank of the next

  //Initialization
  for (int i = 0; i < N; i++) sum[i] = 0;
  for (int i = 0; i < N; i++) sendbuf[i] = rank;
  for (int i = 0; i < N; i++) recvbuf[i] = 0;

  for (int i = 0; i < npes; i++) {
    MPI_Isend(sendbuf, N, MPI_INT, next, 101, MPI_COMM_WORLD, &send);
    MPI_Recv(recvbuf, N, MPI_INT, prev, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    sum_vector(sum,recvbuf,N);

    MPI_Wait(&send, &status);//waiting the end of the send
    swap(&recvbuf,&sendbuf);
  }

  printf("Rank %d\tresult = %d\n",rank, sum[0]);
  print_vector(sum,N);
  MPI_Finalize();

  free(sum);
  free(recvbuf);
  free(sendbuf);

  return 0;
}
