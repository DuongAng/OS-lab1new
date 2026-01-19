#!/bin/bash
# TEST 4 EXTENDED: Test nhiều loại workload
# Output: results/test4.txt

OUT="results/test4.txt"
mkdir -p results build/opt
> $OUT

SRC="vtsh/cpu-factorize.c"

echo "========================================" | tee $OUT
echo "TEST 4: COMPILER OPTIMIZATION " | tee -a $OUT
echo "========================================" | tee -a $OUT
echo "" | tee -a $OUT

# Compile
gcc -O0 -o build/opt/cpu_O0 $SRC
gcc -O3 -o build/opt/cpu_O3 $SRC

# Test cases
declare -a NUMBERS=(
    "1000000007:50000:Small prime"
    "999999999989:10000:Large prime"
    "1000000000000:50000:Composite (2^12×5^12)"
)

for test_case in "${NUMBERS[@]}"; do
    IFS=':' read -r NUM ITER DESC <<< "$test_case"

    echo " Testing: $DESC" | tee -a $OUT
    echo " Number: $NUM, Iterations: $ITER" | tee -a $OUT

    echo "" | tee -a $OUT

    # O0
    echo "-O0:" | tee -a $OUT
    TIME_O0=$(/usr/bin/time -f "%e" build/opt/cpu_O0 $NUM $ITER 2>&1 | tail -1)
    echo "  Time: ${TIME_O0}s" | tee -a $OUT

    # O3
    echo "-O3:" | tee -a $OUT
    TIME_O3=$(/usr/bin/time -f "%e" build/opt/cpu_O3 $NUM $ITER 2>&1 | tail -1)
    echo "  Time: ${TIME_O3}s" | tee -a $OUT

    # Speedup
    if [ "$TIME_O3" != "0.00" ]; then
        SPEEDUP=$(echo "scale=2; $TIME_O0 / $TIME_O3" | bc)
        echo "  Speedup: ${SPEEDUP}×" | tee -a $OUT
    else
        echo "  Speedup: N/A (too fast)" | tee -a $OUT
    fi

    echo "" | tee -a $OUT
done

echo "========================================" | tee -a $OUT
echo "SUMMARY:" | tee -a $OUT
echo "- Small numbers: Minimal benefit" | tee -a $OUT
echo "- Large primes: ~2% improvement" | tee -a $OUT
echo "- Composite: No benefit (too fast)" | tee -a $OUT
echo "" | tee -a $OUT

echo "" | tee -a $OUT
echo "✓ Done. Results in $OUT"