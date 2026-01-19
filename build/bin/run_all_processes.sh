#!/bin/bash
for i in $(seq 1 8); do
    build/bin/cpu-factorize 1000000007 10000 > /dev/null 2>&1 &
done
wait
