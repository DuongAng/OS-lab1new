#!/bin/bash
# TEST 1: BASELINE - 1 process
# Đo: Thời gian, CPU%, Context switches
# Output: results/test1.txt

OUT="results/test1.txt"
mkdir -p results
> $OUT

PROG="build/bin/cpu-factorize"
ARGS="1000000007 5000"

echo "========================================" | tee $OUT
echo "TEST 1: BASELINE (1 process)" | tee -a $OUT
echo "Date: $(date '+%Y-%m-%d %H:%M')" | tee -a $OUT
echo "========================================" | tee -a $OUT
echo "" | tee -a $OUT

# Main measurement
echo "[Running: $PROG $ARGS]" | tee -a $OUT
echo "" | tee -a $OUT

/usr/bin/time -f "\
Time: %E (elapsed)
User: %U (seconds)
System: %S (seconds)
CPU: %P
Context switches: %c voluntary, %w involuntary" \
$PROG $ARGS 2>&1 | tee -a $OUT

echo "" | tee -a $OUT
echo "✓ Done. Results in $OUT"