#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "pseudo.h"

struct {
    struct spinlock lock;
    uint64 seed;
} urandom;

struct {
    struct spinlock lock;
    uint64 counter;
} nullstat;

int pseudoread(short minor, int user_dst, uint64 dst, int n) {
    switch (minor) {
    case MINOR_NULL:
        return 0;
    case MINOR_ZERO:
        char zero = 0;
        for (int i = 0; i < n; i++) {
            if (either_copyout(user_dst, dst + i, &zero, 1) == -1)
                return -1;
        }
        return n;
    case MINOR_URANDOM:
        for (int i = 0; i < n; i++) {
            uint64 rand;
            acquire(&urandom.lock);
            urandom.seed = urandom.seed * 3107121499 + 46061;
            rand = urandom.seed & 0xFF;
            release(&urandom.lock);
            if (either_copyout(user_dst, dst + i, &rand, 1) == -1)
                return -1;
        }
        return n;
    case MINOR_NULLSTAT:
        if (n != sizeof(uint64))
            return -1;
        
        uint64 local_counter;
        acquire(&nullstat.lock);
        local_counter = nullstat.counter;
        release(&nullstat.lock);

        if (either_copyout(user_dst, dst, &local_counter, sizeof(uint64)) == -1)
            return -1;

        return sizeof(uint64);
    default:
        return -1;
    }
}

int pseudowrite(short minor, int user_src, uint64 src, int n) {
    switch (minor) {
    case MINOR_NULL:
        return n;
    case MINOR_ZERO:
        return -1;
    case MINOR_URANDOM:
        if (n != sizeof(uint64))
            return -1;
        uint64 seed;
        if (either_copyin(&seed, user_src, src, n) == -1)
            return -1;
        acquire(&urandom.lock);
        urandom.seed = seed;
        release(&urandom.lock);
        return n;
    case MINOR_NULLSTAT:
        acquire(&nullstat.lock);
        nullstat.counter += n;
        release(&nullstat.lock);
        return n;
    default:
        return -1;
    };
}

void pseudoinit(void) {
    initlock(&nullstat.lock, "nullstat");
    initlock(&urandom.lock,  "urandom");
    devsw[PSEUDO].read = pseudoread;
    devsw[PSEUDO].write = pseudowrite;
}