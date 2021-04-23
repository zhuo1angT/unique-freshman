#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "builtin.h"

#define MAX_LINE 1024
#define MAX_ARGS 256
#define MAX_ARG_LENGTH 64
#define MAX_ENVP_LENGTH 64

char *cmdline;

int argc;
char *argv[MAX_ARGS], *envp[MAX_ARGS];
char cmd_buffer[MAX_LINE];

char prompt[MAX_LINE];

FILE *input_file, *output_file;
int input_fd = 0, output_fd = 1;

int pipe_fd[2];
char pipe_buf[4 * 1024];

int rplc_stdin = false, rplc_stdout = false, rplc_stderr = false;
int save_stdin, save_stdout, save_stderr;

void parse(char *cmdline);
void set_bg();
int io_redirect();
void io_reset();

int set_pipe();

void sigint_handler(int sig);

int bg = false, pid, sh_pid;

int main() {
  signal(SIGINT, sigint_handler);  // installing the "ctrl+C" handler

  sh_pid = getpid();

  printf("shell, by zhuo1ang.\n");

  while (true) {
    sprintf(prompt, "\e[36;1m%s@my-shell > \e[0m", getlogin());
    cmdline = readline(prompt);
    if (cmdline == NULL) {
      write(1, "\n", 1);  // stdout <- "\n"
      exit(0);
    } else if (cmdline[0] == '\0') {
      continue;
    }

    add_history(cmdline);

    parse(cmdline);
    argv[argc] = NULL;

    set_bg();

    if (io_redirect()) goto exec;

    if (!set_pipe()) {
    exec:

      if (is_builtin(argv[0])) {
        exec_builtin(argv[0], argv, NULL);
      } else {
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
    }
    free(cmdline);
    if (input_fd != 0 || output_fd != 1) argc += 2;
    for (int i = 0; i < argc; i++) {
      free(argv[i]);
    }
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
  if (strcmp(argv[argc - 1], "&") == 0) {
    bg = true;
    free(argv[argc - 1]);
    argv[argc - 1] = NULL;
    argc--;
  } else
    bg = false;
}

int io_redirect() {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "<") == 0) {
      if (i + 2 == argc ||
          (i + 3 == argc && strcmp(argv[argc - 1], "&") == 0)) {
        if (!is_builtin(argv[0])) {
          save_stdin = dup(0);
          FILE *new_input_stream = freopen(argv[i + 1], "r", stdin);
        } else {
          input_file = fopen(argv[i + 1], "r");
          input_fd = input_file->_fileno;
        }
        rplc_stdin = true;
        argv[i] = NULL;
        argc = i;
        return true;

      } else {
        // Todo: Error report
      }
    }
    if (strcmp(argv[i], ">") == 0) {
      if (i + 2 == argc ||
          (i + 3 == argc && strcmp(argv[argc - 1], "&") == 0)) {
        if (!is_builtin(argv[0])) {
          save_stdout = dup(1);
          FILE *new_output_stream = freopen(argv[i + 1], "w", stdout);
        } else {
          output_file = fopen(argv[i + 1], "w");
          output_fd = output_file->_fileno;
        }
        rplc_stdout = true;
        argv[i] = NULL;
        argc = i;
        return true;

      } else {
        // Todo: Error report
      }
    }
  }
  return false;
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

  if (input_file && input_file->_fileno != 0) fclose(input_file);
  if (input_file && output_file->_fileno != 1) fclose(output_file);
  input_fd = 0;
  output_fd = 1;
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
        argv[i] = NULL;
        if (!is_builtin(argv[0])) {
          dup2(pipe_fd[1], 1);

          execve(argv[0], argv, NULL);
        } else {
          output_fd = pipe_fd[1];

          exec_builtin(argv[0], argv, NULL);
          exit(0);
        }
      }
      if ((pid1 = Fork()) == 0) {
        close(pipe_fd[1]);  // close write

        for (int j = i + 1; j < argc; j++) {
          argv1[j - i - 1] = argv[j];
        }
        argv1[argc - i - 1] = NULL;

        // dup2(pipe_fd[0], 0);

        // temp code start
        int _size_read = read(pipe_fd[0], pipe_buf, 4 * 1024);
        FILE *pipe_file = fopen("pipe_buf_file", "w");

        for (int i = 0; i < _size_read; i++)
          fprintf(pipe_file, "%c", pipe_buf[i]);

        fclose(pipe_file);

        pipe_file = fopen("pipe_buf_file", "r");
        // temp code end

        if (!is_builtin(argv1[0])) {
          dup2(pipe_file->_fileno, 0);
          execve(argv1[0], argv1, NULL);
        } else {
          input_fd = pipe_fd[0];
          exec_builtin(argv[0], argv, NULL);
          exit(0);
        }
      }

      int status;

      Waitpid(pid0, &status, 0);

      Waitpid(pid1, &status, 0);

      remove("pipe_buf_file");

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