#!/bin/bash

# runs hyperfine benchmark with different amounts of threads

for i in {2..16..2}
do
	hyperfine --warmup 2 --prepare "sleep 1" --export-json benchmark$i.json '../build/monte_carlo_serial 40000000 '$i'' '../build/monte_carlo_parallel 40000000 '$i''
done
