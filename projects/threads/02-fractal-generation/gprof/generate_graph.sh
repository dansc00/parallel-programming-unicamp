#!/bin/bash

gprof ./mandelbrot gmon.out | python3 gprof2dot.py -f prof -o graph.dot
dot -Tpng graph.dot -o graph.png
