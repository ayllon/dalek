/*
 * errno.h
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

extern int errno;

#define EIO     1    // I/O Error
#define EINVAL  2
#define EPERM   3
#define EBUSY   4
#define EBADF   5
#define ENOSYS  40   // Function not implemented

#endif /* __ERRNO_H__ */
