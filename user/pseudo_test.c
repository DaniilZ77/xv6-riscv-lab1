#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argv, char* argc[]) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        fprintf(2, "failed to open file\n");
        exit(1);
    }

    char buf[100];
    if (read(fd, buf, 100) < 0) {
        close(fd);
        fprintf(2, "failed to read from file\n");
        exit(1);
    }
    if (close(fd) < 0) {
        fprintf(2, "failed to close file\n");
        exit(1);
    }

    for (int i = 0; i < 100; i++)
        printf("%d\n", buf[i]);
}