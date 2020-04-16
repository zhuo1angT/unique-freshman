#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../wrapper.h"

bool isDir(char *path) {
    struct stat statbuf;
    stat(path, &statbuf);
    return (statbuf.st_mode & __S_IFMT) == __S_IFDIR;
}

int main(int argc, char *argv[]) {
    if (argc - 3) {
        unix_error("Error: missing args.");
    }

    FILE *src, *dst;

    char *dst_path;

    src = fopen(argv[1], "r");

    if (isDir(argv[2])) {
        // copy to the dst directory, keeping the file name.

        dst_path = (char *)malloc(sizeof(char) *
                                  (2 + strlen(argv[1]) + strlen(argv[2])));

        char *p_file_name = strrchr(argv[1], '/');

        strcpy(dst_path, argv[2]);
        strcat(dst_path, p_file_name ? p_file_name + 1 : argv[1]);

    } else {
        dst_path = (char *)malloc(sizeof(char) * (1 + strlen(argv[2])));
        strcpy(dst_path, argv[2]);
    }

    dst = fopen(dst_path, "w");

    char *buf = (char *)malloc(sizeof(char) * 1024);
    size_t bufsize = 1024;

    while (true) {
        size_t n_read;
        n_read = read(src->_fileno, buf, bufsize);

        if (n_read == 0) break;

        size_t n = n_read;
        if (write(dst->_fileno, buf, n) != n) {
            unix_error("Error: copy error!");
        }
    }

    free(buf);       // not necessary
    free(dst_path);  // not necessary

    exit(0);
}