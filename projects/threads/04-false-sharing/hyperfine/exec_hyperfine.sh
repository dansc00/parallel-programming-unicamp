#!/bin/bash

# runs hyperfine benchmark with different amounts of threads

for i in {2..8..2}
do
	hyperfine --warmup 2 --prepare "sleep 1" --export-json benchmark$i.json '../build/sum_scalar 75746584 '$i'' '../build/sum_scalar_solution 75746584 '$i''
done
