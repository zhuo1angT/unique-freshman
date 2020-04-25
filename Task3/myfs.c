#include "myfs.h"

#include <stdio.h>
#include <stdlib.h>

static struct fuse_operations memfs_oper;

int main(int argc, char *argv[]) {
    static struct fuse_operations myfs_oper = {

        .getattr = myfs_getattr,
        .access = myfs_access,
        .readdir = myfs_readdir,

        .open = myfs_open,
        .read = myfs_read,
        .write = myfs_write,
        .release = myfs_release,

        .mknod = myfs_mknod,
        .unlink = myfs_unlink,

        .mkdir = myfs_mkdir,
        .rmdir = myfs_rmdir,

        .statfs = myfs_statfs,
    };
    return 0;
}