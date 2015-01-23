/*
 * fs.h
 */

#ifndef __FS_H__
#define __FS_H__

#include <io.h>
#include <types.h>


/* Opaque types */
typedef struct FSHandle FSHandle;
typedef struct DIR DIR;


/* File unique identifier */
typedef uint64_t ino_t;


/* An entry in a directory */
typedef struct {
    char name[255];
    size_t name_len;
    ino_t id;
} DirEntry;


/* Filesystem implementation */
typedef struct {
    /* Used to keep a linked list */
    void *next;
    /* FS Name */
    const char *name;
    /* Returns NULL if not supported */
    FSHandle* (*get_handle)(IODevice*);
    /* Release the handle */
    void (*release_handle)(FSHandle*);
    /* Returns the root directory id */
    ino_t (*get_root)(FSHandle*);
    /* Directory reading */
    DIR* (*opendir)(FSHandle*, ino_t);
    DirEntry* (*readdir)(FSHandle*, DIR*, DirEntry*);
    void (*closedir)(FSHandle*, DIR*);
} FileSystemImpl;


/* Register a filesystem implementation */
void fs_register(FileSystemImpl*);

/* Select a device as root directory */
int fs_set_root(IODevice*);

#endif /* __FS_H__ */
