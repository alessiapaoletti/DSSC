#!/bin/bash

for i in 2 4 10 16 20 30 40;do
  echo "np = ${i} " >> pi_mpi.txt
	mpirun -np ${i} ./pi_mpi.x >> pi_mpi.txt
done

for i in 2 4 10 16 20; do
  echo "threads  = ${i} " >> pi_omp.txt
  export OMP_NUM_THREADS=${i}
  ./pi_omp.x >> pi_omp.txt
done
