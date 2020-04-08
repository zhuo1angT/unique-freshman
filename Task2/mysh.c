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

char *cmdline;
char *argc[MAX_ARGS], *envp[MAX_ARGS];
char cmd_buffer[MAX_LINE];

char echo_buf[MAX_LINE];

void eval(char *cmdline);
void sigint_handler(int sig);

char *echo(const char str[]) {
    int i = 0, j = 0;
    int word = false, match = true, len = strlen(str);

    while (i < len) {
        if (word) {
            bool pass = false;
            if (str[i] == ' ' && match) {
                word = false;
                echo_buf[j++] = ' ';
                i++;
                continue;
            }
            if (str[i] == '\"' && !match) {
                match = true;
                i++;
                continue;
            }
            if (str[i] == '\"' && match) {
                i++;
                continue;
            }
            echo_buf[j++] = str[i++];
        } else {
            if (str[i] != ' ') {
                word = true;
                if (str[i] == '\"')
                    match = false;
                else
                    echo_buf[j++] = str[i];
            }
            i++;
        }
    }
    echo_buf[j] = '\0';
    return echo_buf;
}

void echo_test() {
    // passed
    printf("%s\n", echo("  foo      bar          baz "));
    printf("%s\n", echo("foobar"));
    printf("%s\n", echo("     foo "));
    printf("%s\n", echo("  \"foo\""));
    printf("%s\n", echo("foo bar \"baz \"   foo   "));
    printf("%s\n", echo("   \"foo\"\"bar\"    "));
    printf("%s\n", echo("\"foo\"\"bar\"\"baz\""));
    printf("%s\n", echo("\"foo\" \"bar\" \"baz\""));
}

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
    kill(fg_pid, 0);
    fg = false;
}