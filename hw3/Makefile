.DEFAULT_GOAL := all

# MPICC = mpic++
# MPICFLAGS = -std=c++11
# MPICOPTFLAGS = -O3 -g -lpng
# MPILDFLAGS =

TARGETS = naive stride sequential first_add unroll multiple

all: $(TARGETS)

naive: naive.cu timer.c
	nvcc naive.cu timer.c -o $@

stride: stride.cu timer.c
	nvcc stride.cu timer.c -o $@

sequential: sequential.cu timer.c
	nvcc sequential.cu timer.c -o $@

first_add: first_add.cu timer.c
	nvcc first_add.cu timer.c -o $@

unroll: unroll.cu timer.c
	nvcc unroll.cu timer.c -o $@

multiple: multiple.cu timer.c
	nvcc multiple.cu timer.c -o $@

clean:
	rm -f $(TARGETS)

cr:
	rm -f REDUCE.*
	rm -f core.*

# eof
