//module load impi
//module load intel
//mpicc pi_mpi.c
//mpirun -np 4 ./a.out

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

double f(double x)
{
    return 1/(1+x*x);
}

int main( int argc, char * argv[] ){
  int rank; // store the MPI identifier of the process
  int npes; // store the number of MPI processes

  double N = 100000000;
  double h = 1/N;
  double loc_sum = 0.0;
  double tot_sum = 0.0;
  double x,i;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &npes );

  double start_time, end_time;
  start_time = MPI_Wtime();

  for (i = rank + 1; i <= N; i += npes) {
	    x = h * (i - 0.5);
	    loc_sum += 4.0 / (1.0 + x*x);
	}
  loc_sum = loc_sum*h;

  MPI_Reduce(&loc_sum, &tot_sum, 1, MPI_DOUBLE , MPI_SUM, npes-1, MPI_COMM_WORLD) ;
  end_time = MPI_Wtime();

  if(rank == npes-1)
  	MPI_Send(&tot_sum , 1, MPI_DOUBLE ,0, 101,MPI_COMM_WORLD);

  if(rank == 0) {
    MPI_Recv(&tot_sum, 1,MPI_DOUBLE, npes-1, 101, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("res = %lf \t time = %lf \n",tot_sum,end_time -start_time);
  }

  MPI_Finalize();


  return 0;

}
