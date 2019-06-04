#module load openmpi
CXX = gcc
CXX1 = mpicc
OPENPM = -fopenmp

all: pi_mpi.x pi_omp.x

pi_mpi.x: pi_mpi.c
	$(CXX1) -o $@  $< -Wall -Wextra

pi_omp.x: pi_omp.c
		$(CXX) -o $@ $(OPENPM) $< -std=c99 -Wall -Wextra
clean:
	@rm -r *.x

.PHONY: clean all
