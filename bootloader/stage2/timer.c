/*
 * timer.c
 *
 */
#include <timer.h>
#include <types.h>
#include <irq.h>

static uint32 timerCounter = 0;

void timer_handler(struct regs *regs)
{
  timerCounter++;
}

void timer_init()
{
  unsigned int divisor = 1193180 / TICKS_PER_SEC;

  outportb(0x43, 0x34);
  outportb(0x40, divisor & 0xFF); // Low
  outportb(0x40, divisor >> 8);   // High

  irq_install_handler(0, timer_handler);
}

void sleep(uint32 ms)
{
  static uint32 start;
  start = timerCounter;
  while(timerCounter - start < ms)
    asm("hlt");
}
