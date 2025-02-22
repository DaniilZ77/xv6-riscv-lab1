#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_LEN 50

int main(int argc, char *argv[]) {
    char buf[BUF_LEN];

    char err_space[] = "Read failure: must contain one space\n";
    int cc, i, sp = 0, err_space_len = strlen(err_space);
    char c;
    for (i = 0; i + 1 < BUF_LEN; ++i) {
        cc = read(0, &c, 1);
        if (cc < 1) {
            break;
        }
        buf[i] = c;

        if (c == '\n') {
            break;
        }
        if (!('0' <= c && c <= '9') && c != ' ') {
            write(2, "Read failure: must contain digits\n", 35);
            exit(1);
        }
        if (c == ' ' && sp++ != 0) {
            write(2, err_space, err_space_len);
            exit(1);
        }
        if (i + 2 == BUF_LEN) {
            write(2, "Read failure: buffer overflow\n", 31);
            exit(1);
        }
    }

    if (sp == 0) {
        write(2, err_space, err_space_len);
        exit(1);
    }

    buf[i] = '\0';

    char* cur = buf;
    int n1 = atoi(cur);
    while (*cur != ' ') ++cur;
    ++cur;
    int n2 = atoi(cur);

    printf("|%s|\n", buf);
    printf("%d\n", n1 + n2);
}