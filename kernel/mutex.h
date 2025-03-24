struct mutex {
    struct spinlock lock;
    struct sleeplock mu;
    int owner;
};

void mutexclose(struct mutex *mutex);
int mutexalloc(struct file **f);
int mutexunlock(struct file *f, int pid);