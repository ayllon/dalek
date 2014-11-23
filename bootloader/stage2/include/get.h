/*
 * get.h
 *
 * "Get" routines
 */
#ifndef __GET_H__
#define __GET_H__

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

#endif
