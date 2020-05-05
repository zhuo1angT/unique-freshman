#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myfs.h"

extern char lower_dir_path[PATH_MAX];
extern char upper_dir_path[PATH_MAX];
extern char merge_dir_path[PATH_MAX];

static struct fuse_operations myfs_oper;

int main(int argc, char *argv[]) {
    strncpy(merge_dir_path, argv[1], PATH_MAX);
    strncpy(lower_dir_path, argv[2], PATH_MAX);
    strncpy(upper_dir_path, argv[3], PATH_MAX);
    argv[2] = NULL;

    printf("lower : %s\n", lower_dir_path);
    printf("upper : %s\n", upper_dir_path);

    static struct fuse_operations myfs_oper = {
        .getattr = myfs_getattr,

        .access = myfs_access,
        .readdir = myfs_readdir,
        .create = myfs_create,

        .open = myfs_open,
        .read = myfs_read,
        .write = myfs_write,
        //.release = myfs_release,

        //.mknod = myfs_mknod,
        .unlink = myfs_unlink,

        .mkdir = myfs_mkdir,
        // .rmdir = myfs_rmdir,
        //.statfs = myfs_statfs,

        .truncate = myfs_truncate,

    };

    fuse_main(/* argc */ 2, argv, &myfs_oper, NULL);
}