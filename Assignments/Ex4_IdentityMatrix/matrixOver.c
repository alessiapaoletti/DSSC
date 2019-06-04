#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void init_matrix(double* M, int size){
  for(int i = 0; i < size; i++){
      M[i]=0.0;
  }
}

void print_matrix(double* M, int row, int col){
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++)
      printf("%.0f ", M[i*col + j]);
  printf("\n");
  }
}

void file_print_matrix(double* M, int row, int col, FILE* fp){
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++)
      fprintf(fp,"%.0f\t", M[i*col + j]);
    fprintf(fp,"\n");
  }
}

void swap_int(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void swap_buffer(double** x, double** y){
  double* tmp = *x;
  *x = *y;
  *y = tmp;
}

int main(int argc, char *argv[]) {
  int rank; // store the MPI identifier of the process
  int npes; // store the number of MPI processes
  const int root = 0; //root process
  const int N = (argc >= 2 ? atoi(argv[1]): 4); //default size 4

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &npes );

  int nloc = N/npes; //local n
  int rest = N % npes;
  if (rank < rest) nloc++;
  //printf("rank = %d \t nloc = %d\n",rank,nloc);
  double *mat = (double *)malloc (sizeof(double)*nloc*N);
  init_matrix(mat,nloc*N);

  int i_global;
  for (int i = 0; i < nloc; i++) {
    if (rank<rest) i_global = i +(rank*nloc);
    else i_global = i +((rank)*nloc)+rest;
    for (int j = 0; j < N; j++) {
      if (i_global == j ) mat[i*N +j]=1;
    }
  }

  if(rank==root){
    FILE *fp;
    if (N>10) fp = fopen("matrix.dat","wb");  //w for write, b for binary
    MPI_Request receive;
    MPI_Status status;
    for (int i = 1; i < npes; i++) {
      //for each process I have to compute the size of the matrix I have to receive
      int nloc_rec = N/npes;
      int rest_rec = N%npes;
      if (i<rest_rec) nloc_rec++;
      double *recBuffer = (double*)malloc(sizeof(double)*nloc_rec*N);
      MPI_Irecv(recBuffer, nloc_rec*N, MPI_DOUBLE, i, 101, MPI_COMM_WORLD, &receive);
      if (N<10) print_matrix(mat,nloc,N);
      else file_print_matrix(mat,nloc,N,fp);
      MPI_Wait(&receive, &status);
      swap_buffer(&mat,&recBuffer); //swap the buffers
      swap_int(&nloc,&nloc_rec); //swap the local_n
      free(recBuffer);
    }
    if (N<10) print_matrix(mat,nloc,N);
    else file_print_matrix(mat,nloc,N,fp);
    if (N>10) fclose(fp);
  } else {
    MPI_Send(mat, nloc*N, MPI_DOUBLE, root, 101,MPI_COMM_WORLD);
  }

  free(mat);
  MPI_Finalize();

  return 0;
}
