#!/bin/bash

echo "Demo started"

#compile serial algorithm
gcc -o "serialexe" "KMP_Serial.c"
mpicc -g -Wall -o "mpiexe" "KMP_Parallel_MPI.c"
gcc -o "openmpexe" -fopenmp "KMP_Parallel_OpenMP.c"

echo "Serial execution vs 2 cores execution, pattern to find: 'kill' "
./"serialexe" "kill"
mpiexec -n 2 ./"mpiexe" "kill"
export OMP_NUM_THREADS=2
./"openmpexe" "kill"

echo "Serial execution vs 4 cores execution, pattern to find: 'kill' "
./"serialexe" "kill"
mpiexec -n 4 ./"mpiexe" "kill"
export OMP_NUM_THREADS=4
./"openmpexe" "kill"



