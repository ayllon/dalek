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
    uint8_t fg = 0, bg = 0;

    // There are arguments?
    if (argn > 1) {
        // Help
        if (strcmp(argv[1], "help") == 0) {
            printf("Clear the screen.\nUsage: clear [help] | [<forecolor> [<backcolor>]]\n");
            return 0;
        }
        else {
            fg = atoi(argv[1]);
            if (argn > 2)
                bg = atoi(argv[2]);
        }
    }

    // Clear
    //screen_setcolor(fg, bg);
    printf("\x1b[2J");

    // Return
    return 0;
}


REGISTER_CLI_COMMAND("clear", "Clear the screen", clear);
