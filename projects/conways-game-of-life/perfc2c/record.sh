#!/bin/bash

perf record -a -g -o serial-matrix.perf ../build/serial-matrix $1
perf record -a -g -o serial-set.perf ../build/serial-set $1
perf record -a -g -o parallel-matrix.perf ../build/parallel-matrix $1
perf record -a -g -o parallel-set.perf ../build/parallel-set $1