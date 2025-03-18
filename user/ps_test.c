#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "procinfo.h"

int main(int argc, char *argv[]) {
    struct procinfo a[1];
    int ret = ps_listinfo(a, 1);
    printf(ret == -1 ? "Test1(insufficient lim): passed\n" : "Test1(insufficient lim): failed\n");

    ret = ps_listinfo((struct procinfo *)123, 3);
    printf(ret == -2 ? "Test2(incorrect address): passed\n" : "Test2(incorrect address): failed\n");

    struct procinfo b[32];
    ret = ps_listinfo(b, 32);
    printf(ret == 3 ? "Test3(success): passed\n" : "Test3(success): failed\n");

    ret = ps_listinfo(0, 32);
    printf(ret == 3 ? "Test4(success): passed\n" : "Test4(success): failed\n");

    ret = ps_listinfo(0, 0);
    printf(ret == 3 ? "Test5(success): passed\n" : "Test5(success): failed\n");
}