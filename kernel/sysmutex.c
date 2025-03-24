#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mutex.h"

extern int fdalloc(struct file *f);

uint64 sys_mutex(void) {
  uint64 fdaddr;
  struct file *f;
  int fd;
  struct proc *pr = myproc();

  argaddr(0, &fdaddr);
  if(mutexalloc(&f) < 0)
    return -1;
  if((fd = fdalloc(f)) < 0){
    fileclose(f);
    return -1;
  }
  if(copyout(pr->pagetable, fdaddr, (char*)&fd, sizeof(fd)) < 0){
    pr->ofile[fd] = 0;
    fileclose(f);
    return -1;
  }
  return 0;
}

uint64 sys_mutex_lock(void) {
    int fd;
    argint(0, &fd);
    struct file *f;
    int pid;
    struct proc *pr = myproc();
    acquire(&pr->lock);
    pid = pr->pid;
    release(&pr->lock);

    f = pr->ofile[fd];

    acquiresleep(&f->mutex->mu);
    acquire(&f->mutex->lock);
    f->mutex->owner = pid;
    release(&f->mutex->lock);

    return 0;
}

uint64 sys_mutex_unlock(void) {
    int fd;
    argint(0, &fd);
    struct file *f;
    int pid;
    struct proc *pr = myproc();
    f = pr->ofile[fd];
    acquire(&pr->lock);
    pid = pr->pid;
    release(&pr->lock);
    return mutexunlock(f, pid);
}