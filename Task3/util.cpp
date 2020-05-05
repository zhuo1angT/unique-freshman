#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <map>
#include <vector>

#include "myfs.h"

using std::map;
using std::vector;

/*
 * marco reminder
 *
 * #define LOWER_LAYER 0
 * #define UPPER_LAYER 1
 *
 * #define PATH_MAX 256
 */

struct my_inode {
    uint32_t inode_id;
    uint32_t f_type;  // file, directory, link, ...
    bool layer;       // bool, actually
    char pad[3];      // padding bytes
    char name_with_fullpath[PATH_MAX];
};

vector<struct my_inode*> inodes;

int regi_my_inode(char* path, int layer, int file_type,
                  struct my_inode* inode) {
    inode = (struct my_inode*)malloc(sizeof(struct my_inode));
    inode->inode_id = inodes.size() + 1;

    // get f_type ...

    inode->layer = layer;
    strncpy(inode->name_with_fullpath, path, PATH_MAX);

    inodes.push_back(inode);

    return 0;
}