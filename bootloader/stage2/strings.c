/*
 * String functions
 *
 */
#include <strings.h>
#include <types.h>

/* itoa()
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
  if (base == 10 && value < 0)
  {
    *p++ = '-';
    s++;
    ud = -value;
  }
  else if (base == 16)
    divisor = 16;
     
  /* Divide UD by DIVISOR until UD == 0. */
  do
  {
    int remainder = ud % divisor;
     
    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
  }
  while (ud /= divisor);
     
  /* Terminate BUF. */
  *p = 0;
     
  /* Reverse BUF. */
  p1 = s;
  p2 = p - 1;
  while (p1 < p2)
  {
    char tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
    p1++;
    p2--;
  }
  return s;
}

/* strcmp()
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
  for(i = 0; s1[i] != '\0'; i++)
  {
    // Differents
    if(s1[i] > s2[i] || s2[i] == '\0')
      return 1;
    if(s1[i] < s2[i])
      return -1;
  }
  // End of s1
  return (s2[i] == '\0')?0:1;
}

