#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
    int fd;
    int status = mutex(&fd);
    printf("status: %d\n", status);

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    } else if (pid == 0) {
        pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            exit(1);
        } else if (pid == 0) {
            for (int i = 0; i < 10; i++) {
                mutex_lock(fd);
                printf("child of child iteration: %d\n", i);
                mutex_unlock(fd);
            }
        } else {
            for (int i = 0; i < 10; i++) {
                mutex_lock(fd);
                printf("child iteration: %d\n", i);
                mutex_unlock(fd);
            }
            wait(&status);
        }
    } else {
        for (int i = 0; i < 10; i++) {
            mutex_lock(fd);
            printf("parent iteration: %d\n", i);
            mutex_unlock(fd);
        }
        wait(&status);
    }
}