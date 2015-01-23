/*
 * fs.c
 */
#include <errno.h>
#include <fs.h>
#include <stdio.h>

static FileSystemImpl* fs_list = NULL;
static FSHandle* root = NULL;
static FileSystemImpl* root_fs = NULL;


void fs_register(FileSystemImpl* fs)
{
    fs->next = fs_list;
    fs_list = fs;
    log(LOG_DEBUG, __func__, "Registered filesystem %s", fs->name);
}


int fs_set_root(IODevice* dev)
{
    if (dev->read == NULL || dev->seek == NULL) {
        errno = EBADF;
        log(LOG_ERROR, __func__, "Invalid device: missing read and/or seek");
        return -1;
    }

    if (root) {
        root_fs->release_handle(root);
        root_fs = NULL;
        root = NULL;
    }

    FileSystemImpl* p = fs_list;
    root = NULL;
    while (p) {
        root = p->get_handle(dev);
        if (root) {
            root_fs = p;
            return 0;
        }
        p = p->next;
    }

    log(LOG_ERROR, __func__, "Filesystem not recognized");
    return -1;
}
