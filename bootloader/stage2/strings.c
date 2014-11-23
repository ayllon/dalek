/*
 * String functions
 */
#include <strings.h>
#include <types.h>
#include <memory.h>

/**
 * Check if the character c is a number (in base)
 * c    The character
 * base The base
 * Returns 1 if it is, 0 if it isn't
 */
uint8 isnum(char c, int base)
{
    switch (base) {
    case 2:
        return c == '1' || c == '0';
    case 8:
        return c >= '0' && c <= '7';
    case 10:
        return c >= '0' && c <= '9';
    case 16:
        return (c >= '0' && c <= '9')
                || ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
    default:
        return 0;
    }
}

/**
 * Converts a numerical value to a string
 * value  The value
 * s      Where to put the string
 * base   The base
 * Returns s if success
 */
char *itoa(long value, char *s, int base)
{
    char *p = s;
    char *p1, *p2;
    unsigned long ud = value;
    int divisor = 10;

    /* If %d is specified and D is minus, put `-' in the head. */
    if (base == 10 && value < 0) {
        *p++ = '-';
        s++;
        ud = -value;
    }
    else if (base == 16)
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0. */
    do {
        int remainder = ud % divisor;

        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    /* Terminate BUF. */
    *p = 0;

    /* Reverse BUF. */
    p1 = s;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    return s;
}

/**
 * Converts a string to an integer
 */
int16 atoi(const char *s)
{
    static uint16 base, value;
    // Get the base (2, 8, 10, 16)
    if (s[0] == '0') {
        switch (s[1]) {
        case 'x':
            base = 16;
            s += 2;
            break;
        case 'b':
            base = 2;
            s += 2;
            break;
        default:
            base = 8;
            s += 1;
        }
    }
    else {
        base = 10;
    }

    // Calculate
    value = 0;
    while (isnum(*s, base)) {
        value *= base;

        if (*s >= '0' && *s <= '9')
            value += *s - '0';
        else if (*s >= 'A' && *s <= 'F')
            value += (*s - 'A') + 0x0A;
        else if (*s >= 'a' && *s <= 'f')
            value += (*s - 'a') + 0x0A;

        s++;
    }

    return value;
}

/**
 * Compares two strings
 * s1  The first string
 * s2  The second string
 * Returns:
 *   0  if s1 == s2
 *   1  if s1 > s2
 *   -1 if s1 < s2
 */
uint8 strcmp(const char *s1, const char *s2)
{
    int i;
    for (i = 0; s1[i] != '\0'; i++) {
        // Differents
        if (s1[i] > s2[i] || s2[i] == '\0')
            return 1;
        if (s1[i] < s2[i])
            return -1;
    }
    // End of s1
    return (s2[i] == '\0') ? 0 : 1;
}

/**
 * Copies the string orig into dest
 * dest Where to copy
 * orig From to copy
 */
void strcpy(char *dest, const char *orig)
{
    int i;
    for (i = 0; orig[i] != '\0'; i++) {
        dest[i] = orig[i];
    }
}

/**
 * Search for the first ocurrence of character c in s, returns the index (or -1 if not found)
 */
int16 strcontains(const char *s, char c)
{
    static int16 i;

    for (i = 0; s[i] != '\0' && s[i] != c; i++)
        ;
    if (s[i] == '\0')
        i = -1;

    return i;
}

/**
 * Splits a string
 * s         The string
 * array     Where to put the result (i.e. array[0] is the first string). Last string begins with '\0'
 * separator The characters used to split
 * Returns the number of strings
 */
uint8 strsplit(const char *s, char **array, char *separator)
{
    static uint16 i, j, z;

    i = 0;
    z = 0;
    // If we are not at the end
    while (s[i] != '\0')
    {
        // Search for next separator
        for (j = i; s[j] != '\0' && strcontains(separator, s[j]) == -1; j++)
            ;
        // Copy this string into the array
        memcpy((uint8*) array[z], (uint8*) (s + i), j - i);
        // End with '\0'
        array[z][j - i] = '\0';
        // Increment
        if (s[j] != '\0')
            i = j + 1;
        else
            i = j;
        z++;
    }

    // Last array begins with '\0'
    array[z][0] = '\0';

    // Return
    return z;
}
