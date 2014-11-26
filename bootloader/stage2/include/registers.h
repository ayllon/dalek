/*
 * registers.h
 */

#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <types.h>

typedef struct
{
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
} Registers;

#endif
