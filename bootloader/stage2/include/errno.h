/*
 * errno.h
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

extern int errno;

#define EIO     1    // I/O Error
#define ENOENT  2
#define EINVAL  3
#define EPERM   4
#define EBUSY   5
#define EBADF   6
#define EFAULT  7
#define ENOSYS  40   // Function not implemented

#endif /* __ERRNO_H__ */
