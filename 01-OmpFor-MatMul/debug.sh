#!/bin/bash

# make debug directories and compile with debug flags, create an executable binary in each directory and add configuration files

SERIAL_SOURCE=$1
SERIAL_OUTPUT=$2
PARALLEL_SOURCE=$3
PARALLEL_OUTPUT=$4
ARGUMENT="tests/5.in"
DEBUG_SOURCES="gperftools perf hyperfine"
FLAGS="-g -o"
LIBRARIES="-fopenmp -lprofiler"
array=($DEBUG_SOURCES)

for out in "${array[@]}";
do
	mkdir -p ./$out
	gcc $SERIAL_SOURCE $FLAGS ./$out/$SERIAL_OUTPUT $LIBRARIES
	gcc $PARALLEL_SOURCE $FLAGS ./$out/$PARALLEL_OUTPUT $LIBRARIES
done


# make perf directories and scripts
mkdir -p perf/perfc2c
touch perf/perf_serial.sh
touch perf/perf_parallel.sh
touch perf/report.txt
touch perf/perfc2c/report.txt

echo "#!/bin/bash

perf record -a -o serial_record ./serial ../$ARGUMENT" > perf/perf_serial.sh

echo "#!/bin/bash

perf record -a -o parallel_record ./parallel ../$ARGUMENT" > perf/perf_parallel.sh

echo "perf report -i input" > perf/report.txt

echo "#!/bin/bash

perf c2c record -a -o serial_record ../serial ../$ARGUMENT" > perf/perfc2c/perf_serial.sh

echo "#!/bin/bash

perf c2c record -a -o parallel_record ../parallel ../$ARGUMENT" > perf/perfc2c/perf_parallel.sh

echo "perf c2c report -i input --stdio" > perf/perfc2c/report.txt

# gperftools guide

echo "Add gperftools profiler delimiter functions in the code, runs to generate .prof files, use google-pprof --gv ./bin bin.prof to show nodes graph. See help to other options" > gperftools/guide.txt
