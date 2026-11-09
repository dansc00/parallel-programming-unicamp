#!/bin/bash

valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out.wrong ./sum_scalar_gdb 100000000 8
valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out.solution ./sum_scalar_solution_gdb 100000000 8

