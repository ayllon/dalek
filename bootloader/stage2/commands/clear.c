/*
 * clear.c
 */
#include <printf.h>
#include <cli.h>
#include <strings.h>

/**
 * Two possible arguments: forecolor and backcolor
 */
uint8_t clear(uint8_t argn, const char **argv)
{
    static uint8_t fg, bg;

    getcolor(&fg, &bg);

    // There are arguments?
    if (argn > 1) {
        // Help
        if (strcmp(argv[1], "help") == 0) {
            printf(
                    "Clear the screen.\nUsage: clear [help] | [<forecolor> [<backcolor>]]\n");
            return 0;
        }
        else {
            fg = atoi(argv[1]);
            if (argn > 2)
                bg = atoi(argv[2]);
        }
    }

    // Clear
    setcolor(fg, bg);
    cls();

    // Return
    return 0;
}


REGISTER_CLI_COMMAND("clear", "Clear the screen", clear);
