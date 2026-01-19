cmake_minimum_required(VERSION 3.28)
project(lab1 C)

set(CMAKE_C_STANDARD 11)

# Nơi xuất tất cả executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Target main
add_executable(lab1 main.c)

# Target myshell
add_executable(myshell vtsh/myshell.c)

# Target cpu-factorize
add_executable(factorize vtsh/cpu-factorize.c)

# Target ema-replace-int
add_executable(ema-replace-int vtsh/ema-replace-int.c)

# Target multi-threading
add_executable(factorize_mt vtsh/cpu-factorize-mt.c)
