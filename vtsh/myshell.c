//
// Created by admin on 1/18/2026.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <time.h>
#include <errno.h>

#define MAX_ARGS 64
#define MAX_LINE 1024
#define MAX_COMMANDS 32

//Wrapper cho syscall clone3 do linux chưa có clone3 trong glibc chuẩn.
static inline pid_t my_clone3(struct clone_args *cl_args, size_t size) {
    return syscall(__NR_clone3, cl_args, size);
}

// tính time hoạt động
double time_diff_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1000000.0;
}


int parse_command(char *line, char **args) {
    int i = 0;
    char *token = strtok(line, " \t\n");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return i;
}


int execute_single_command(char **args) {
    struct timespec start, end;
    struct clone_args cl_args;
    pid_t pid;
    int status;

    if (args[0] == NULL) {
        return 0;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    memset(&cl_args, 0, sizeof(cl_args));

    cl_args.exit_signal = SIGCHLD;

    //tạo tiến trình con (proccess)
    pid = my_clone3(&cl_args, sizeof(cl_args));

    if (pid < 0) {
        perror("clone3 failed");
        return -1;
    }

    if (pid == 0) {
        extern char **environ;

        if (strchr(args[0], '/') != NULL) {
            execve(args[0], args, environ);
        } else {
            char *path_env = getenv("PATH");
            if (path_env) {
                char path_copy[4096];
                strncpy(path_copy, path_env, sizeof(path_copy) - 1);

                char *dir = strtok(path_copy, ":");
                while (dir != NULL) {
                    char full_path[1024];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, args[0]);

                    execve(full_path, args, environ);

                    dir = strtok(NULL, ":");
                }
            }

            execve(args[0], args, environ);
        }

        perror("execve failed");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &status, 0);

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed = time_diff_ms(start, end);
        printf("[Execution time: %.2f ms]\n", elapsed);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return -1;
    }
}

void execute_commands_sequential(char *line) {
    char *commands[MAX_COMMANDS];
    int cmd_count = 0;

    char *cmd = strtok(line, ";");
    while (cmd != NULL && cmd_count < MAX_COMMANDS) {
        commands[cmd_count++] = cmd;
        cmd = strtok(NULL, ";");
    }

    for (int i = 0; i < cmd_count; i++) {
        char *args[MAX_ARGS];
        char cmd_copy[MAX_LINE];

        strncpy(cmd_copy, commands[i], MAX_LINE - 1);
        cmd_copy[MAX_LINE - 1] = '\0';

        int argc = parse_command(cmd_copy, args);

        if (argc > 0) {
            printf("Executing: %s\n", args[0]);
            execute_single_command(args);
        }
    }
}

int main() {
    char line[MAX_LINE];

    printf("Simple Shell with clone3() and ';' operator\n");
    printf("===========================================\n");
    printf("Usage:\n");
    printf("  Single command: ls -la\n");
    printf("  Sequential (;): ls ; pwd ; date\n");
    printf("  Type 'exit' to quit\n\n");

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (line[0] == '\n') {
            continue;
        }

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (strchr(line, ';') != NULL) {
            execute_commands_sequential(line);
        } else {
            char *args[MAX_ARGS];
            parse_command(line, args);
            execute_single_command(args);
        }
    }

    return 0;
}
