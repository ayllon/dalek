/*
 * cli.h
 *
 * Simple console for stage 2
 *
 */
#ifndef __CLI_H__
#define __CLI_H__

#include <modules.h>
#include <types.h>

typedef struct {
    void *next; // To be used by cli.c
    const char* name;
    const char* description;
    uint8_t (*func)(uint8_t argn, const char** argv);
} CliCommand;

/** Helper for registration */
#define REGISTER_CLI_COMMAND(name, descr, func) \
    static CliCommand __cli_cmd_entry_##func = {NULL, name, descr, func }; \
    static int __cli_cmd_register_##func(void) { \
    	return cli_register(&__cli_cmd_entry_##func); \
    } \
    MODULE_INIT(__cli_cmd_register_##func)

/**
 * Register a command
 */
int cli_register(CliCommand* cli);

/**
 * Main function of the Command Line Interpreter
 */
void cli_start();

/**
 * Help command
 */
uint8_t cli_help(uint8_t argn, const char** argv);

#endif
