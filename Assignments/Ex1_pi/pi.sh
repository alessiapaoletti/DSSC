#!/bin/bash
for i in 1 2 4 8 16 20;do
	printf "\nnthreads =  ${i}\n"
	export OMP_NUM_THREADS=${i}
	./pi.x
done
