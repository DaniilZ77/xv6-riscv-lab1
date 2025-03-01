#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        fprintf(stderr, "error creating pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork error\n");
    } else if (pid > 0) {
        close(pfd[0]);

        int w, len, ret = 0;
        argv++;
        while (*argv != 0 && (len = strlen(*argv)) && (w = write(pfd[1], *argv, len))) {
            if (len != w) {
                *argv += len;
            } else {
                if ((ret = write(pfd[1], " ", 1)) < 0) {
                    break;
                }
                argv++;
            }
        }
        if (ret < 0 || (ret = write(pfd[1], "\n", 1)) < 0) {
            close(pfd[1]);
            fprintf(stderr, "parent: write error\n");
            exit(1);
        } else if (close(pfd[1]) < 0) {
            fprintf(stderr, "parent: close error\n");
            exit(1);
        }

        int status;
        wait(&status);
        exit(status);
    } else if (pid == 0) {
        close(pfd[1]);

        char buf[BUF_SIZE];
        int w, r;
        while ((r = read(pfd[0], &buf, BUF_SIZE)) > 0) {
            char* cur = buf;
            while ((w = write(1, cur, r)) != r) {
                r -= w;
                cur += w;
            }
            if (w < 0) {
                fprintf(stderr, "child: write error");
                exit(1);
            }
        }
        close(pfd[0]);
        if (r < 0) {
            fprintf(stderr, "child: read error");
            exit(1);
        }

        exit(0);
    }
}