#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#include "../wrapper.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        unix_error("Error: missing args.");
    }

    int status = 0;

    for (int i = 1; i < argc; i++) {
        status |= remove(argv[i]);
        if (status) {
            unix_error("Error: remove failed.");
        }
    }

    exit(status);  // more specific, 0
}
