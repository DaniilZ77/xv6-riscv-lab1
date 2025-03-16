#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        fprintf(2, "error creating pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    } else if (pid > 0) {
        if (close(pfd[0]) < 0) {
            fprintf(2, "close error\n");
            exit(1);
        }

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
            if (close(pfd[1]) < 0) {
                fprintf(2, "close error\n");
            }
            fprintf(2, "write error\n");
            exit(1);
        } else if (close(pfd[1]) < 0) {
            fprintf(2, "close error\n");
            exit(1);
        }

        int status;
        wait(&status);
        exit(status);
    } else if (pid == 0) {
        if (close(0) < 0) {
            fprintf(2, "close error\n");
            exit(1);
        }
        if (dup(pfd[0]) < 0) {
            fprintf(2, "dup error\n");
            exit(1);
        }
        if (close(pfd[0]) < 0) {
            fprintf(2, "close error\n");
            exit(1);
        }
        if (close(pfd[1]) < 0) {
            fprintf(2, "close error\n");
            exit(1);
        }

        char *argv[] = {"/wc", 0};
        exec("/wc", argv);

        fprintf(2, "exec error");
        exit(1);
    }
}