#ifndef _UNIQUE_FS_H_
#define _UNIQUE_FS_H_

#define FUSE_USE_VERSION 31

#define PATH_MAX 256
#define LOWER_LAYER 0
#define UPPER_LAYER 1

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <utime.h>

// Get file attributes.
int myfs_getattr(const char *, struct stat *);

// Read the target of a symbolic link
int myfs_readlink(const char *, char *, size_t);

// Create a file node
int myfs_mknod(const char *, mode_t, dev_t);

// Create a directory
int myfs_mkdir(const char *, mode_t);

// Remove a file
int myfs_unlink(const char *);

// Remove a directory
int myfs_rmdir(const char *);

// Create a symbolic link
int myfs_symlink(const char *, const char *);

// Rename a file
int myfs_rename(const char *, const char *);

// Create a hard link to a file
int myfs_link(const char *, const char *);

// Change the permission bits of a file
int myfs_chmod(const char *, mode_t);

// Change the owner and group of a file
int myfs_chown(const char *, uid_t, gid_t);

// Change the size of a file
int myfs_truncate(const char *, off_t);

// Change the access and / or modification times of a file
int myfs_utime(const char *, struct utimbuf *);

// File open operation
int myfs_open(const char *, struct fuse_file_info *);

// Read data from an open file
int myfs_read(const char *, char *, size_t, off_t, struct fuse_file_info *);

// Write data to an open file
int myfs_write(const char *, const char *, size_t, off_t,
               struct fuse_file_info *);
// Get file system statistics

int myfs_statfs(const char *, struct statvfs *);
// Possibly flush cached data

int myfs_flush(const char *, struct fuse_file_info *);
// Release an open file

int myfs_release(const char *, struct fuse_file_info *);
// Synchronize file contents

int myfs_fsync(const char *, int, struct fuse_file_info *);
// Set extended attributes

int myfs_setxattr(const char *, const char *, const char *, size_t, int);
// Get extended attributes

int myfs_getxattr(const char *, const char *, char *, size_t);
// List extended attributes

int myfs_listxattr(const char *, char *, size_t);
// Remove extended attributes

int myfs_removexattr(const char *, const char *);
// Open directory

int myfs_opendir(const char *, struct fuse_file_info *);

// Read directory
int myfs_readdir(const char *, void *, fuse_fill_dir_t, off_t,
                 struct fuse_file_info *);

// Release directory
int myfs_releasedir(const char *, struct fuse_file_info *);

// Synchronize directory contents
int myfs_fsyncdir(const char *, int, struct fuse_file_info *);

// Initialize filesystem
void *myfs_init(struct fuse_conn_info *conn);

// Clean up filesystem
void myfs_destroy(void *);

// Check file access permissions
int myfs_access(const char *, int);

// Create and open a file
int myfs_create(const char *, mode_t, struct fuse_file_info *);

// Change the size of an open file
int myfs_ftruncate(const char *, off_t, struct fuse_file_info *);

// Get attributes from an open file
int myfs_fgetattr(const char *, struct stat *, struct fuse_file_info *);

// Perform POSIX file locking operation
int myfs_lock(const char *, struct fuse_file_info *, int cmd, struct flock *);

// Change the access and modification times of a file with
// nanosecond resolution
int myfs_utimens(const char *, const struct timespec tv[2]);

// Map block index within file to block index within device
int myfs_bmap(const char *, size_t blocksize, uint64_t *idx);

// Ioctl
int myfs_ioctl(const char *, int cmd, void *arg, struct fuse_file_info *,
               unsigned int flags, void *data);

// Poll for IO readiness events
int myfs_poll(const char *, struct fuse_file_info *, struct fuse_pollhandle *ph,
              unsigned *reventsp);

// Write contents of buffer to an open file
int myfs_write_buf(const char *, struct fuse_bufvec *buf, off_t off,
                   struct fuse_file_info *);

// Store data from an open file in a buffer
int myfs_read_buf(const char *, struct fuse_bufvec **bufp, size_t size,
                  off_t off, struct fuse_file_info *);

// Perform BSD file locking operation
int myfs_flock(const char *, struct fuse_file_info *, int op);

// Allocates space for an open file
int myfs_fallocate(const char *, int, off_t, off_t, struct fuse_file_info *);

#endif