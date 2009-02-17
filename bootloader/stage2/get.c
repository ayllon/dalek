/*
 * get.c
 *
 */
#include <get.h>
#include <keyboard.h>

/* char getc()
 */
char getc()
{
  static uint16 c = 0;

  // Only ASCII
  while((c = kb_getc()) > 0xFF);

  // Return
  return c & 0xFF;
}

/* char *gets(char*)
 */
char *gets(char *dest) 
{
  static uint16 i;
  static char c;

  i = 0;

  // Get character
  do{
    c = getc();

    // Actions
    switch(c)
    {
    case '\b': // Delete
      if(i > 0)
      {
	dest[--i] = '\0';
	putc(c); // Echo
      }
      break;
    case '\n': // EOL
      dest[i] = '\0';
      putc(c);
      break;
    default: // Rest
      putc(dest[i++] = c);
    }
  }while(c != '\n'); // Until EOL

  // Return
  return dest;
}
