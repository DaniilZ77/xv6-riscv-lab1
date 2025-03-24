#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char* argv[]) {
    int fd;
    int status = mutex(&fd);
    if (status < 0) {
        printf("mutex failed\n");
        exit(1);
    }

    printf(read(fd, 0, 0) == -1 ? "Test 1 (read from mutex): passed\n" : "Test 1 (read from mutex): failed\n");
    printf(write(fd, 0, 0) == -1 ? "Test 2 (write to mutex): passed\n" : "Test 2 (write to mutex): failed\n");

    mutex_lock(fd);
    printf(close(fd) == 0 && mutex_lock(fd) == -1 ? "Test 3 (close mutex): passed\n" : "Test 3 (close mutex): failed\n");

    status = mutex(&fd);
    if (status < 0) {
        printf("mutex failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } else if (pid == 0) {
        mutex_lock(fd);
    } else {
        sleep(2);
        printf(mutex_unlock(fd) == -1 ? "Test 4 (unlock mutex from another process): passed\n" : "Test 4 (unlock mutex from another process): failed\n");
        printf(close(fd) == 0 && mutex_unlock(fd) == -1 ? "Test 5 (close mutex from another process): passed\n" : "Test 5 (close mutex from another process): failed\n");
    }
}