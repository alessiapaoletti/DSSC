#module load openmp
CXX = gcc
OPENPM = -fopenmp

all: loop.x

loop.x: loop.c
	$(CXX) -o $@ $(OPENPM) $< -std=c99 -Wall -Wextra

clean:
	@rm -r *.x

.PHONY: clean all
