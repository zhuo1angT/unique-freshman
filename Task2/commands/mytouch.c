#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MODE_RW_UGO (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main(int argc, char *argv[]) {
    struct timespec newtime[2];
    newtime[0].tv_nsec = UTIME_NOW;
    newtime[1].tv_nsec = UTIME_NOW;
    for (int i = 1; i < argc; i++) {
        open(argv[i], O_CREAT | O_WRONLY, MODE_RW_UGO);
        utimensat(AT_FDCWD, argv[i], newtime, 0);
    }
    return 0;
}
