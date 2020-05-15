#!/bin/bash
#$ -N Mandelbrot_ms
#$ -q class
#$ -pe mpi 64
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

trial=3
if_render=0
start_sz=256
end_sz=16384

# Run the program 
mpirun -np 2    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 4    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 8    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 16   ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 32   ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 64   ./mandelbrot_ms $start_sz $end_sz $trial $if_render
