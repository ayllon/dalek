/*
 * types.h
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL	(void*)(0x00)

#ifdef __i386__
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef signed   char  int8;
typedef signed   short int16;
typedef signed   int   int32;

#if __LONG_MAX__==9223372036854775807L
typedef signed long int int64_t;
typedef unsigned long int uint64_t;
#elif __LONG_LONG_MAX__==9223372036854775807LL
typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;
#else
#error Can not find a 64 bits type
#endif

#else
#error Only i386 is supported
#endif

typedef int64_t  off_t;
typedef uint64_t size_t;
typedef int64_t  ssize_t;

struct regs
{
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

#endif
