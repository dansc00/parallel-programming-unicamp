#!/bin/bash

perf record -a -o parallel_record ./parallel tests/5.in
