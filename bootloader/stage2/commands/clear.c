/*
 * clear.c
 */
#include <cli.h>
#include <stdio.h>
#include <strings.h>

/**
 * Two possible arguments: forecolor and backcolor
 */
uint8_t clear(uint8_t argn, const char **argv)
{
    uint8_t fg, bg;

    screen_getcolor(&fg, &bg);

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
    screen_setcolor(fg, bg);
    screen_clear();

    // Return
    return 0;
}


REGISTER_CLI_COMMAND("clear", "Clear the screen", clear);
