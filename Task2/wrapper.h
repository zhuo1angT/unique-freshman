#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void unix_error(char *msg) {  // unix-style error
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

pid_t Fork() {
    pid_t pid;
    if ((pid = fork()) < 0) unix_error("Error: Fork failed!");
    return pid;
}

pid_t wait(int *status) {
    pid_t pid;
    if ((pid = wait(status)) < 0) unix_error("wait failed: ");
    return pid;
}

pid_t Waitpid(pid_t pid, int *status, int options) {
    pid_t ret;
    if ((ret = waitpid(pid, status, options)) < 0)
        unix_error("waitpid failed: ");
    return ret;
}

int Execve(char *filename, char *argv[], char *envp[]) {
    int ret;
    if ((ret = execve(filename, argv, envp)) < 0) unix_error("execve failed: ");
    return ret;
}