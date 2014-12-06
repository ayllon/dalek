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
int isdigit(char c, int base)
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

static char* str_reverse(char* begin, char* end)
{
    char* p1 = begin;
    char* p2 = end - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    return begin;
}

/**
 * Converts a numerical value to a string
 * value   The value
 * base    The base (i.e 16 or 10)
 * reverse Normally 1 to get the proper string.
 *         If 0, leave the buffer from less significant to most
 * s       Where to put the string
 * base    The base
 * Returns Pointer to the \0 byte, or NULL on failure
 */
static char *itoa_last(long long value, int base, int reverse, char *s, size_t bsize)
{
    char *p = s;
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
    if (reverse) {
        str_reverse(s, p);
    }
    return p;
}

/**
 * Converts a numerical value to a string
 * value  The value
 * s      Where to put the string
 * base   The base
 * Returns s if success
 */
char *itoa_s(long long value, int base, char pad, int w, char *buffer,
        size_t bsize)
{
    char *p = itoa_last(value, base, 0, buffer, bsize);
    if (!p)
        return NULL;
    int len = p - buffer;
    int remaining = w - len;
    while (remaining > 0) {
        *p = pad;
        ++p;
        --remaining;
    }
    *p = '\0';
    return str_reverse(buffer, p);
}


/**
 * Converts a floating point number into a string
 */
char *ftoa_s(double value, int decimals, char* buffer, size_t bsize)
{
    long integer = (long)value;
    char* p = itoa_last(integer, 10, 1, buffer, bsize);
    if (p - buffer < bsize) {
        *p = '.';
        ++p;
    }
    char digit;
    while (decimals) {
        value *= 10;
        digit = (long)(value) % 10;
        *p = '0' + digit;
        ++p;
        decimals--;
    }
    *p = '\0';
    return buffer;
}

/**
 * Converts a string to an integer
 */
int atoi(const char *s)
{
    static uint16_t base, value;
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
    while (isdigit(*s, base)) {
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
int strncmp(const char *s1, const char *s2, size_t len)
{
    int i;
    for (i = 0; s1[i] != '\0' && i < len; i++) {
        if (s1[i] > s2[i] || s2[i] == '\0')
            return 1;
        if (s1[i] < s2[i])
            return -1;
    }
    // End of s1
    return (s2[i] == '\0') ? 0 : 1;
}

/**
 * Compares two strings
 */
int strcmp(const char *s1, const char *s2)
{
    int i;
    for (i = 0; s1[i] != '\0'; i++) {
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
void strlcpy(char *dest, const char *orig, size_t len)
{
    int i;
    for (i = 0; orig[i] != '\0' && i < len; i++) {
        dest[i] = orig[i];
    }
    dest[i] = dest[len - 1] ='\0';
}

/**
 * Search for the first occurrence of character c in s,
 * returns the index (or -1 if not found)
 */
int strcontains(const char *s, char c)
{
    static int16_t i;

    for (i = 0; s[i] != '\0' && s[i] != c; i++)
        ;
    if (s[i] == '\0')
        i = -1;

    return i;
}

/**
 * Splits a string
 * s         The string
 * array     Where to put the result (i.e. array[0] is the first string).
 *           Last string begins with '\0'
 * separator The characters used to split
 * Returns the number of strings
 */
int strsplit(const char *s, char **array, char *separator)
{
    static uint16_t i, j, z;

    i = 0;
    z = 0;
    // If we are not at the end
    while (s[i] != '\0')
    {
        // Search for next separator
        for (j = i; s[j] != '\0' && strcontains(separator, s[j]) == -1; j++)
            ;
        // Copy this string into the array
        memcpy((uint8_t*) array[z], (uint8_t*) (s + i), j - i);
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
