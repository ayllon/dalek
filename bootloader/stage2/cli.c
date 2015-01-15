/*
 * cli.c
 *
 * Simple console for stage 2
 *
 */
#include <cli.h>
#include <stdio.h>
#include <strings.h>


static CliCommand help_cli = {
        .next = NULL,
        .name = "help",
        .func = cli_help
};

static CliCommand* cli_list = &help_cli;


uint8_t cli_help(uint8_t argn, const char** argv)
{
    CliCommand* cmd;
    for (cmd = cli_list; cmd != NULL; cmd = (CliCommand*)cmd->next) {
        printf("%s\n\t%s\n", cmd->name, cmd->description);
    }
    return 0;
}


int cli_register(CliCommand* cli)
{
    cli->next = cli_list;
    cli_list = cli;
    return 0;
}


void cli_start()
{
    char buffer[128];
    char string_array_buffer[2048];
    char *string_array[16];
    uint8_t i, argn;

    // Initialize string array pointers
    for (i = 0; i < 16; i++) {
        string_array[i] = string_array_buffer + i * 128;
    }

    // Always
    while (1) {
        // Prompt
        printf("?: ");
        // Read command line
        gets(buffer);
        // Split
        argn = strsplit(buffer, string_array, " \t");
        // Search for command
        CliCommand* cmd;
        for (cmd = cli_list; cmd != NULL; cmd = (CliCommand*)cmd->next) {
             if (strncmp(string_array[0], cmd->name, 128) == 0)
                 break;
        }
        // Run (or not)
        if (cmd != NULL)
            (cmd->func)(argn, (const char**) (string_array));
        else
            printf("Wrong command \"%s\"\n", string_array[0]);
    }
}
