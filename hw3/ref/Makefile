.DEFAULT_GOAL := all

# MPICC = mpic++
# MPICFLAGS = -std=c++11
# MPICOPTFLAGS = -O3 -g -lpng
# MPILDFLAGS =

TARGETS = ref


all: $(TARGETS)

ref: transpose.cu
	nvcc transpose.cu -o $@


clean:
	rm -f $(TARGETS)

cr:
	rm -f REDUCE.*
	rm -f core.*

# eof
