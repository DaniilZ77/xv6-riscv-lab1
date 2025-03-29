#ifndef PROCINFO_H
#define PROCINFO_H

enum procinfostate { 
    procinfostate_sleeping,
    procinfostate_runnable,
    procinfostate_running,
    procinfostate_zombie
};

struct procinfo {
    int pid;
    int parent_pid;
    char name[16];
    enum procinfostate state;
};

#endif