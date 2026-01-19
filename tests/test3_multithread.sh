#!/bin/bash
# TEST 3: Multi-threaded vs Multi-process
# Output: results/test3.txt

OUT="results/test3.txt"
mkdir -p results
> $OUT

PROG_MP="build/bin/cpu-factorize"
PROG_MT="build/bin/cpu_factorize_mt"
NUM="1000000007"
TOTAL=80000
N=8

echo "========================================" | tee $OUT
echo "TEST 3: THREADS vs PROCESSES" | tee -a $OUT
echo "Total work: $TOTAL iterations" | tee -a $OUT
echo "Threads/Processes: $N" | tee -a $OUT
echo "========================================" | tee -a $OUT
echo "" | tee -a $OUT

# Multi-threaded
if [ -f "$PROG_MT" ]; then
    echo "--- Multi-threaded (pthread) ---" | tee -a $OUT
    /usr/bin/time -f "Time: %e s\nMemory: %M KB" \
        $PROG_MT $NUM $TOTAL $N 2>&1 | tee -a $OUT
    echo "" | tee -a $OUT
else
    echo "⚠ Compile first:" | tee -a $OUT
    echo "gcc -pthread -O0 -o $PROG_MT vtsh/cpu-factorize-mt.c" | tee -a $OUT
    echo "" | tee -a $OUT
fi

# Multi-process
echo "--- Multi-process ---" | tee -a $OUT
PER_PROC=$((TOTAL / N))

# Tạo helper script
HELPER="build/bin/run_all_processes.sh"
cat > $HELPER << HELPER_EOF
#!/bin/bash
for i in \$(seq 1 $N); do
    $PROG_MP $NUM $PER_PROC > /dev/null 2>&1 &
done
wait
HELPER_EOF
chmod +x $HELPER

# Đo với /usr/bin/time
/usr/bin/time -f "Time: %e s\nMemory: %M KB" \
    bash $HELPER 2>&1 | tee -a $OUT

echo "" | tee -a $OUT

echo "========================================" | tee -a $OUT
echo "EXPECTED: Similar time, threads use less memory" | tee -a $OUT
echo "========================================" | tee -a $OUT

echo "" | tee -a $OUT
echo "✓ Done. Results in $OUT"