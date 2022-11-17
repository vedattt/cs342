#!/bin/bash

for ((i = 1; i <= ${1:-1}; i++)); do
    echo "iteration #$i"

    echo PL tests 150 - 300 - 450 - 600 - 750
    ./cfs C -3 3 exponential 150 100 3000 exponential 400 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 300 100 3000 exponential 400 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 450 100 3000 exponential 400 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 600 100 3000 exponential 400 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 750 100 3000 exponential 400 100 3000 20 15 1 | tail -1

    echo

    echo IAT tests 150 - 300 - 450 - 600 - 750
    ./cfs C -3 3 exponential 400 100 3000 exponential 150 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 400 100 3000 exponential 300 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 400 100 3000 exponential 450 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 400 100 3000 exponential 600 100 3000 20 15 1 | tail -1
    ./cfs C -3 3 exponential 400 100 3000 exponential 750 100 3000 20 15 1 | tail -1
done

