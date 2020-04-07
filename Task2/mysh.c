#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 500

void eval(char *cmd);
void sigint_handler(int sig);

char *cmdline;
int fg = false, fg_pid;

int main() {
    cmdline = (char *)malloc(sizeof(char) * MAXLINE);
    signal(SIGINT, sigint_handler);

    printf("my-shell.\n");

    while (true) {
        cmdline = readline("\e[36;1mzhuo1ang@my-shell > \e[0m");
        if (cmdline == NULL) {
            write(1, "\n", 1);  // stdout <- "\n"
            exit(0);
        }

        // temp code start

        char *argv[2];
        argv[0] = cmdline;
        argv[1] = NULL;
        if ((fg_pid = fork()) == 0) {
            execve(cmdline, argv, NULL);
        } else {
            fg = true;
            wait(NULL);
        }

        // temp code end

        // eval(cmdline);
    }

    return 0;
}

void eval(char *cmd) {}
void sigint_handler(int sig) {
    if (fg) {
        kill(fg_pid, SIGKILL);
        waitpid(fg_pid, NULL, 0);
        fg = false;
    }
}