/*
 * String functions
 *
 */
#ifndef __STRINGS_H__
#define __STRINGS_H__
#include <types.h>

/**
 * Check if the character c is a number (in base)
 * c    The character
 * base The base
 * Returns 1 if it is, 0 if it isn't
 */
int isnum(char c, int base);

/**
 * Converts a numerical value to a string
 * value  The value
 * s      Where to put the string
 * base   The base
 * Returns s if success
 */
char *itoa(long value, char *s, int base);

/**
 * Converts a string to an integer
 * 0x0000  Hexadecimal
 * 0b0000  Binary
 * 00000   Octal
 * Else    Decimal
 */
int atoi(const char *s);

/**
 * Compares two strings
 * s1  The first string
 * s2  The second string
 * Returns:
 *   0  if s1 == s2
 *   1  if s1 > s2
 *   -1 if s1 < s2
 */
int strncmp(const char *s1, const char *s2, size_t len);

/**
 * Compares two strings
 * Same return as strncmp
 */
int strcmp(const char* s1, const char* s2);

/**
 * Copies the string orig into dest, always end with \0
 * dest Where to copy
 * orig From to copy
 */
void strlcpy(char *dest, const char *orig, size_t len);

/**
 * Search for the first ocurrence of character c in s, returns the index (or -1 if not found)
 */
int strcontains(const char *s, char c);

/**
 * Splits a string
 * s         The string
 * array     Where to put the result (i.e. array[0] is the first string). Last string begins with '\0'
 * separator The character used to split
 * Returns the number of strings
 */
int strsplit(const char *s, char **array, char *separator);

#endif
