/*
 *
 * This code implements the first "Hello World" using the paradigm of message passing (MPI) 
 *
 * Detailed documentation of the MPI APis here:
 * https://www.mpi-forum.org/docs/mpi-1.1/mpi-11-html/node182.html
 *
 * Compile with: 
 * $mpicc hello_mpi.c
 * 
 * Run with:
 * $mpirun -np 4 ./a.out
 *
 */

#include <stdlib.h>
#include <stdio.h>

// Header file for compiling code including MPI APIs
#include <mpi.h>


int main( int argc, char * argv[] ){

  int rank = 0; // store the MPI identifier of the process
  int npes = 1; // store the number of MPI processes

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &npes );

  fprintf( stderr, "\nI am process %d of %d MPI processes\n", rank, npes );

  MPI_Finalize();
  
  return 0;

}
