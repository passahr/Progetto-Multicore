#!/bin/bash

echo "Demo started" 


#compile serial algorithm
gcc -o "serialexe" "KMP_Serial.c"
mpicc -g -Wall -o "mpiexe" "KMP_Parallel_MPI.c"
gcc -o "openmpexe" -fopenmp "KMP_Parallel_OpenMP.c"



echo "Serial execution vs 2 cores execution"
./"serialexe" "kill" "pill" "chill"
mpiexec -n 2 ./"mpiexe" "kill" "pill" "chill"
export OMP_NUM_THREADS=2
./"openmpexe" "kill" "pill" "chill"

echo "Serial execution vs 4 cores execution"
./"serialexe" "kill" "pill" "chill"
mpiexec -n 4 ./"mpiexe" "kill" "pill" "chill"
export OMP_NUM_THREADS=4
./"openmpexe" "kill" "pill" "chill"

echo "Serial execution vs 2 cores execution"
./"serialexe" "killhill" "kill" "pill" "chill"
mpiexec -n 2 ./"mpiexe" "killhill" "kill" "pill" "chill"
export OMP_NUM_THREADS=2
./"openmpexe" "killhill" "kill" "pill" "chill"

echo "Serial execution vs 4 cores execution, pattern to find: 'killhill' "
./"serialexe" "killhill" "kill" "pill" "chill"
mpiexec -n 4 ./"mpiexe" "killhill" "kill" "pill" "chill"
export OMP_NUM_THREADS=4
./"openmpexe" "killhill" "kill" "pill" "chill"

echo "Demo ended"

