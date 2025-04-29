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

uint64 sys_rtc(void) {
  uint32 lo = ReadRegLo();
  uint32 hi = ReadRegHi();
  return ((uint64)hi << 32) | lo;
}
