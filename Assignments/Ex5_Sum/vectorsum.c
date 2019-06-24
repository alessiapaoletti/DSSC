#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define RIGHT(proc, nproc) (proc - 1 + nproc) % nproc
#define LEFT(proc, nproc) (proc + 1 + nproc) % nproc

void vector_sum(int* out, int* in, size_t size){
    size_t i;
    for(i = 0; i<size; i++)
        out[i] += in[i];
}

void fill_vector(int* v, int x, size_t size){
    size_t i;
    for(i = 0; i<size; i++)
        v[i] = x;
}

void print_vector(int* v, size_t size){
    size_t i;
    for(i = 0; i<size; i++)
        printf("%d ", v[i]);

    printf("\n");
}

void swapPointers(int** a, int** b){
    int* t = *a;
    *a = *b;
    *b = t;
}

int main(int argc, char* argv[]){

    int rank; // identifier of the process
    int np; // total number of MPI processes

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &np );
    MPI_Request request;
    MPI_Status status;

    size_t N = argc<2 ? 4 : atoi(argv[1]);
    int i;
    int* recbuf = malloc(sizeof(int)*N); // buffer for receiving the message
    int* sendbuf = malloc(sizeof(int)*N); // buffer for sending the message
    fill_vector(sendbuf, rank, N);
    int* sum = malloc(sizeof(int)*N);
    fill_vector(sum, 0, N);

    for(i=0; i<np; i++)
    {
        MPI_Isend(sendbuf, N, MPI_INT, LEFT(rank,np), 101, MPI_COMM_WORLD, &request);
        vector_sum(sum, sendbuf, N);
        MPI_Recv(recbuf, N, MPI_INT, RIGHT(rank,np), 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Wait(&request, &status);
        swapPointers(&recbuf,&sendbuf);
    }
    printf("Rank = %d \t Sum = %d\n", rank, sum[0]);

    free(recbuf);
    free(sendbuf);
    free(sum);

}
