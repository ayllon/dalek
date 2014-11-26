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

uint8_t help(uint8_t, const char**);

/** COMMAND LIST **/
static struct
{
  uint8_t (*f)(uint8_t argn, const char **argv);
  char  *fname;
}command_list[] = {
   {version   , "version"   },
   {clear     , "clear"     },
   {reboot    , "reboot"    },
   {halt      , "halt"      },
   {echo      , "echo"      },
   {memory    , "memory"    },
   {test_timer, "test_timer"},
   {help      , "help"      },
   {NULL      , NULL        }
};

/* void CLI()
 */
void CLI()
{
    static char buffer[128];
    static char string_array_buffer[2048];
    static char *string_array[16];
    static uint8_t i, argn;

    // Initialize string array pointers
    for (i = 0; i < 16; i++)
        string_array[i] = string_array_buffer + i * 128;

    // Always
    while (1) {
        // Prompt
        printf("%s", CLI_PROMPT);
        // Read command line
        gets(buffer);
        // Split
        argn = strsplit(buffer, string_array, " \t");
        // Search for command
        for (i = 0;
             command_list[i].f != NULL
             && strncmp(string_array[0], command_list[i].fname, 128) != 0;
             i++)
            ;
        // Run (or not)
        if (command_list[i].f != NULL)
            (command_list[i].f)(argn, (const char**) (string_array));
        else
            printf(CLI_NOT_FOUND, string_array[0]);
    }
}

uint8_t help(uint8_t argn, const char** argv)
{
    int i;
    for (i = 0; command_list[i].f != NULL ; ++i) {
        printf("\t%s\n", command_list[i].fname);
    }
    return 0;
}
