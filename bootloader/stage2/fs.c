/*
 * fs.c
 */
#include <fs.h>

static FSImpl* fs_list = NULL;


void fs_register(FSImpl* fs)
{
    fs->p = fs_list;
    fs_list->p = fs;
}


int fs_set_root(IODevice* dev)
{
    return -1;
}
