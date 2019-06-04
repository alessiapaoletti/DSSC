#module load openmp

CXX = gcc
OPENPM = -fopenmp

all: pi.x

pi.x: pi.c
	$(CXX) -o $@ $(OPENPM) $< -lm -std=c99 -Wall -Wextra

clean:
	@rm -r *.x

.PHONY: clean all
