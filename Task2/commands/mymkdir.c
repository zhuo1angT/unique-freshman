#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../wrapper.h"

int main(int argc, char *argv[]) {
    mode_t mode = (mode_t)0755;
    if (argc == 1) {
        unix_error("Error: missing dir arg.");
    }

    int status = 0;
    for (int i = 1; i < argc; i++) {
        status |= mkdir(argv[i], mode);
        if (status != 0) {
            fprintf(stderr, "Error: %s\n%s\n", argv[i], errno);
        }
    }
    exit(status);
}