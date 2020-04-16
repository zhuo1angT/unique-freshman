#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

extern char *optarg;
extern int optind;
int opterr;
int optopt;

char **file_names;

int capacity = 0, cnt = 0;

int cmp(void *a, void *b) {
    char *_a = *(char **)a;
    char *_b = *(char **)b;

    int len_a = strlen(_a), len_b = strlen(_b);

    for (int i = 0; i < len_a && i < len_b; i++) {
        if (isalpha(_a[i]) && isalpha(_b[i])) {
            int ord_a = (_a[i] <= 'Z') ? _a[i] - 'A' : _a[i] - 'a';
            int ord_b = (_b[i] <= 'Z') ? _b[i] - 'A' : _b[i] - 'a';
            if (ord_a < ord_b) return -1;
            if (ord_a > ord_b) return 1;
        } else if (_a[i] < _b[i])
            return -1;
        else if (_a[i] > _b[i])
            return 1;
    }
    if (len_a == len_b)
        return 0;
    else
        return len_a < len_b ? -1 : 1;
}

int main(int argc, char *argv[]) {
    int ch;
    int aflag = false, lflag = false;
    while ((ch = getopt(argc, argv, "al")) != -1) {
        switch (ch) {
            case 'a':
                aflag = true;
                break;
            case 'l':
                lflag = true;
                break;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                exit(1);
        }
    }
    // printf("%s %s\n", aflag ? "a" : "no-a", lflag ? "l" : "no-l");

    DIR *dir;
    struct dirent *dirp;
    if ((dir = opendir(".")) == NULL) {  // executable but not readable dir..
        fprintf(stderr, "Open directory failed.\n");
        exit(1);
    }

    while ((dirp = readdir(dir)) != NULL) {  // cout << dirp->d_name << endl;
        if (aflag || dirp->d_name[0] != '.') {
            // printf("%s\n", dirp->d_name);
            if (cnt == capacity) {
                if (capacity)
                    capacity <<= 1;
                else
                    capacity++;

                file_names =
                    (char **)realloc(file_names, sizeof(char *) * capacity);
            }
            file_names[cnt++] = dirp->d_name;
        }
    }

    qsort(file_names, cnt, sizeof(char *), cmp);

    if (!lflag) {
        for (int i = 0; i < cnt; i++) {
            printf("%s  ", file_names[i]);
        }
        puts("");
    } else {
        for (int i = 0; i < cnt; i++) {
            struct stat file_status;
            stat(file_names[i], &file_status);
            if (S_ISBLK(file_status.st_mode))
                putchar('b');
            else if (S_ISCHR(file_status.st_mode))
                putchar('c');
            else if (S_ISDIR(file_status.st_mode))
                putchar('d');
            else if (S_ISFIFO(file_status.st_mode))
                putchar('f');
            else if (S_ISLNK(file_status.st_mode))
                putchar('1');
            else if (S_ISREG(file_status.st_mode))
                putchar('-');
            else if (S_ISSOCK(file_status.st_mode))
                putchar('s');

            char mod[10] = "---------";
            if (file_status.st_mode & S_IRUSR) mod[0] = 'r';
            if (file_status.st_mode & S_IWUSR) mod[1] = 'w';
            if (file_status.st_mode & S_IXUSR) mod[2] = 'x';
            if (file_status.st_mode & S_IRGRP) mod[3] = 'r';
            if (file_status.st_mode & S_IWGRP) mod[4] = 'w';
            if (file_status.st_mode & S_IXGRP) mod[5] = 'x';
            if (file_status.st_mode & S_IROTH) mod[6] = 'r';
            if (file_status.st_mode & S_IWOTH) mod[7] = 'w';
            if (file_status.st_mode & S_IXOTH) mod[8] = 'x';
            printf("%s %d ", mod, file_status.st_nlink);

            struct passwd *user = getpwuid(file_status.st_uid);
            struct group *group = getgrgid(file_status.st_gid);
            printf("%s %s", user->pw_name, group->gr_name);

            printf("%*d", 8, file_status.st_size);

            struct tm *local_time = localtime(&file_status.st_mtime);
            printf(" %2d月%3d%3d:%02d", local_time->tm_mon + 1,
                   local_time->tm_mday, local_time->tm_hour,
                   local_time->tm_sec);

            printf(" %s\n", file_names[i]);
        }
    }
    closedir(dir);
    exit(0);
}