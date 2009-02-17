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

/** COMMAND LIST **/
static struct
{
  uint8 (*f)(uint8 argn, const char **argv);
  char  *fname;
}command_list[] =
  {{version, "version"},
   {clear,   "clear"},
   {reboot,  "reboot"},
   {halt,    "halt"},
   {echo,    "echo"},
   {NULL, NULL}};

/* void CLI()
 */
void CLI()
{
  static char buffer[128];
  static char string_array_buffer[2048];
  static char *string_array[16];
  static uint8 i, argn;

  // Initialize string array pointers
  for(i = 0; i < 16; i++)
    string_array[i] = string_array_buffer + i * 128;

  // Always
  while(1)
  {
    // Prompt
    printf("%s", CLI_PROMPT);
    // Read command line
    gets(buffer);
    // Split
    argn = strsplit(buffer, string_array, " \t");
    // Search for command
    for(i = 0;
	command_list[i].f != NULL && strcmp(string_array[0], command_list[i].fname) != 0;
	i++);
    // Run (or not)
    if(command_list[i].f != NULL)
      (command_list[i].f)(argn, (char**)(string_array));
    else
      printf(CLI_NOT_FOUND, string_array[0]);
  }
}
