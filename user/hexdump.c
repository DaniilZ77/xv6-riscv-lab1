#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int stoi(char* num) {
    int res = 0;
    while (*num != 0) {
        if (*num > '9' || *num < '0')
            return -1;
        res = res * 10 + (*num - '0');
        num++;
    }
    return res;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "invalid amount of args\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(2, "failed to open file\n");
        exit(1);
    }

    int buf_size = stoi(argv[2]);
    if (buf_size < 0) {
        fprintf(2, "failed to parse buffer size\n");
        exit(1);
    }

    char* buf = malloc(buf_size);
    if (buf == 0) {
        fprintf(2, "failed to allocate memory\n");
        exit(1);
    }

    int r = 0;
    int r_all = 0;
    while ((r = read(fd, buf + r_all, buf_size)) > 0) {
        if (r < 0) {
            fprintf(2, "failed to read from file\n");
            exit(1);
        }
        buf_size -= r;
        r_all += r;
    }
    if (close(fd) < 0) {
        fprintf(2, "failed to close file\n");
        exit(1);
    }

    for (int i = 0; i < r_all; i++)
        printf("%x", buf[i]);
    if (r_all > 0)
        printf("\n");
    free(buf);
}