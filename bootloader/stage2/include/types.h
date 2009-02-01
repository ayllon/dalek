/*
 * typesh
 *
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL	(void*)(0x00)

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;
typedef signed   char  int8;
typedef signed   short int16;
typedef signed   int   int32;
typedef signed   long  int64;

struct regs
{
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};


#endif
