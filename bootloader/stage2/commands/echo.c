/*
 * echo.c
 *
 */
#include <printf.h>
#include <cli.h>
#include <strings.h>

uint8 echo(uint8 argn, const char **argv)
{
  uint8 i;

  for(i = 1; i < argn; i++)
    printf("%s ", argv[i]);
  printf("\n");

  return 0;
}
