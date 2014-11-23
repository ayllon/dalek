/*
 * ports.h
 */
#ifndef _PORTS_H_
#define _PORTS_H_

#include <types.h>

uint8 inportb(uint16 port);
void outportb(uint16 port, uint8 data);

#endif
