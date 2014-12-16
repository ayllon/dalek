/*
 * cli.h
 *
 * Simple console for stage 2
 *
 */
#ifndef __CLI_H__
#define __CLI_H__
#include <types.h>

typedef struct {
    const char* name;
    const char* description;
    uint8_t (*func)(uint8_t argn, const char** argv);
} CliCommand;

#define REGISTER_CLI_COMMAND(cmd, descr, func) \
    static const CliCommand k_cli_cmd_##func __attribute__((section("_k_cli_cmd"), unused)) = {cmd, descr, func};

/* Defined by the linker */
extern CliCommand __start__k_cli_cmd[];
extern CliCommand __stop__k_cli_cmd[];

/* void CLI()
 * Main function of the Command Line Interpreter
 */
void CLI();

/** COMMANDS DECLARATIONS **
 * All commands function must have the form:
 * uint8 <function>(uint8, const char **args);
 * and return 0 if everything is OK
 */

/**
 * Clear the screen
 */
uint8_t clear(uint8_t argn, const char **argv);

/**
 * Information about the stage2 loader version
 */
uint8_t version(uint8_t argn, const char **argv);

/**
 * Reboots
 */
uint8_t reboot(uint8_t argn, const char **argv);

/**
 * Halts
 */
uint8_t halt(uint8_t argn, const char **argv);

/**
 * Prints a string (or set of strings)
 */
uint8_t echo(uint8_t argn, const char **argv);

/**
 * Prints information about the memory
 */
uint8_t memory(uint8_t argn, const char **argv);

/**
 * Tests the timer
 */
uint8_t test_timer(uint8_t argn, const char **argv);

/**
 * Get a list of registered devices
 */
uint8_t devices(uint8_t argn, const char **argv);

#endif
