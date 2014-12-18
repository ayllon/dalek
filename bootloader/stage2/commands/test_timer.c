#include <timer.h>
#include <printf.h>
#include <cli.h>

static void callback(int id)
{
    static int numCalls = 0;

    numCalls++;
    printf("TIMER CALLBACK %i...\n", numCalls);

    if (numCalls > 20) {
        numCalls = 0;
        timer_unregister_task(id);
    }
}

/**
 * Test timer method
 */
uint8_t test_timer(uint8_t argn, const char **argv)
{
    printf("Registering callback...\n");
    timer_register_task(callback, 1000);

    return 0;
}


REGISTER_CLI_COMMAND("test_timer", "Just test the timer", test_timer);
