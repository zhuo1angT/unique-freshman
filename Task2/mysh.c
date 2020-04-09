#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"

#define MAX_LINE 500
#define MAX_ARGS 50
#define MAX_ARG_LENGTH 50
#define MAX_ENVP_LENGTH 100

char *cmdline;

int argc;
char *argv[MAX_ARGS], *envp[MAX_ARGS];
char cmd_buffer[MAX_LINE];

void parse(char *cmdline);
void sigint_handler(int sig);

int bg = false, fg_pid, sh_pid;

int main() {
    signal(SIGINT, sigint_handler);  // installing the "ctrl+C" handler

    sh_pid = getpid();

    printf("my-shell.\n");

    while (true) {
        cmdline = readline("\e[36;1mzhuo1ang@my-shell > \e[0m");
        if (cmdline == NULL) {
            write(1, "\n", 1);  // stdout <- "\n"
            exit(0);
        }

        parse(cmdline);

        for (int i = 0; i < argc; i++) {
            printf("%d: %s\n", i, argv[i]);
        }

        argv[argc] = NULL;

        if ((fg_pid = fork()) == 0) {
            execve(cmdline, argv, NULL);
        } else {
            bg = false;
            waitpid(fg_pid, NULL, 0);
        }

        // eval(cmdline);

        free(cmdline);
    }

    return 0;
}

void parse(char *cmdline) {
    int i = 0, j = 0;
    int word = false, match = true, len = strlen(cmdline);

    argc = 0;

    while (i < len) {
        if (word) {
            if ((cmdline[i] == ' ' || cmdline[i] == '>' || cmdline[i] == '<' ||
                 cmdline[i] == '|' || cmdline[i] == '&') &&
                match) {
                word = false;
                argv[argc - 1][j++] = '\0';
                i++;
                j = 0;
                continue;
            }
            if (cmdline[i] == '\"' && !match) {
                match = true;
                i++;
                continue;
            }
            if (cmdline[i] == '\"' && match) {
                i++;
                continue;
            }
            argv[argc - 1][j++] = cmdline[i++];
        } else {
            if (cmdline[i] != ' ') {
                word = true;
                argc++;

                argv[argc - 1] = (char *)malloc(sizeof(char) * MAX_ARG_LENGTH);

                if (cmdline[i] == '\"')
                    match = false;
                else
                    argv[argc - 1][j++] = cmdline[i];
            }
            i++;
        }
    }
}

void sigint_handler(int sig) {
    write(1, "\n", 1);  // stdout <- "\n"
    kill(fg_pid, 0);
    bg = true;
}