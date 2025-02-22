#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"


uint64 sys_add(void) {
    int n1, n2;
    argint(0, &n1);
    argint(1, &n2);

    return n1 + n2;
}