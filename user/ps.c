#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "procinfo.h"

#define MAX_TRIES 15

int main(int argc, char *argv[]) {
    int size = 1;
    struct procinfo* plist = (struct procinfo*)malloc(sizeof(struct procinfo) * size);
    int ret;
    int tries = 0;
    while (tries < MAX_TRIES) {
        ret = ps_listinfo(plist, size);
        if (ret == -1) {
            size *= 2;
            free(plist);
            plist = (struct procinfo*)malloc(sizeof(struct procinfo) * size);
            ++tries;
            continue;
        } else if (ret == -2) {
            fprintf(2, "error: failed to write into provided address\n");
            exit(1);
        }
        break;
    }

    printf("total procs: %d\n---\n", ret);

    for (int i = 0; i < ret; i++) {
        printf("pid: %d\n", plist[i].pid);
        printf("ppid: %d\n", plist[i].parent_pid);
        printf("name: %s\n", plist[i].name);
        printf("state: ");
        switch (plist[i].state) {
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
    free(plist);

    exit(0);
}