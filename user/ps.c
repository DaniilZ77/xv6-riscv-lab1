#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "procinfo.h"

#define LIM 10

int main(int argc, char *argv[]) {
    struct procinfo plist[LIM];
    int ret = ps_listinfo(plist, LIM);
    if (ret == -1) {
        fprintf(2, "error: must increase buffer size\n");
        exit(1);
    } else if (ret == -2) {
        fprintf(2, "error: failed to write into provided address\n");
        exit(1);
    }

    printf("total procs: %d\n---\n", ret);

    for (int i = 0; i < ret; i++) {
        printf("pid: %d\n", plist[i].pid);
        printf("ppid: %d\n", plist[i].parent_pid);
        printf("name: %s\n", plist[i].name);
        printf("state: ");
        switch (plist[i].state)
        {
        case procinfostate_used:
            printf("used");
            break;
        case procinfostate_runnable:
            printf("runnable");
            break;
        case procinfostate_running:
            printf("running");
            break;
        case procinfostate_sleeping:
            printf("sleeping");
            break;
        case procinfostate_zombie:
            printf("zombie");
        }
        printf("\n---\n");
    }

    exit(0);
}