#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../wrapper.h"

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    char *buf = (char *)malloc(sizeof(char) * 1024);
    size_t bufsize = 1024;

    size_t n_read;

    while (true) {
        n_read = read(file->_fileno, buf, bufsize);

        if (n_read == 0) break;

        size_t n = n_read;
        if (write(STDOUT_FILENO, buf, n) != n) {
            unix_error("Error: write error!");
        }
    }
    free(buf);  // not necessary

    exit(0);
}
