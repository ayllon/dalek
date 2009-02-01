/*
 * Timer and task routines
 *
 */
#ifndef __TIMER_H__
#define __TIMER_H__
#include <types.h>

#define TICKS_PER_SEC 1000

void timer_init();
void sleep(uint32 ms);

#endif
