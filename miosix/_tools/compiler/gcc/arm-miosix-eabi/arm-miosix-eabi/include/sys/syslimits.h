
#ifndef _SYS_SYSLIMITS_H
#define _SYS_SYSLIMITS_H

/* 
 * Max length of command line arguments (including environment),
 * POSIX requires at least 4096, but for now 1024 will do.
 */
#define ARG_MAX 1024

/*
 * Max nonblocking pipe read,
 * POSIX requires at least 512, but for now 128 will do.
 */
#define PIPE_BUF 128

#define NAME_MAX 255 /* Max filename, not including NUL, used for dirent */
#define PATH_MAX 512 /* Max filesystem path, including NUL */

#endif /* _SYS_SYSLIMITS_H */
