#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../wrapper.h"

#define MAX_ARGS 50

int main(int argc, char *argv[]) {
    if (argc == 1) {
        unix_error("Error: missing command.");
    }

    char *buf = (char *)malloc(sizeof(char) * 1024);
    size_t bufsize = 1024;

    size_t n_read = read(STDIN_FILENO, buf, bufsize);

    int exec_argc = 0;
    char *exec_argv[50];

    exec_argv[exec_argc] = (char *)malloc(sizeof(char) * (1 + strlen(argv[1])));
    strcpy(exec_argv[exec_argc++], argv[1]);
    int word = false, st = 0;
    for (int i = 0; i < n_read; i++) {
        if (word) {
            if (buf[i] == ' ' || !isprint(buf[i])) {
                word = false;
                exec_argv[exec_argc] =
                    (char *)malloc(sizeof(char) * (1 + i - st));
                strncpy(exec_argv[exec_argc], (char *)(buf + st), i - st);
                exec_argv[exec_argc++][i - st] = '\0';
            }
        } else if (buf[i] != ' ' && isprint(buf[i])) {
            word = true;
            st = i;
        }
    }
    if (word) {
        exec_argv[exec_argc] = (char *)malloc(sizeof(char) * (1 + n_read - st));
        strncpy(exec_argv[exec_argc], (char *)(buf + st), n_read - st);
        exec_argv[exec_argc++][n_read - st] = '\0';
    }

    // printf("argc = %d\n", exec_argc);
    // for (int i = 0; i < exec_argc; i++) printf("%d: %s\n", i, exec_argv[i]);

    execve(exec_argv[0], exec_argv, NULL);

    exit(0);
}