#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <limits.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file was written to be compatible with the BSD directory
 * routines, so it looks like it.  But it was written from scratch.
 * Sean Eric Fagan, sef@Kithrup.COM.
 * Additionally modified for Miosix by Terraneo Federico, fede.tft@hotmail.it
 */

#define MAXNAMLEN NAME_MAX

typedef struct __DIR
{
    int dd_fd;
    long dd_loc;
    long dd_size;
    char *dd_buf;
    int dd_len;
    long dd_seek;
    void (*dd_onclose)(struct __DIR *);
} DIR;

struct dirent
{
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_type;
    char d_name[NAME_MAX + 1];
};

enum
{
    DT_UNKNOWN = 0,
    /* Equivalent to S_XXXX in sys/stat.h, but shifted to fit in a char */
    DT_FIFO = 0010000>>12,
    DT_CHR  = 0020000>>12,
    DT_DIR  = 0040000>>12,
    DT_BLK  = 0060000>>12,
    DT_REG  = 0100000>>12,
    DT_LNK  = 0120000>>12,
    DT_SOCK = 0140000>>12,
};

#define IFTODT(mode) (((mode) & 0170000)>>12)
#define DTTOIF(type) ((type)<<12)

#define __dirfd(dp) ((dp)->dd_fd)

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *, struct dirent *, struct dirent **);
void rewinddir(DIR *);
int closedir(DIR *);
void seekdir(DIR *dir, long loc);
long telldir(DIR *dir);

int scandir(const char *dirname, struct dirent *** namelist,
   int (*select)(const struct dirent *),
   int (*dcomp)(const struct dirent **, const struct dirent **));

#ifdef __cplusplus
}
#endif 

#endif
