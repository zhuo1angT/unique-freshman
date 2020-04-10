#ifndef _UNIQUE_BUILTIN_H_
#define _UNIQUE_BUILTIN_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "wrapper.h"

#define MAX_LINE 500
#define MAX_ARGS 50

char echo_buf[MAX_LINE];
char pwd_buf[MAX_LINE];

void myEcho(const char str[], FILE* outstream) {
    int i = 0, j = 0;
    int word = false, match = true, len = strlen(str);

    while (i < len) {
        if (word) {
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

    // return echo_buf;
}

void myExit() { exit(0); }

void myCd(char target[]) {
    if (chdir(target) == -1) {
        unix_error("Error: cd failed\n");
    }
}

void myPwd() {
    if (getcwd(pwd_buf, MAX_LINE) == NULL) {
        unix_error("Error: pwd failed\n");
    }
    printf("%s\n", pwd_buf);
}

void myKill(int pid) {
    if (pid != 0) Kill(pid, SIGTERM);
}

void myExport(char* argv[]) {
    char *ptr0 = argv[1], *ptr1 = argv[1];
    while (*ptr1 != '=') ptr1++;
    *ptr1 = '\0';
    setenv(ptr0, ptr1, 1);
}

void echo_test() {
    // passed
    myEcho("  foo      bar          baz ", stdout);
    myEcho("foobar", stdout);
    myEcho("     foo ", stdout);
    myEcho("  \"foo\"", stdout);
    myEcho("foo bar \"baz \"   foo   ", stdout);
    myEcho("   \"foo\"\"bar\"    ", stdout);
    myEcho("\"foo\"\"bar\"\"baz\"", stdout);
    myEcho("\"foo\" \"bar\" \"baz\"", stdout);
}

#endif