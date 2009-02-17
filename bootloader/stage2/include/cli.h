/*
 * cli.h
 *
 * Simple console for stage 2
 *
 */
#ifndef __CLI_H__
#define __CLI_H__
#include <types.h>

#define CLI_PROMPT    "?: "
#define CLI_NOT_FOUND "Wrong command \"%s\"\n"

/* void CLI()
 * Main function of the Command Line Interpreter
 */
void CLI();

/** COMMANDS DECLARATIONS **
 * All commands function must have the form:
 * uint8 <function>(uint8, const char **args);
 * and return 0 if everything is OK
 */

/* clear()
 * Clear the screen
 */
uint8 clear(uint8 argn, const char **argv);

/* version()
 * Information about the stage2 loader version
 */
uint8 version(uint8 argn, const char **argv);

/** reboot()
 * Reboots
 */
uint8 reboot(uint8 argn, const char **argv);

/** halt()
 * Halts
 */
uint8 halt(uint8 argn, const char **argv);

/** echo()
 * Prints a string (or set of strings)
 */
uint8 echo(uint8 argn, const char **argv);

#endif
