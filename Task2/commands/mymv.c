#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#include "../wrapper.h"

int main(int argc, char *argv[]) {
    if (argc - 3) {
        unix_error("Error: missing args.");
    }

    int status = 0;

    if ((status = rename(argv[1], argv[2]) < 0)) {
        unix_error("Error: moving file failed.");
    }

    exit(status);  // more specific, 0
}
