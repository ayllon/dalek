/*
 * registers.h
 */

#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <types.h>

typedef struct
{
    unsigned int cs, ds, ss, es;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed)) Registers;

#endif
