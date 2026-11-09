#!/bin/bash

# runs hyperfine benchmark with different amounts of inputs

for i in {1..5}
do
	hyperfine --warmup 2 --prepare "sleep 1" --export-json benchmark$i.json '../build/serial ../tests/'$i'.in' '../build/parallel ../tests/'$i'.in'
done

