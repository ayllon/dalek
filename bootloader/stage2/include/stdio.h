/*
 * printf.h
 *
 * Functions to print string to the screen
 */
#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <io.h>
#include <stdarg.h>
#include <types.h>

extern IODevice* stdin;
extern IODevice* stdout;

/* Log level */
#define LOG_INFO    0
#define LOG_WARN    1
#define LOG_ERROR   2

/**
 * Gets an ascii character from keyboard
 * (No echo)
 */
char getc();

/**
 * Read a string from the keyboard until a '\n'
 * It gets only ASCII characters
 */
char *gets(char *s);

/* Simplified printf and vprintf functions.
 * Only partial formatting support
 */
int vprintf(const char *s, va_list args);
int printf(const char *s, ...);

/**
 * Prints a log entry
 */
int log(int level, const char* func, const char* msg, ...);

#endif
