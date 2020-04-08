#ifndef UNIQUE_CMD_H_
#define UNIQUE_CMD_H_

#include <stdio.h>

#define MAX_LINE 500
#define MAX_ARGS 50

char echo_buf[MAX_LINE];

char *myEcho(const char str[]) {
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

void myExit() { exit(0); }

void echo_test() {
    // passed
    printf("%s\n", myEcho("  foo      bar          baz "));
    printf("%s\n", myEcho("foobar"));
    printf("%s\n", myEcho("     foo "));
    printf("%s\n", myEcho("  \"foo\""));
    printf("%s\n", myEcho("foo bar \"baz \"   foo   "));
    printf("%s\n", myEcho("   \"foo\"\"bar\"    "));
    printf("%s\n", myEcho("\"foo\"\"bar\"\"baz\""));
    printf("%s\n", myEcho("\"foo\" \"bar\" \"baz\""));
}

#endif