/*
 * version.c
 */
#include <cli.h>
#include <stdio.h>

/**
 * Just prints the version
 */
uint8_t version(uint8_t argn, const char **argv)
{
    printf("\nDAleK Boot Loader (Stage 2). Version 0.0.1\n");
    printf("==========================================\n");
    printf("\n2009 (c) Alejandro Alvarez Ayllon\n");
    printf("Licensed under the GPL license.\n\n");
    return 0;
}


REGISTER_CLI_COMMAND("version", "Show information about the version", version);
