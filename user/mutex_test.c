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
            sleep(1);
            mutex_lock(fd);
            for (int i = 0; i < 10; i++) {
                printf("child of child iteration: %d\n", i);
            }
            mutex_unlock(fd);
        } else {
            mutex_lock(fd);
            for (int i = 0; i < 10; i++) {
                printf("child iteration: %d\n", i);
            }
            mutex_unlock(fd);
        }
    } else {
        sleep(2);
        mutex_lock(fd);
        for (int i = 0; i < 10; i++) {
            printf("parent iteration: %d\n", i);
        }
        mutex_unlock(fd);
        sleep(50);
    }
}