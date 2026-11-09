#!/bin/bash

# compile with -g 

gcc ../src/sum_scalar.c -g -Wall -o sum_scalar_gdb -pthread
gcc ../src/sum_scalar_solution.c -g -Wall -o sum_scalar_solution_gdb -pthread

