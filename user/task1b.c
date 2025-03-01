#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
    } else if (pid > 0) {
        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        kill(pid);

        int cpid, status;
        cpid = wait(&status);

        printf("child pid = %d exit with status = %d\n", cpid, status);
        exit(0);
    } else if (pid == 0) {
        sleep(10);
        exit(1);
    }
}