#!/bin/bash
#$ -N Mandelbrot_ms
#$ -q class
#$ -pe mpi 16
#$ -R y

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load mpich-3.0.4/gcc-4.8.3

# Run the program 
mpirun -np 16 -ppn 8  ./mandelbrot_ms 1000 1000 5
