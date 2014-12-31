/*
 * cli.c
 *
 * Simple console for stage 2
 *
 */
#include <cli.h>
#include <printf.h>
#include <keyboard.h>
#include <get.h>
#include <strings.h>

/* void CLI()
 */
void CLI()
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
        for (cmd = __start___k_cli_cmd; cmd != __stop___k_cli_cmd; ++cmd) {
             if (strncmp(string_array[0], cmd->name, 128) == 0)
                 break;
        }
        // Run (or not)
        if (cmd != __stop___k_cli_cmd)
            (cmd->func)(argn, (const char**) (string_array));
        else
            printf("Wrong command \"%s\"\n", string_array[0]);
    }
}

uint8_t help(uint8_t argn, const char** argv)
{
    CliCommand* cmd;
    for (cmd = __start___k_cli_cmd; cmd != __stop___k_cli_cmd; ++cmd) {
        printf("%s\n\t%s\n", cmd->name, cmd->description);
    }
    return 0;
}

REGISTER_CLI_COMMAND("help", "Display the console help", help);

