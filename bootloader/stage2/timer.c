/*
 * timer.c
 *
 */
#include <timer.h>
#include <types.h>
#include <irq.h>
#include <ports.h>

// Tasks register
struct timer_task {
  timer_callback callback;
  uint32         period;
  uint32         last_call;
};

struct timer_task task_list[TIMER_MAX_TASKS];

// Timer counter
static uint32 timerCounter = 0;

void timer_handler(struct regs *regs)
{
  timerCounter++;
  // Check tasks
  register int i;
  for(i = 0; i < TIMER_MAX_TASKS; i++)
    if(task_list[i].callback &&
       timerCounter - task_list[i].last_call >= task_list[i].period) {
      // Call it
      task_list[i].callback(i);
      task_list[i].last_call = timerCounter;
    }
}

void timer_init()
{
  unsigned int divisor = 1193180 / TICKS_PER_SEC;

  outportb(0x43, 0x34);
  outportb(0x40, divisor & 0xFF); // Low
  outportb(0x40, divisor >> 8);   // High

  // Initialice queue
  register int i;
  for(i = 0; i < TIMER_MAX_TASKS; i++)
    task_list[i].callback = NULL;

  // Handler
  irq_install_handler(0, timer_handler);
}

void sleep(uint32 ms)
{
  static uint32 start;
  start = timerCounter;
  while(timerCounter - start < ms)
    asm("hlt");
}

int timer_register_task(timer_callback func, uint32 ms)
{
  register int i;
  // Put it somewhere
  for(i = 0; i < TIMER_MAX_TASKS; i++)
    if(task_list[i].callback == NULL) {
      task_list[i].callback  = func;
      task_list[i].period    = ms;
      task_list[i].last_call = timerCounter;
      return i;
    }
  // There is no place
  printf("timer_register_task: Queue full\n");
  return -1;
}

void timer_unregister_task(int task_id)
{
  task_list[task_id].callback = NULL;
}
