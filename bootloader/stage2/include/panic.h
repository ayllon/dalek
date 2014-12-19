/*
 * panic.h
 */
#ifndef __PANIC_H__
#define __PANIC_H__

/**
 * Show a message and freeze the system
 */
void panic(const char* f, const char *s, ...);

#endif
