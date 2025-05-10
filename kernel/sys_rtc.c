#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define Reg(o) ((volatile uint32*)(RTC0 + (o)))
#define ReadRegLo() (*Reg(0))
#define ReadRegHi() (*Reg(4))

struct spinlock rtc_lock;

uint64 sys_rtc(void) {
  acquire(&rtc_lock);
  uint32 lo = ReadRegLo();
  uint32 hi = ReadRegHi();
  release(&rtc_lock);
  return ((uint64)hi << 32) | lo;
}

void rtcinit(void) {
  initlock(&rtc_lock, "rtc");
}