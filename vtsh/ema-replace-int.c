//
// Created by admin on 1/18/2026.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_BLOCK_SIZE 4096
#define DEFAULT_BLOCK_COUNT 1000

typedef enum {
    MODE_READ,
    MODE_WRITE,
    MODE_SEARCH_REPLACE
} AccessMode;

typedef enum {
    TYPE_SEQUENTIAL,
    TYPE_RANDOM
} AccessType;

// Cấu hình chương trình
typedef struct {
    AccessMode mode;
    size_t block_size;
    size_t block_count;
    char filename[256];
    long range_start;
    long range_end;
    int use_direct;
    AccessType access_type;
    int search_value;
    int replace_value;
    int iterations;
} Config;

// Tạo file với dữ liệu ngẫu nhiên
void generate_file(Config *cfg) {
    int fd;
    int flags = O_WRONLY | O_CREAT | O_TRUNC;

    if (cfg->use_direct) {
        flags |= O_DIRECT;
    }

    fd = open(cfg->filename, flags, 0644);
    if (fd < 0) {
        perror("Cannot create file");
        exit(1);
    }

    printf("Generating file '%s' with %zu blocks of %zu bytes...\n",
           cfg->filename, cfg->block_count, cfg->block_size);

    // Allocate aligned buffer cho O_DIRECT
    void *buffer;
    if (posix_memalign(&buffer, 512, cfg->block_size) != 0) {
        perror("posix_memalign failed");
        exit(1);
    }

    srand(time(NULL));

    for (size_t i = 0; i < cfg->block_count; i++) {
        // Fill buffer với random integers
        int *int_buffer = (int *)buffer;
        size_t int_count = cfg->block_size / sizeof(int);

        for (size_t j = 0; j < int_count; j++) {
            int_buffer[j] = rand() % 100000;
        }

        if (write(fd, buffer, cfg->block_size) != (ssize_t)cfg->block_size) {
            perror("Write failed");
            free(buffer);
            close(fd);
            exit(1);
        }

        if ((i + 1) % 100 == 0) {
            printf("  Progress: %zu/%zu blocks\n", i + 1, cfg->block_count);
        }
    }

    free(buffer);
    close(fd);
    printf("File generated successfully!\n\n");
}

// Tìm và thay thế giá trị trong file
int search_and_replace(Config *cfg) {
    int fd;
    int flags = O_RDWR;
    int found_count = 0;
    int replaced_count = 0;

    if (cfg->use_direct) {
        flags |= O_DIRECT;
    }

    fd = open(cfg->filename, flags);
    if (fd < 0) {
        perror("Cannot open file");
        return -1;
    }

    // Get file size
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat failed");
        close(fd);
        return -1;
    }

    size_t file_size = st.st_size;
    size_t blocks_to_process = file_size / cfg->block_size;

    // Apply range if specified
    if (cfg->range_end > 0 && cfg->range_end < (long)file_size) {
        blocks_to_process = (cfg->range_end - cfg->range_start) / cfg->block_size;
    }

    printf("Searching for value %d and replacing with %d...\n",
           cfg->search_value, cfg->replace_value);
    printf("Processing %zu blocks...\n", blocks_to_process);

    // Allocate aligned buffer
    void *buffer;
    if (posix_memalign(&buffer, 512, cfg->block_size) != 0) {
        perror("posix_memalign failed");
        close(fd);
        return -1;
    }

    // Sequential or random access
    for (size_t i = 0; i < blocks_to_process; i++) {
        off_t offset;

        if (cfg->access_type == TYPE_RANDOM) {
            // Random block
            offset = (rand() % blocks_to_process) * cfg->block_size;
        } else {
            // Sequential
            offset = cfg->range_start + i * cfg->block_size;
        }

        // Seek to position
        if (lseek(fd, offset, SEEK_SET) < 0) {
            perror("lseek failed");
            continue;
        }

        // Read block
        ssize_t bytes_read = read(fd, buffer, cfg->block_size);
        if (bytes_read != (ssize_t)cfg->block_size) {
            continue;
        }

        // Search and replace
        int *int_buffer = (int *)buffer;
        size_t int_count = cfg->block_size / sizeof(int);
        int modified = 0;

        for (size_t j = 0; j < int_count; j++) {
            if (int_buffer[j] == cfg->search_value) {
                int_buffer[j] = cfg->replace_value;
                found_count++;
                modified = 1;
            }
        }

        // Write back if modified
        if (modified) {
            if (lseek(fd, offset, SEEK_SET) < 0) {
                perror("lseek for write failed");
                continue;
            }

            if (write(fd, buffer, cfg->block_size) == (ssize_t)cfg->block_size) {
                replaced_count++;
            }
        }

        if ((i + 1) % 100 == 0) {
            printf("  Progress: %zu/%zu blocks, found: %d\n",
                   i + 1, blocks_to_process, found_count);
        }
    }

    free(buffer);
    close(fd);

    printf("\nSearch completed!\n");
    printf("  Values found: %d\n", found_count);
    printf("  Blocks modified: %d\n", replaced_count);

    return found_count;
}

void print_usage(char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  --rw <read|write|search>      Access mode (default: search)\n");
    printf("  --block_size <size>           Block size in bytes (default: 4096)\n");
    printf("  --block_count <count>         Number of blocks (default: 1000)\n");
    printf("  --file <path>                 File path (default: test_data.bin)\n");
    printf("  --range <start>-<end>         Range in bytes (default: 0-0 = entire file)\n");
    printf("  --direct <on|off>             Use O_DIRECT (default: off)\n");
    printf("  --type <sequence|random>      Access type (default: sequence)\n");
    printf("  --search <value>              Value to search for (default: 12345)\n");
    printf("  --replace <value>             Value to replace with (default: 99999)\n");
    printf("  --iterations <count>          Number of iterations (default: 1)\n");
    printf("\nExamples:\n");
    printf("  %s --rw write --block_count 10000 --file data.bin\n", prog_name);
    printf("  %s --rw search --search 12345 --replace 99999 --file data.bin\n", prog_name);
}

int main(int argc, char *argv[]) {
    Config cfg = {
        .mode = MODE_SEARCH_REPLACE,
        .block_size = DEFAULT_BLOCK_SIZE,
        .block_count = DEFAULT_BLOCK_COUNT,
        .filename = "test_data.bin",
        .range_start = 0,
        .range_end = 0,
        .use_direct = 0,
        .access_type = TYPE_SEQUENTIAL,
        .search_value = 12345,
        .replace_value = 99999,
        .iterations = 1
    };

    if (argc > 1 &&
        (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_usage(argv[0]);
        return 0;
    }

    // Parse arguments
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) break;

        if (strcmp(argv[i], "--rw") == 0) {
            if (strcmp(argv[i+1], "write") == 0) cfg.mode = MODE_WRITE;
            else if (strcmp(argv[i+1], "read") == 0) cfg.mode = MODE_READ;
            else cfg.mode = MODE_SEARCH_REPLACE;
        } else if (strcmp(argv[i], "--block_size") == 0) {
            cfg.block_size = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--block_count") == 0) {
            cfg.block_count = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--file") == 0) {
            strncpy(cfg.filename, argv[i+1], sizeof(cfg.filename) - 1);
        } else if (strcmp(argv[i], "--direct") == 0) {
            cfg.use_direct = (strcmp(argv[i+1], "on") == 0);
        } else if (strcmp(argv[i], "--type") == 0) {
            cfg.access_type = (strcmp(argv[i+1], "random") == 0) ? TYPE_RANDOM : TYPE_SEQUENTIAL;
        } else if (strcmp(argv[i], "--search") == 0) {
            cfg.search_value = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--replace") == 0) {
            cfg.replace_value = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--iterations") == 0) {
            cfg.iterations = atoi(argv[i+1]);
        }
    }

    printf("========================================\n");
    printf("EMA Replace Integer - IO Loader\n");
    printf("========================================\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Execute based on mode
    if (cfg.mode == MODE_WRITE) {
        generate_file(&cfg);
    } else if (cfg.mode == MODE_SEARCH_REPLACE) {
        for (int i = 0; i < cfg.iterations; i++) {
            printf("\n--- Iteration %d/%d ---\n", i + 1, cfg.iterations);
            search_and_replace(&cfg);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("\n========================================\n");
    printf("Total execution time: %.2f ms\n", elapsed);
    printf("========================================\n");

    return 0;
}

