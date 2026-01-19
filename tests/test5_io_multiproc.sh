#!/bin/bash
# TEST 5: IO Multi-process (Does IO scale?)
# Output: results/test5.txt

OUT="results/test5.txt"
mkdir -p results data
> $OUT

PROG="build/bin/ema-replace-int"

echo "========================================" | tee $OUT
echo "TEST 5: IO SCALING" | tee -a $OUT
echo "========================================" | tee -a $OUT
echo "" | tee -a $OUT

for N in 1 2 4; do
    echo "--- $N IO processes ---" | tee -a $OUT

    # Create files
    for i in $(seq 1 $N); do
        if [ ! -f "data/io_${i}.bin" ]; then
            $PROG --rw write --block_count 12500 \
                --file data/io_${i}.bin > /dev/null 2>&1
        fi
    done

    sync
    sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches' 2>/dev/null || true

    START=$(date +%s.%N)

    for i in $(seq 1 $N); do
        $PROG --rw search --search 12345 --replace 99999 \
            --file data/io_${i}.bin --iterations 1 &
    done

    wait

    END=$(date +%s.%N)
    TIME=$(echo "$END - $START" | bc)

    echo "Time: ${TIME}s" | tee -a $OUT
    echo "" | tee -a $OUT
done

echo "========================================" | tee -a $OUT
echo "✓ Done. Results in $OUT"