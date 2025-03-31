#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define D_FLAG 0b01
#define A_FLAG 0b10
#define ALL_FLAG 0b0
#define PTE_G (1L << 5)
#define PTE_A (1L << 6)
#define PTE_D (1L << 7)

void indent(int level) {
    for (int i = level; i < 2; i++)
        printf("...");
}

void zeros(int n) {
    printf("0x");
    if (n < 0x10) printf("00");
    else if (n < 0x100) printf("0");
}

void flags(pte_t pte) {
    if (pte & PTE_R) printf("R");
    else printf("_");
    if (pte & PTE_W) printf("W");
    else printf("_");
    if (pte & PTE_X) printf("X");
    else printf("_");
    if (pte & PTE_U) printf("U");
    else printf("_");
    if (pte & PTE_G) printf("G");
    else printf("_");
    if (pte & PTE_A) printf("A");
    else printf("_");
    if (pte & PTE_D) printf("D");
    else printf("_");
    printf("\n");
}

int satisfy_flags(pte_t pte, int pagetable_flags) {
    switch (pagetable_flags) {
    case D_FLAG:
        if (pte & PTE_D) return 1;
        return 0;
    case A_FLAG:
        if (pte & PTE_A) return 1;
        return 0;
    case ALL_FLAG:
        return 1;
    default:
        panic("unreachable");
    }
}

void walk_buffer(
  int level,
  pagetable_t pagetable,
  int pagetable_flags,
  uint64 buf_addr,
  uint64 buf_len,
  void (*lambda)(int level, uint64 index, pagetable_t next_pagetable, int pagetable_flags, pte_t *pte)
) {
    if (level < 0) return;
    uint64 va, n, i = 0, index, block_size = 1L << (9 * level + PGSHIFT);
    pte_t *pte;
    while ((buf_len > 0 || buf_addr == 0) && i < 512) {
        va = PGROUNDDOWN(buf_addr);
        index = buf_addr == 0 ? i : PX(level, va);
        pte = &pagetable[index];
        if ((*pte & PTE_V) && (satisfy_flags(*pte, pagetable_flags) || level > 0)) {
            pagetable_t next_pagetable = (pagetable_t)PTE2PA(*pte);
            (*lambda)(level, index, next_pagetable, pagetable_flags, pte);
            walk_buffer(level - 1, next_pagetable, pagetable_flags, buf_addr, buf_len, lambda);
        }
        if (*pte & PTE_V) {
            n = block_size - (buf_addr - va);
            buf_addr = buf_addr == 0 ? 0 : va + block_size;
            if (n > buf_len)
              n = buf_len;
            buf_len -= n;
        }
        i++;
    }
}

void print_info(int level, uint64 index, pagetable_t next_pagetable, int pagetable_flags, pte_t *pte) {
    indent(level);
    zeros(index);
    printf("%lx -> %p ", index, next_pagetable);
    flags(*pte);
}

uint64 sys_proc_pagetables(void) {
    uint64 buf_addr;
    argaddr(0, &buf_addr);
    int buf_len;
    argint(1, &buf_len);
    int pagetable_flags;
    argint(2, &pagetable_flags);
    if (pagetable_flags > 2 || pagetable_flags < 0) return -1;
    pagetable_t pagetable = myproc()->pagetable;
    printf("PAGETABLE %p\n", (pagetable_t)PTE2PA(pagetable[0]));
    if (buf_addr == 0 || buf_len == 0) {
      buf_addr = 0;
      buf_len = 0;
    }
    walk_buffer(2, pagetable, pagetable_flags, buf_addr, buf_len, print_info);
    return 0;
}

void unflag(int level, uint64 index, pagetable_t next_pagetable, int pagetable_flags, pte_t *pte) {
  switch (pagetable_flags) {
  case D_FLAG:
      *pte &= ~PTE_D;
      break;
  case A_FLAG:
      *pte &= ~PTE_A;
      break;
  case ALL_FLAG:
      *pte &= ~(PTE_A | PTE_D);
      break;
  default:
      panic("unreachable");
  }
}

uint64 sys_proc_pagetables_unflag(void) {
    uint64 buf_addr;
    argaddr(0, &buf_addr);
    int buf_len;
    argint(1, &buf_len);
    int pagetable_flags;
    argint(2, &pagetable_flags);
    if (pagetable_flags > 2 || pagetable_flags < 0) return -1;
    pagetable_t pagetable = myproc()->pagetable;
    if (buf_addr == 0 || buf_len == 0) {
      buf_addr = 0;
      buf_len = 0;
    }
    walk_buffer(2, pagetable, pagetable_flags, buf_addr, buf_len, unflag);
    return 0;
}
