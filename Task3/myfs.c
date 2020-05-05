#define FUSE_USE_VERSION 31
#define _GNU_SOURCE

#include "myfs.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char lower_dir_path[PATH_MAX];
char upper_dir_path[PATH_MAX];
char merge_dir_path[PATH_MAX];

char fpath[PATH_MAX];

char file_path[PATH_MAX];

void log_fpath() {
    for (int i = 0; i < 5; i++) puts("");
    printf("DEBUG:: %s\n", fpath);
    for (int i = 0; i < 5; i++) puts("");
}

void set_fpath(const char* path, int layer) {
    if (layer == LOWER_LAYER)
        strncpy(fpath, lower_dir_path, PATH_MAX);
    else if (layer == UPPER_LAYER)
        strncpy(fpath, upper_dir_path, PATH_MAX);

    strncpy(fpath + strlen(fpath), path, PATH_MAX);
}

int myfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info* fi) {
    printf("lower_path :: %s\n", lower_dir_path);

    set_fpath(path, LOWER_LAYER);

    DIR* dp = opendir(fpath);

    struct dirent* de;

    if (dp != NULL) {
        while ((de = readdir(dp)) != NULL) {
            set_fpath(path, UPPER_LAYER);
            strncpy(file_path, fpath, PATH_MAX);
            strncpy(file_path + strlen(file_path), de->d_name,
                    strlen(de->d_name) + 1);

            if (access(file_path, F_OK) != 0) {
                struct stat st;
                memset(&st, 0, sizeof(st));
                st.st_ino = de->d_ino;
                st.st_mode = de->d_type << 12;

                printf("filename:: lower : %s\n", de->d_name);

                if (filler(buf, de->d_name, &st, 0, 0)) break;
            }
        }
    }

    printf("lower_path :: %s\n", lower_dir_path);

    set_fpath(path, UPPER_LAYER);

    dp = opendir(fpath);

    int cnt = 0;

    if (dp != NULL) {
        while ((de = readdir(dp)) != NULL) {
            printf("lower_path :: %s\n", lower_dir_path);
            struct stat st;
            memset(&st, 0, sizeof(st));
            st.st_ino = de->d_ino;
            st.st_mode = de->d_type << 12;

            printf("filename:: upper : %s\n", de->d_name);

            if (filler(buf, de->d_name, &st, 0, 0)) break;
        }
    }

    closedir(dp);

    printf("lower_path :: %s\n", lower_dir_path);

    return 0;
}

int myfs_getattr(const char* path, struct stat* st) {
    int ret;
    memset(st, 0, sizeof(struct stat));

    set_fpath(path, UPPER_LAYER);
    if (!access(fpath, F_OK)) {
        ret = lstat(fpath, st);
    } else {
        set_fpath(path, LOWER_LAYER);
        ret = lstat(fpath, st);
    }

    if (ret == -1) return -errno;

    return ret;
}

/*
int myfs_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
    int ret;

    // set_fpath(path); // temp

    ret = open(fpath, fi->flags, mode);
    if (ret == -1) return -errno;

    fi->fh = ret;
    return 0;
}
*/

int myfs_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
    int ret;

    set_fpath(path, UPPER_LAYER);

    for (int i = strlen(fpath) - 1; i >= 0; i--) {
        if (fpath[i] == '/') {
            fpath[i] = '\0';  // teminates the fpath here
            break;
        }
    }
    log_fpath();

    if (!access(fpath, F_OK)) {
        set_fpath(path, UPPER_LAYER);

        ret = open(fpath, fi->flags, mode);
        if (ret == -1) return -errno;
        fi->fh = ret;
        return 0;
    } else {
        // the required path is not in the upper_layer,
        // so the create operation can not be done.
        return 1;
    }
}

/*
int myfs_unlink(const char* path) {
    int ret;

    // set_fpath(path); //temp

    ret = unlink(fpath);
    if (ret == -1) return -errno;

    return 0;
}
*/

int myfs_unlink(const char* path) {
    int ret;

    set_fpath(path, UPPER_LAYER);

    ret = unlink(fpath);
    if (ret == -1) return -errno;
    return 0;
}

int myfs_mkdir(const char* path, mode_t mode) {
    int ret;

    set_fpath(path, UPPER_LAYER);

    ret = mkdir(fpath, mode);
    if (ret == -1) return -errno;
    return 0;
}

int myfs_access(const char* path, int mask) {
    int ret;

    set_fpath(path, UPPER_LAYER);

    if (access(fpath, F_OK) == 0) {
        ret = access(fpath, mask);
    } else {
        set_fpath(path, LOWER_LAYER);
        if (access(fpath, F_OK) == 0) {
            ret = access(fpath, mask);
        }
    }

    if (ret == -1) return -errno;
    return 0;
}

int myfs_open(const char* path, struct fuse_file_info* fi) {
    int ret;

    set_fpath(path, UPPER_LAYER);

    if (access(fpath, F_OK) == 0) {
        ret = open(fpath, fi->flags);
    } else {
        set_fpath(path, LOWER_LAYER);
        if (access(fpath, F_OK) == 0) {
            ret = open(fpath, fi->flags);
        }
    }

    if (ret == -1) return -errno;

    fi->fh = ret;
    return 0;
}

int myfs_read(const char* path, char* buf, size_t size, off_t offset,
              struct fuse_file_info* fi) {
    int fd;
    int ret;

    char* file = NULL;
    set_fpath(path, UPPER_LAYER);
    if (access(fpath, F_OK) == 0) {
        file = fpath;
    } else {
        set_fpath(path, LOWER_LAYER);
        if (access(fpath, F_OK) == 0) {
            file = fpath;
        }
    }

    fd = open(file, O_RDONLY);

    printf("fd = %d\n", fd);

    if (fd == -1) return -errno;

    ret = pread(fd, buf, size, offset);
    if (ret == -1) ret = -errno;

    if (fi == NULL) close(fd);
    return ret;
}

int myfs_write(const char* path, const char* buf, size_t size, off_t offset,
               struct fuse_file_info* fi) {
    int fd;
    int ret;

    char* file = NULL;

    set_fpath(path, UPPER_LAYER);
    if (access(fpath, F_OK) == 0) {
        file = fpath;
    } else {
        set_fpath(path, LOWER_LAYER);
        if (access(fpath, F_OK) == 0) file = fpath;
    }

    fd = open(file, O_WRONLY);

    if (fd == -1) return -errno;

    ret = pwrite(fd, buf, size, offset);
    if (ret == -1) ret = -errno;

    close(fd);
    return ret;
}

int myfs_truncate(const char* path, off_t size) {
    int ret;

    set_fpath(path, UPPER_LAYER);
    if (access(fpath, F_OK) == 0)
        ret = truncate(fpath, size);
    else {
        set_fpath(path, LOWER_LAYER);
        if (access(fpath, F_OK) == 0) ret = truncate(fpath, size);
    }

    if (ret == -1) return -errno;

    return 0;
}