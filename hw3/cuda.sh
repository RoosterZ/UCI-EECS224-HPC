#!/bin/bash
#$ -N REDUCE
#$ -q gpu

# Notes: 
# The GPU node (compute-1-14) has 4 Tesla M2090 GPU cards. The node also
# has 24 Intel cores.
#
# To see how many GPU cards compute-1-14 has avaialble for a job:
# qhost -F gpu -h compute-1-14
#
# For details, see: http://hpc.oit.uci.edu/gpu
#

# Module load Cuda Compilers and GCC
module load  cuda/8.0
module load  gcc/4.9.0

# Runs a bunch of standard command-line
# utilities, just as an example:

echo "Script began:" `date`
echo "Node:" `hostname`
echo "Current directory: ${PWD}"

echo ""
echo "=== Running 5 trials of naive ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./naive
done

echo ""
echo "=== Running 5 trials of stride ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./stride
done

echo ""
echo "=== Running 5 trials of sequential ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./sequential
done

echo ""
echo "=== Running 5 trials of first_add ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./first_add
done

echo ""
echo "=== Running 5 trials of unroll ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./unroll
done

echo ""
echo "=== Running 5 trials of multiple ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./multiple
done

echo ""
echo "=== Running 5 trials of transpose (1024) ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./transpose 1024
done

echo ""
echo "=== Running 5 trials of transpose (2048) ... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./transpose 2048
done

echo ""
echo "=== Running 5 trials of transpose (4096)... ==="
for trial in 1 2 3 4 5; do
  echo "*** Trial ${trial} ***"
  ./transpose 4096
done

echo ""
echo "=== Done! ==="

# eof
