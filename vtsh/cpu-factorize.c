//
// Created by admin on 1/18/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define MAX_FACTORS 100

typedef struct {
    uint64_t factor;
    int count;
} FactorPair;

int is_prime(uint64_t n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    for (uint64_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int factorize(uint64_t number, FactorPair *factors) {
    int factor_count = 0;
    uint64_t n = number;

    if (n % 2 == 0) {
        factors[factor_count].factor = 2;
        factors[factor_count].count = 0;
        while (n % 2 == 0) {
            factors[factor_count].count++;
            n /= 2;
        }
        factor_count++;
    }

    for (uint64_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            factors[factor_count].factor = i;
            factors[factor_count].count = 0;
            while (n % i == 0) {
                factors[factor_count].count++;
                n /= i;
            }
            factor_count++;
        }
    }

    if (n > 1) {
        factors[factor_count].factor = n;
        factors[factor_count].count = 1;
        factor_count++;
    }

    return factor_count;
}

void print_factors(uint64_t number, FactorPair *factors, int count) {
    printf("%lu = ", number);
    for (int i = 0; i < count; i++) {
        if (i > 0) printf(" × ");
        printf("%lu", factors[i].factor);
        if (factors[i].count > 1) {
            printf("^%d", factors[i].count);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    uint64_t number;
    int iterations = 1;
    struct timespec start, end;

    if (argc < 2) {
        printf("Usage: %s <number> [iterations]\n", argv[0]);
        printf("  number: The number to be factored (should be the product of large prime numbers)\n");
        printf("  iterations:Number of iterations (default: 1)\n");
        printf("\nFor example:\n");
        printf("  %s 1000000007\n", argv[0]);
        printf("  %s 9876543210987654321 100\n", argv[0]);
        return 1;
    }

    number = strtoull(argv[1], NULL, 10);

    if (argc >= 3) {
        iterations = atoi(argv[2]);
    }

    printf("CPU Factorization Loader\n");
    printf("========================================\n");
    printf("Number: %lu\n", number);
    printf("Iterations: %d\n", iterations);
    printf("========================================\n\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    FactorPair factors[MAX_FACTORS];
    int factor_count = 0;

    // Lặp nhiều lần để load CPU
    for (int i = 0; i < iterations; i++) {
        factor_count = factorize(number, factors);

        // Chỉ in kết quả lần đầu hoặc mỗi 10% tiến trình
        if (i == 0 || (iterations >= 10 && i % (iterations / 10) == 0)) {
            printf("[Iteration %d/%d] ", i + 1, iterations);
            print_factors(number, factors, factor_count);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("RESULTS:\n");
    printf("========================================\n");
    printf("Total iterations: %d\n", iterations);
    printf("Total time: %.2f ms\n", elapsed);
    printf("Average time per iteration: %.4f ms\n", elapsed / iterations);
    printf("Factorization result: ");
    print_factors(number, factors, factor_count);
    printf("=================DONE==================\n");

    return 0;
}
