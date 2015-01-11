/*
 * errno.h
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

extern int errno;

#define EIO     1    // I/O Error
#define ENOENT  2    // No such entity
#define EINVAL  3    // Invalid value
#define EPERM   4    // Operation not permitted
#define EBUSY   5    // Resource busy
#define EBADF   6    // Bad file descriptor
#define EFAULT  7    // Invalid pointer
#define ETIME   8    // Timer expired
#define EAGAIN  9    // Try again
#define ENOSYS  40   // Function not implemented

#endif /* __ERRNO_H__ */
