#!/bin/bash

g++ -pg -g -o mandelbrot ../src/main.cpp ../src/mandelbrotThread.cpp ../src/mandelbrotSerial.cpp ../src/ppm.cpp -lpthread

