#ifndef _UNIQUE_BUILTIN_H_
#define _UNIQUE_BUILTIN_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "wrapper.h"

#define MAX_LINE 1024
#define MAX_ARGS 256

char echo_buf[MAX_LINE];
char pwd_buf[MAX_LINE];

extern int argc;
extern char *argv[];
extern int input_fd, output_fd;

extern FILE *input_file, *output_file;

bool is_builtin(char cmd[]);
void exec_builtin(char cmd[], char *argv[], char *envp);

void myEcho(char *argv[]) {
  for (int i = 1; i < argc; i++) {
    write(output_fd, argv[i], strlen(argv[i]));
    write(output_fd, " ", 1);
  }
  write(output_fd, "\n", 1);
}

void myExit() { exit(0); }

void myCd(const char *target_dir) {
  if (chdir(target_dir) == -1) {
    unix_error("Error: cd failed\n");
  }
}

void myPwd() {
  if (getcwd(pwd_buf, MAX_LINE) == NULL) {
    unix_error("Error: pwd failed\n");
  }
  if (output_fd == 1)
    printf("%s\n", pwd_buf);
  else
    write(output_fd, pwd_buf, strlen(pwd_buf));
}

void myKill(int pid) {
  if (pid != 0)
    Kill(pid, SIGTERM);
}

void myExport(char *argv[]) {
  char *ptr0 = argv[1], *ptr1 = argv[1];
  while (*ptr1 != '=')
    ptr1++;
  *ptr1 = '\0';
  setenv(ptr0, ptr1, 1);
}

bool is_builtin(char cmd[]) {
  if (strcmp(cmd, "echo") == 0 || strcmp(cmd, "exit") == 0 ||
      strcmp(cmd, "cd") == 0 || strcmp(cmd, "pwd") == 0 ||
      strcmp(cmd, "kill") == 0 || strcmp(cmd, "export") == 0) {
    return true;
  } else
    return false;
}

void exec_builtin(char cmd[], char *argv[], char *envp) {
  if (strcmp(cmd, "echo") == 0) {
    myEcho(argv);
  } else if (strcmp(cmd, "exit") == 0) {
    myExit();
  } else if (strcmp(cmd, "cd") == 0) {
    myCd(argv[1]);
  } else if (strcmp(cmd, "pwd") == 0) {
    myPwd();
  } else if (strcmp(cmd, "kill") == 0) {
    myKill(atoi(argv[1]));
  } else if (strcmp(cmd, "export") == 0) {
    myExport(argv);
  }
}

/*
void echo_test() {
    // passed
    myEcho("  foo      bar          baz ");
    myEcho("foobar");
    myEcho("     foo ");
    myEcho("  \"foo\"");
    myEcho("foo bar \"baz \"   foo   ");
    myEcho("   \"foo\"\"bar\"    ");
    myEcho("\"foo\"\"bar\"\"baz\"");
    myEcho("\"foo\" \"bar\" \"baz\"");
}
*/

#endif