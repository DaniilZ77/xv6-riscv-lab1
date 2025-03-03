#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define BUF_LEN 50
int main(int argc, char *argv[]) {
    char buf[BUF_LEN];
    int cc, i, sp = 0;
    char c;
    char *cur;
    for (i = 0; i + 1 < BUF_LEN; ++i) {
        cc = read(0, &c, 1);
        if (cc == 0) break;
        if (cc < 0) {
            write(2, "Read failure\n", 14);
            exit(1);
        }
        buf[i] = c;
        if (c == '\n') break;
        if (!('0' <= c && c <= '9') && c != ' ') {
            write(2, "Read failure: must contain digits\n", 35);
            exit(1);
        }
        if (c == ' ' && (cur = buf + i) && ++sp);
        if (i + 2 == BUF_LEN) {
            write(2, "Read failure: buffer overflow\n", 31);
            exit(1);
        }
    }
    if (sp != 1) {
        write(2, "Read failure: must contain one space\n", 38);
        exit(1);
    }
    buf[i] = '\0';
    int n1 = atoi(buf);
    int n2 = atoi(++cur);
    printf("|%s|\n", buf);
    printf("%d\n", n1 + n2);
    exit(0);
}