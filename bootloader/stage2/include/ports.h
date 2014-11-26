/*
 * ports.h
 */
#ifndef _PORTS_H_
#define _PORTS_H_

#include <types.h>

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t data);

#endif
