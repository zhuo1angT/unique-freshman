#define FUSE_USE_VERSION 31
#define _GNU_SOURCE

#include "myfs.h"

int myfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info* fi) {
    return filler(buf, "Hello, World\n", NULL, 0, 0);
}

int myfs_getattr(const char* path, struct stat* st) {
    memset(st, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0)
        st->st_mode = 0755 | S_IFDIR;
    else
        st->st_mode = 0644 | S_IFREG;

    return 0;
}
