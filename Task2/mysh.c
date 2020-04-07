#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 500
#define MAX_ARGS 50

void eval(char *cmdline);
void sigint_handler(int sig);

char *cmdline;
char *argc[MAX_ARGS], *envp[MAX_ARGS];
char cmd_buffer[MAX_LINE];

int fg = false, fg_pid, sh_pid;

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

        // temp code start

        char *argv[2] = {};
        argv[0] = cmdline;
        argv[1] = NULL;
        if ((fg_pid = fork()) == 0) {
            execve(cmdline, argv, NULL);
        } else {
            fg = true;
            printf("pid = %d\n", fg_pid);
            waitpid(fg_pid, NULL, 0);
        }

        // temp code end

        // eval(cmdline);

        free(cmdline);
    }

    return 0;
}

void eval(char *cmdline) {}
void sigint_handler(int sig) {
    write(1, "\n", 1);  // stdout <- "\n"
    kill(fg_pid, 0);    // kill(fg_pid, 0)     // to fix
    fg = false;
}