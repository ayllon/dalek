/*
 * Timer and task routines
 */
#ifndef __TIMER_H__
#define __TIMER_H__
#include <types.h>

#define TICKS_PER_SEC   1000
#define TIMER_MAX_TASKS 32

/* Timer callback */
typedef void (*timer_callback)(int task_id);

/* Inits the timer */
void timer_init();

/* Sleep for some milliseconds */
void sleep(uint32_t ms);

/* Register a function that must be trigger */
int timer_register_task(timer_callback func, uint32_t ms);

/* Unregister a function from the list */
void timer_unregister_task(int task_id);

#endif
