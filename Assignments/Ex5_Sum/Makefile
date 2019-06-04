SRC = sum.c \
			vectorsum.c

CXX = mpicc
CXXFLAGS = -Wall -Wextra

EXE = $(SRC:.c=.x)

# eliminate default suffixes
.SUFFIXES:
SUFFIXES =

# just consider our own suffixes
.SUFFIXES: .c .x

all: $(EXE)

.PHONY: all

%.x: %.c
	$(CXX) $< -o $@ $(CXXFLAGS)

.PHONY: format

clean:
	rm -f $(EXE) *~

.PHONY: clean
