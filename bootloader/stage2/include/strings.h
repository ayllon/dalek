/*
 * String functions
 *
 */
#ifndef __STRINGS_H__
#define __STRINGS_H__
#include <types.h>

/* itoa()
 * Converts a numerical value to a string
 * value  The value
 * s      Where to put the string
 * base   The base
 * Returns s if success
 */
char *itoa(long value, char *s, int base);

/* strcmp()
 * Compares two strings
 * s1  The first string
 * s2  The second string
 * Returns:
 *   0  if s1 == s2
 *   1  if s1 > s2
 *   -1 if s1 < s2
 */
uint8 strcmp(const char *s1, const char *s2);

#endif
