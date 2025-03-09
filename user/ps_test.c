#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "procinfo.h"

int main(int argc, char *argv[]) {
    struct procinfo plist[1];
    int ret = ps_listinfo(plist, 1);
    printf(ret == -1 ? "Test1(insufficient lim): passed\n" : "Test1(insufficient lim): failed\n");

    ret = ps_listinfo((struct procinfo *)123, 3);
    printf(ret == -2 ? "Test2(incorrect address): passed\n" : "Test2(incorrect address): failed\n");
}