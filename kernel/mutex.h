struct mutex {
    struct spinlock lock;
    struct sleeplock mu;
    int owner;
};

void mutexclose(struct mutex *mutex);
int mutexalloc(struct file **f);
int mutex_unlock(struct file *f, int pid);