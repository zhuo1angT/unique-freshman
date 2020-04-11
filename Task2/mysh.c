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

int pipe_fd[2];
char pipe_buf[4 * 1024];

int rplc_stdin = false, rplc_stdout = false, rplc_stderr = false;
int save_stdin, save_stdout, save_stderr;

void parse(char *cmdline);
void set_bg();
void io_redirect();
void io_reset();

int set_pipe();

void sigint_handler(int sig);

int bg = false, pid, sh_pid;

int main() {
    signal(SIGINT, sigint_handler);  // installing the "ctrl+C" handler

    sh_pid = getpid();

    printf("my-shell.\n");

    while (true) {
        cmdline = readline("\e[36;1mzhuo1ang@my-shell > \e[0m");
        if (cmdline == NULL) {
            write(1, "\n", 1);  // stdout <- "\n"
            exit(0);
        } else if (cmdline[0] == '\0') {
            continue;
        }
        parse(cmdline);
        argv[argc] = NULL;

        for (int i = 0; i < argc; i++) {
            printf("%d: %s\n", i, argv[i]);
        }

        set_bg();

        io_redirect();

        if (!set_pipe()) {
            if ((pid = fork()) == 0) {  // child process
                execve(argv[0], argv, NULL);
                exit(0);
            }

            if (!bg) {
                int status;
                Waitpid(pid, &status, 0);
            } else {
                printf("%d %s\n", pid, cmdline);
            }
        }

        // printf("%s\n", bg ? "bg" : "fg");

        free(cmdline);
        for (int i = 0; i < argc; i++) free(argv[i]);
        io_reset();
    }

    return 0;
}

void parse(char *cmdline) {
    int i = 0, j = 0;
    int word = false, match = true, len = strlen(cmdline);

    argc = 0;

    while (i < len) {
        if (cmdline[i] == '|' || cmdline[i] == '&' || cmdline[i] == '>' ||
            cmdline[i] == '<') {
            word = false;
            argc++;
            argv[argc - 1] = (char *)calloc(MAX_ARG_LENGTH, sizeof(char));
            argv[argc - 1][0] = cmdline[i++];
            j = 0;
            continue;
        }

        if (word) {
            if (cmdline[i] == ' ' && match) {
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

                argv[argc - 1] = (char *)calloc(MAX_ARG_LENGTH, sizeof(char));

                if (cmdline[i] == '\"')
                    match = false;
                else
                    argv[argc - 1][j++] = cmdline[i];
            }
            i++;
        }
    }
}

void set_bg() {
    if (strcmp(argv[argc - 1], "&") == 0)
        bg = true;
    else
        bg = false;
}

void io_redirect() {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "<") == 0) {
            if (i + 2 == argc ||
                (i + 3 == argc && strcmp(argv[argc - 1], "&"))) {
                rplc_stdin = true;
                save_stdin = dup(0);
                FILE *new_input_stream = freopen(argv[i + 1], "r", stdin);

                argv[i] = NULL;

                return;
            } else {  // Error
                // Todo: Error report
            }
        }
        if (strcmp(argv[i], ">") == 0) {
            if (i + 2 == argc ||
                (i + 3 == argc && strcmp(argv[argc - 1], "&"))) {
                rplc_stdout = true;
                save_stdout = dup(1);
                FILE *new_output_stream = freopen(argv[i + 1], "w", stdout);

                argv[i] = NULL;

                return;
            } else {  // Error
                // Todo: Error report
            }
        }
    }
}

void io_reset() {
    if (rplc_stdin) {
        dup2(save_stdin, 0);
        rplc_stdin = false;
    }
    if (rplc_stdout) {
        dup2(save_stdout, 1);
        rplc_stdout = false;
    }
    if (rplc_stderr) {
        dup2(save_stderr, 2);
        rplc_stderr = false;
    }
}

int set_pipe() {
    // only one pipe
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "|") == 0) {
            int argc1;
            char *argv1[MAX_ARGS];
            Pipe(pipe_fd);

            int pid0, pid1;

            if ((pid0 = Fork()) == 0) {
                close(pipe_fd[0]);  // close read

                dup2(pipe_fd[1], 1);

                argv[i] = NULL;
                execve(argv[0], argv, NULL);

                exit(0);
            }
            if ((pid1 = Fork()) == 0) {
                close(pipe_fd[1]);  // close write

                dup2(pipe_fd[0], 0);

                
                for (int j = i + 1; j < argc; j++) {
                    argv1[j - i - 1] = argv[j];

                    printf("child1: %d : %s\n", j - i - 1, argv1[j - i - 1]);
                }

                argv1[argc - i - 1] = NULL;
                execve(argv1[0], argv1, NULL);
                exit(0);
            }

            int status;
            Waitpid(pid1, &status, 0);

            return 1;
        }
    }
    return 0;
}

void sigint_handler(int sig) {
    write(1, "\n", 1);  // stdout <- "\n"
    Kill((pid_t)pid, 0);
    bg = true;
}