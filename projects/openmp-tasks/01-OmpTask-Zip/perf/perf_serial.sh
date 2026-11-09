#!/bin/bash

perf record -a -o serial_record ./serial tests/5.in
