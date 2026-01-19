//
// Created by admin on 1/19/2026.
//
// cpu-factorize-mt.c - Multi-threaded version
// Lưu vào: vtsh/cpu-factorize-mt.c
// Compile: gcc -pthread -O0 -o build/bin/cpu_factorize_mt vtsh/cpu-factorize-mt.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int thread_id;
    long long number;
    int iterations;
} thread_arg_t;

void factorize(long long n) {
    long long temp = n;
    while (temp % 2 == 0) temp /= 2;
    for (long long i = 3; i * i <= temp; i += 2) {
        while (temp % i == 0) temp /= i;
    }
}

void* worker(void* arg) {
    thread_arg_t* a = (thread_arg_t*)arg;
    for (int i = 0; i < a->iterations; i++) {
        factorize(a->number);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <number> <total_iterations> <num_threads>\n", argv[0]);
        return 1;
    }

    long long number = atoll(argv[1]);
    int total = atoi(argv[2]);
    int n_threads = atoi(argv[3]);
    int per_thread = total / n_threads;

    printf("Multi-threaded Factorization\n");
    printf("Number: %lld, Total: %d, Threads: %d\n", number, total, n_threads);
    printf("Per thread: %d iterations\n\n", per_thread);

    pthread_t* threads = malloc(n_threads * sizeof(pthread_t));
    thread_arg_t* args = malloc(n_threads * sizeof(thread_arg_t));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < n_threads; i++) {
        args[i].thread_id = i;
        args[i].number = number;
        args[i].iterations = per_thread;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Total time: %.3f seconds\n", elapsed);

    free(threads);
    free(args);
    return 0;
}