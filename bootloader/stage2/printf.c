/*
 * printf.c
 *
 * Functions to print strings
 *
 */
#include <printf.h>
#include <stdarg.h>
#include <strings.h>
#include <memory.h>

static struct
{
  uint8   *video;
  uint8    attr;
  uint16   posx, posy;
} screen = {(uint8*)T_VIDEO, T_ATTR, 0, 0};

/* cls()
 * Clear the terminal
 */
void cls()
{
  uint16 i = 0;
  
  for(i = 0; i < T_COLUMNS * T_ROWS * 2; i++)
    *(screen.video + i) = 0x00;

  screen.posx = 0;
  screen.posy = 0;
}

/* scroll(uint8)
 * Scrolls the terminal
 */
void scroll(uint8 n)
{
  if(n > T_ROWS)
    n = T_ROWS;
  if(n == 0)
    return;

  // 2 bytes per character (color and character)
  memcpy(screen.video,
	 screen.video + T_COLUMNS * n * 2,
	 (T_ROWS - n + 1) * T_COLUMNS * 2);
  memset(screen.video + (T_ROWS - n + 1) * T_COLUMNS * 2,
	 0,
	 n * T_COLUMNS * 2);

  screen.posy = T_ROWS - n + 1;
  screen.posx = 0;
}

/* setcolor(uint8, uint8)
 * Changes de current color for output
 */
void setcolor(uint8 forecolor, uint8 backcolor)
{
  screen.attr = forecolor | (backcolor << 4);
}

/* restorecolor()
 */
void restorecolor()
{
  screen.attr = T_ATTR;
}

/* putc(char)
 * Puts a character into the console
 * c   The character
 */
void putc(char c)
{
  // New line
  if(c == '\n')
  {
    screen.posx = 0;
    if(++(screen.posy) > T_ROWS)
      scroll(1);
    return;
  }else if(c == '\r')
  {
    screen.posx = 0;
    return;
  }else if(c == '\b')
  {
    if(screen.posx > 0)
    {
      screen.posx--;
      putc('\0');
      screen.posx--;
    }
    return;
  }

  // Put char
  *(screen.video + (screen.posx + screen.posy * T_COLUMNS) * 2)     = c & 0xFF;
  *(screen.video + (screen.posx + screen.posy * T_COLUMNS) * 2 + 1) = screen.attr;

  if(++(screen.posx) > T_COLUMNS)
  {
    screen.posx = 0;
    screen.posy++;
    if(screen.posy > T_ROWS)
      scroll(1);
  }
}

/* printf(const char *, ...)
 * Just a map for vprintf
 */
int printf(const char *s, ...)
{
  int a;
  va_list args;
  va_start(args, s);
  a = vprintf(s, args);
  va_end(args);
  return a;
}

/* vprintf(const char *, va_list)
 * Prints a formated string in the console
 * s    The formated string
 * args The arguments
 */
int vprintf(const char *s, va_list args)
{
  unsigned int i;
  static char buffer[16], *p;
  
  for(i = 0; *s; s++)
  {
    // Field
    if(*s != '%')
      putc(*s);
    else
    {
      switch(*(++s))
      {
        case 'c':
	  putc((char)(va_arg(args, int)));
	  break;
        case 'd': case 'i':
	  itoa(va_arg(args, int), buffer, 10);
	  p = buffer;
	  goto string;
        case 'x': case 'X': case 'p':
	  itoa(va_arg(args, int), buffer, 16);
	  p = buffer;
	  goto string;
        case 's':
	  p = va_arg(args, char*);
          string:
	  while(*p)
	    putc(*p++);
	  break;
        default:
	  putc(*s);
      }
    }
  }

  return i;  
}
