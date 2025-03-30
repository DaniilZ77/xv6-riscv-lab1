#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    // char* arr = (char*)malloc(4096);
    proc_pagetables(0, 0, 0b0);
}