#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10000000000

double f(double x) { return 1 / (1 + x * x); }

int main(int argc, char *argv[]) {
  int rank = 0;
  int npes = 1;
  double global;
  double h = 1. / N;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &npes);

  double tstart = MPI_Wtime();

  long int start = N / npes * rank;
  long int end;
  if (rank + 1 == npes && N % npes != 0)
    end = N % npes + N / npes * (rank + 1);
  else
    end = N / npes * (rank + 1);
  long int i;
  double local = 0;
  for (i = start; i < end; i++) {
    double x_i = i * h + h / 2;
    local += f(x_i);
  }

  MPI_Reduce(&local, &global, 1, MPI_DOUBLE, MPI_SUM, npes - 1, MPI_COMM_WORLD);
  double tend = MPI_Wtime();

  if (rank == npes - 1)
    MPI_Send(&global, 1, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD);
  if (rank == 0) {
    MPI_Recv(&global, 1, MPI_DOUBLE, npes - 1, 101, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("res = %f \t time = %f  \n", h * global * 4, tend - tstart);
  }

  MPI_Finalize();

  return 0;
}
