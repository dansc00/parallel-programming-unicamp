#!/bin/bash

# runs hyperfine benchmark with different amounts of inputs

FIRST_INPUT=$1
SECOND_INPUT=$2

for i in {1..5..1}
do
	hyperfine --warmup 2 --prepare "sleep 1" --export-json benchmark$i.json ''$1' ../tests/'$i'.in' ''$2' ../tests/'$i'.in'
done

