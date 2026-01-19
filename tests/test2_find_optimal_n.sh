#!/bin/bash
# TEST 2: Tìm số process tối ưu
# FINAL VERSION: Tăng workload để giảm noise
# Output: results/test2.txt

OUT="results/test2.txt"
mkdir -p results
> $OUT

PROG="build/bin/cpu-factorize"
TOTAL_WORK=100000

echo "========================================" | tee $OUT
echo "TEST 2: OPTIMAL NUMBER OF PROCESSES" | tee -a $OUT
echo "System: $(nproc) CPUs (8 cores × 2 HT)" | tee -a $OUT
echo "Total workload: $TOTAL_WORK iterations" | tee -a $OUT
echo "========================================" | tee -a $OUT
echo "" | tee -a $OUT

for N in 1 2 4 8 16; do
    WORK_PER_PROC=$((TOTAL_WORK / N))

    echo "--- Testing $N processes ---" | tee -a $OUT
    echo "Each process: $WORK_PER_PROC iterations" | tee -a $OUT

    START=$(date +%s.%N)

    for i in $(seq 1 $N); do
        $PROG 1000000007 $WORK_PER_PROC &
    done

    wait

    END=$(date +%s.%N)
    TIME=$(echo "$END - $START" | bc)

    echo "Time: ${TIME}s" | tee -a $OUT

    if [ "$N" -eq 1 ]; then
        BASELINE=$TIME
        echo "Baseline" | tee -a $OUT
    else
        SPEEDUP=$(echo "scale=2; $BASELINE / $TIME" | bc)
        EFFICIENCY=$(echo "scale=1; $SPEEDUP / $N * 100" | bc)
        echo "Speedup: ${SPEEDUP}×" | tee -a $OUT
        echo "Efficiency: ${EFFICIENCY}%" | tee -a $OUT
    fi

    echo "" | tee -a $OUT
done

echo "========================================" | tee -a $OUT

echo "" | tee -a $OUT
echo "✓ Done. Results in $OUT"