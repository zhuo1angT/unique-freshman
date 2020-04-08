#ifndef _UNIQUE_WRAPPER_H_
#define _UNIQUE_WRAPPER_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void unix_error(char *msg) {  // unix-style error
    fprintf(stderr, "%s\n errno:%s\n", strerror(errno), msg);
    exit(0);
}

pid_t Fork() {
    pid_t pid;
    if ((pid = fork()) < 0) unix_error("Error: fork failed!");
    return pid;
}

pid_t Wait(int *status) {
    pid_t pid;
    if ((pid = wait(status)) < 0) unix_error("Error: wait failed!");
    return pid;
}

pid_t Waitpid(pid_t pid, int *status, int options) {
    pid_t ret;
    if ((ret = waitpid(pid, status, options)) < 0)
        unix_error("Error: waitpid failed!");
    return ret;
}

int Execve(char *filename, char *argv[], char *envp[]) {
    int ret;
    if ((ret = execve(filename, argv, envp)) < 0)
        unix_error("Error: execve failed!");
    return ret;
}

#endif