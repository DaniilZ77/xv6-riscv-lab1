#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    } else if (pid > 0) {
        sleep(20);
        if (kill(pid) < 0) {
            fprintf(2, "kill error\n");
            exit(1);
        }

        int status;
        wait(&status);
        printf("pid = %d exit with status = %d after kill\n", pid, status);
        exit(0);
    } else {
        char *args[] = {"task1a", 0};
        exec("/task1a", args);

        fprintf(2, "exec error\n");
        exit(1);
    }
} 