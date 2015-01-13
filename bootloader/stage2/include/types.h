/*
 * types.h
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#define NULL	(void*)(0x00)

#ifdef __i386__

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef signed   char  int8_t;
typedef signed   short int16_t;
typedef signed   int   int32_t;

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

typedef int32_t  off_t;
typedef uint32_t size_t;
typedef int32_t  ssize_t;

#endif
