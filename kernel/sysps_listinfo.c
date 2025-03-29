#include "procinfo.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

uint64 sys_ps_listinfo(void) {
    uint64 plist;
    int lim;

    argaddr(0, &plist);
    argint(1, &lim);

    struct proc *p;
    int pcount = 0;
    if (plist == 0) {
        for (p = proc; p < &proc[NPROC]; p++) {
            acquire(&p->lock);
            if (p->state != UNUSED && p->state != USED) {
                ++pcount;
            }
            release(&p->lock);
        }
        return pcount; 
    }

    struct procinfo pitem;
    for (p = proc; p < &proc[NPROC]; p++) {
        acquire(&p->lock);
        if (p->state != UNUSED && p->state != USED) {
            if (++pcount > lim) {
                release(&p->lock);
                return -1;
            }

            pitem.pid = p->pid;
            safestrcpy(pitem.name, p->name, sizeof(pitem.name));
            switch(p->state) {
            case SLEEPING:
                pitem.state = procinfostate_sleeping;
                break;
            case RUNNABLE:
                pitem.state = procinfostate_runnable;
                break;
            case RUNNING:
                pitem.state = procinfostate_running;
                break;
            case ZOMBIE:
                pitem.state = procinfostate_zombie;
            default:
            }
            acquire(&wait_lock);
            pitem.parent_pid = -1;
            if (p->parent) {
                acquire(&p->parent->lock);
                pitem.parent_pid = p->parent->pid;
                release(&p->parent->lock);
            }
            release(&wait_lock);

            if (copyout(myproc()->pagetable, plist, (char*)&pitem, sizeof(struct procinfo)) < 0) {
                release(&p->lock);
                return -2;
            }

            plist += sizeof(struct procinfo);
        }
        release(&p->lock);
    }
    return pcount;
}