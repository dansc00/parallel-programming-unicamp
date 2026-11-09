#!/bin/bash

# runs hyperfine benchmark with different amounts of threads

for i in {2..16..2}
do
	hyperfine --warmup 2 --prepare "sleep 1" --export-json benchmark$i.json '../build/hello_threads '$i'' '../build/hello_threads_solution '$i''
done
