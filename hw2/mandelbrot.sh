#!/bin/bash
#$ -N Mandelbrot
#$ -q class16
#$ -pe mpi 48
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

trial=3         # Average the running time of 3 tests
if_render=0     # Do not render the image
start_sz=256    # Start from image size 256 * 256 
end_sz=16384    # End with image size 16386 * 16384

# Serial
mpirun -np 1  ./mandelbrot_serial $start_sz $end_sz $trial $if_render

# Susie's strategy
mpirun -np 2    ./mandelbrot_susie $start_sz $end_sz $trial $if_render
mpirun -np 4    ./mandelbrot_susie $start_sz $end_sz $trial $if_render
mpirun -np 8    ./mandelbrot_susie $start_sz $end_sz $trial $if_render
mpirun -np 16   ./mandelbrot_susie $start_sz $end_sz $trial $if_render
mpirun -np 32   ./mandelbrot_susie $start_sz $end_sz $trial $if_render
mpirun -np 48   ./mandelbrot_susie $start_sz $end_sz $trial $if_render

# Joe's strategy
mpirun -np 2    ./mandelbrot_joe $start_sz $end_sz $trial $if_render
mpirun -np 4    ./mandelbrot_joe $start_sz $end_sz $trial $if_render
mpirun -np 8    ./mandelbrot_joe $start_sz $end_sz $trial $if_render
mpirun -np 16   ./mandelbrot_joe $start_sz $end_sz $trial $if_render
mpirun -np 32   ./mandelbrot_joe $start_sz $end_sz $trial $if_render
mpirun -np 48   ./mandelbrot_joe $start_sz $end_sz $trial $if_render

# Master/slave model
mpirun -np 2    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 4    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 8    ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 16   ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 32   ./mandelbrot_ms $start_sz $end_sz $trial $if_render
mpirun -np 48   ./mandelbrot_ms $start_sz $end_sz $trial $if_render