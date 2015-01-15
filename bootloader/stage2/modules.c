/*
 * modules.c
 */
#include <modules.h>
#include <stdio.h>


void mod_init_all(void)
{
    ModuleInit* mod_init;

    log(LOG_INFO, __func__, "Initializing modules");
    for (mod_init = __start___k_mod_init;
         mod_init != __stop___k_mod_init;
         ++mod_init)
    {
        (*mod_init)();
    }
    log(LOG_INFO, __func__, "Modules initialized");
}


void mod_destroy_all(void)
{
    ModuleDestroy* mod_destroy;

    log(LOG_INFO, __func__, "Destroying modules");
    for (mod_destroy = __stop___k_mod_destroy - 1;
         mod_destroy >= __start___k_mod_destroy;
         --mod_destroy)
    {
        (*mod_destroy)();
    }
    log(LOG_INFO, __func__, "Modules destroyed");
}
