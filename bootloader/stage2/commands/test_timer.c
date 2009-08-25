#include <timer.h>
#include <printf.h>
#include <cli.h>

static void callback(int id)
{
  static int numCalls = 0;

  numCalls++;
  printf("TIMER CALLBACK %i...\n", numCalls);

  if(numCalls > 20) {
    numCalls = 0;
    timer_unregister_task(id);
  }
}

uint8 test_timer(uint8 argn, const char **argv)
{
  printf("Registering callback...\n");
  timer_register_task(callback, 1000);

  return 0;
}
