/*
 * echo.c
 */
#include <cli.h>
#include <stdio.h>
#include <strings.h>

/**
 * Echoes the arguments
 */
uint8_t echo(uint8_t argn, const char **argv)
{
    uint8_t i;

    for (i = 1; i < argn; i++)
        printf("%s ", argv[i]);
    printf("\n");

    return 0;
}


REGISTER_CLI_COMMAND("echo", "Echo the string(s)", echo);
