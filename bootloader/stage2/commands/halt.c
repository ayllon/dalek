/*
 * halt.c
 *
 */
#include <printf.h>
#include <cli.h> 
#include <strings.h>

// Try to power off the computer
// We may keep working if we can't shut down
uint8 halt(uint8 argn, const char **argv)
{
  // Help
  if(argn > 1 && strcmp(argv[1], "help"))
  {
    printf("Shut down the system if it is possible\n");
    printf("Usage: halt [help]\n");
    return 0;
  }

  return 1;
}
