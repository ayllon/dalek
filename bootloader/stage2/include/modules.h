/*
 * modules.h
 */
#ifndef __MODULES_H__
#define __MODULES_H__

typedef int(*ModuleInit)(void);
typedef int(*ModuleDestroy)(void);

/* Register module entry point. 0 goes first. */
#define MODULE_INIT_LVL(init, level) \
    ModuleInit __k_mod_##init __attribute__((section("__k_mod_init_"#level), used)) = init;

/* Register module exit point. 0 goes last. */
#define MODULE_DESTROY_LVL(deinit, level) \
    ModuleDestroy __k_mod_##deinit __attribute__((section("__k_mod_deinit_"#level), used)) = deinit;

/* Shortcuts */
#define MODULE_INIT(init) \
    MODULE_INIT_LVL(init, 99)
#define MODULE_DESTROY(deinit) \
    MODULE_DESTROY_LVL(deinit, 0)

/* Symbols defined by the linker */
extern ModuleInit __start___k_mod_init[];
extern ModuleInit __stop___k_mod_init[];
extern ModuleDestroy __start___k_mod_destroy[];
extern ModuleDestroy __stop___k_mod_destroy[];

/* Calls the modules initialization */
void mod_init_all(void);

/* Calls the modules destroy */
void mod_destroy_all(void);

#endif
