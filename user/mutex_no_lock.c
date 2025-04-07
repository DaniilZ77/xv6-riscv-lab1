#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
    int pid = fork();
    char c;
    int status;
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    } else if (pid == 0) {
        for (int i = 0; i < argc; i++) {
            for (int j = 0; argv[i][j] != '\0'; j++) {
                c = argv[i][j];
                printf("%d: arg %d, char %s\n", getpid(), i, &c);
            }
        }
    } else {
        for (int i = 0; i < argc; i++) {
            for (int j = 0; argv[i][j] != '\0'; j++) {
                c = argv[i][j];
                printf("%d: arg %d, char %s\n", getpid(), i, &c);
            }
        }
        wait(&status);
    }
}