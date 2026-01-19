#!/bin/bash
# Script compile tất cả các phiên bản cần thiết

echo "========================================="
echo "COMPILING ALL VERSIONS"
echo "========================================="

mkdir -p build
cd vtsh

# 1. CPU Factorize - Baseline (no optimization)
echo "[1/5] Compiling cpu_factorize (baseline -O0)..."
gcc -O0 -o ../build/bin/cpu-factorize cpu-factorize.c
echo "  ✓ build/cpu_factorize"

# 2. CPU Factorize - Optimized
echo "[2/5] Compiling cpu_factorize_O3 (optimized -O3)..."
gcc -O3 -o ../build/bin/cpu_factorize_O3 cpu-factorize.c
echo "  ✓ build/cpu_factorize_O3"

# 3. CPU Factorize - Multi-threaded
echo "[3/5] Compiling cpu_factorize_mt (multi-threaded)..."
gcc -O0 -pthread -o ../build/bin/cpu_factorize_mt cpu-factorize-mt.c
echo "  ✓ build/cpu_factorize_mt"

# 4. EMA Replace
echo "[4/5] Compiling ema_replace..."
gcc -O0 -o ../build/bin/ema-replace-int ema-replace-int.c
echo "  ✓ build/ema_replace"

# 5. Shell
echo "[5/5] Compiling myshell..."
gcc -O0 -o ../build/bin/myshell myshell.c
echo "  ✓ build/myshell"

cd ..

echo ""
echo "========================================="
echo "COMPILATION COMPLETE!"
echo "========================================="
echo "Binaries in build/bin/ directory:"
ls -lh build/bin/
echo ""
