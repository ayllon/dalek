/*
 * reboot.c
 *
 */
#include <printf.h>
#include <cli.h> 
#include <timer.h>
#include <ports.h>
#include <strings.h>

// We will use the keyboard controller (8042)
// to reset the computer (command 0xFE)
uint8 reboot(uint8 argn, const char **argv)
{
  static uint8 temp;

  // Help
  if(argn > 1 && strcmp(argv[1], "help") == 0)
  {
    printf("Reboots the system\n");
    printf("Usage: reboot [help]\n");
    return 0;
  }

  // Tell
  printf("Rebooting...\n");
  sleep(500);

  // Wait until the controller is ready
  do{
    temp = inportb(0x64);
  }while((temp & 0x02) != 0x00);

  // And reset
  outportb(0x64, 0xFE);

  // If we are here, bad thing
  return 1;
}
