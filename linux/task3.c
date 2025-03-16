#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        perror("pipe error");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        if (close(pfd[0]) < 0) {
            perror("parent close read end error");
            exit(1);
        }

        ssize_t w, len, ret;
        argv++;
        while (*argv != NULL && (len = strlen(*argv)) > 0) {
            w = write(pfd[1], *argv, len);
            if (w < 0) {
                perror("parent write error");
                exit(1);
            }
            if ((size_t)w < len) {
                *argv += w;
            } else {
                ret = write(pfd[1], " ", 1);
                if (ret < 0) {
                    perror("parent write space error");
                    break;
                }
                argv++;
            }
        }
        ret = write(pfd[1], "\n", 1);
        if (ret < 0) {
            perror("parent write newline error");
            close(pfd[1]);
            exit(1);
        }
        if (close(pfd[1]) < 0) {
            perror("parent close write end error");
            exit(1);
        }

        int status;
        if (wait(&status) < 0) {
            perror("wait error");
            exit(1);
        }
        exit(status);
    } else {
        if (close(pfd[1]) < 0) {
            perror("child close write end error");
            exit(1);
        }

        char buf[BUF_SIZE];
        ssize_t w, r;
        while ((r = read(pfd[0], buf, BUF_SIZE)) > 0) {
            ssize_t total_written = 0;
            while (total_written < r) {
                w = write(1, buf + total_written, r - total_written);
                if (w < 0) {
                    perror("child write error");
                    exit(1);
                }
                total_written += w;
            }
        }
        if (r < 0) {
            perror("child read error");
            exit(1);
        }
        if (close(pfd[0]) < 0) {
            perror("child close read end error");
            exit(1);
        }
        exit(0);
    }
}
