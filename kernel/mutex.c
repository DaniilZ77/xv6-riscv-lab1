#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mutex.h"

int mutex_unlock(struct file *f, int pid) {
    acquire(&f->mutex->lock);
    if (f->mutex->owner != pid) {
        release(&f->mutex->lock);
        return -1;
    }
    f->mutex->owner = -1;
    release(&f->mutex->lock);
    releasesleep(&f->mutex->mu);
    return 0;
}

int mutexalloc(struct file **f) {
    *f = 0;
    struct mutex* mutex = 0;
    if((*f = filealloc()) == 0)
        goto bad;

    if ((mutex = (struct mutex*)kalloc()) == 0)
        goto bad;

    initsleeplock(&mutex->mu, "mutex");
    initlock(&mutex->lock, "mutex");
    mutex->owner = -1;
    (*f)->type = FD_MUTEX;
    (*f)->readable = (*f)->writable = 0;
    (*f)->pipe = 0;
    (*f)->mutex = mutex;
    return 0;

bad:
    if(mutex)
        kfree((char*)mutex);
    if(*f)
        fileclose(*f);
    return -1;
}

void mutexclose(struct mutex *mutex) {
    kfree((char*)mutex);
}