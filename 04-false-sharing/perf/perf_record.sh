#!/bin/bash

# execute perf c2c record

perf c2c record -a ./sum_scalar_gdb 100000000 8
mv perf.data perf.data.wrong 
perf c2c record -a ./sum_scalar_solution_gdb 100000000 8
mv perf.data perf.data.solution


