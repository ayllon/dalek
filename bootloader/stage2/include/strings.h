/*
 * String functions
 *
 */
#ifndef __STRINGS_H__
#define __STRINGS_H__
#include <types.h>

/* uint8 isnum(char c, int base = 10)
 * Check if the character c is a number (in base)
 * c    The character
 * base The base
 * Returns 1 if it is, 0 if it isn't
 */
uint8 isnum(char c, int base);

/* char itoa(long value, char *s, int base)
 * Converts a numerical value to a string
 * value  The value
 * s      Where to put the string
 * base   The base
 * Returns s if success
 */
char *itoa(long value, char *s, int base);

/* int16(const char *s)
 * Converts a string to an integer
 * 0x0000  Hexadecimal
 * 0b0000  Binary
 * 00000   Octal
 * Else    Decimal
 */
int16 atoi(const char *s);

/* uint8 strcmp(const char *s1, const char *s2)
 * Compares two strings
 * s1  The first string
 * s2  The second string
 * Returns:
 *   0  if s1 == s2
 *   1  if s1 > s2
 *   -1 if s1 < s2
 */
uint8 strcmp(const char *s1, const char *s2);

/* void strcpy(char *dest, const char *orig)
 * Copies the string orig into dest
 * dest Where to copy
 * orig From to copy
 */
void strcpy(char *dest, const char *orig);

/* int16 strcontains(const char *s, char c)
 * Search for the first ocurrence of character c in s, returns the index (or -1 if not found)
 */
int16 strcontains(const char *s, char c);

/* char **strsplit(const char *s, char **array, char separator)
 * Splits a string
 * s         The string
 * array     Where to put the result (i.e. array[0] is the first string). Last string begins with '\0'
 * separator The character used to split
 * Returns the number of strings
 */
uint8 strsplit(const char *s, char **array, char *separator);

#endif
