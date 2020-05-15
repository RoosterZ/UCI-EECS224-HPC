#!/bin/bash
#$ -N Mandelbrot_Susie
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
mpirun -np 2 -ppn 1  ./mandelbrot_susie 1024 2048 2 1
mpirun -np 4 -ppn 2  ./mandelbrot_susie 1024 2048 2 1