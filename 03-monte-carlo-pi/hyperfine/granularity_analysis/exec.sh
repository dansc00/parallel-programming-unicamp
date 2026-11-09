#!/bin/bash

n=4000

for i in {1..5}
do
	hyperfine --warmup 1 --prepare "sleep 1" --export-json benchmark$i.json '../../build/monte_carlo_serial '$n' 12' '../../build/monte_carlo_parallel '$n' 12'
	((n = n * 10))
done
