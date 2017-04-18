/**
 *  @file   norfs.h
 *  @brief  NOR Filesystem
 *  $Id: norfs.h,v 1.37 2016/05/11 11:56:51 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.37 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2013/08/28  hugo    New file.
 *
 */
#ifndef __NORFS_H
#define __NORFS_H

#include <stdio.h>
#include <errcode.h>
#include <mconfig.h>
#include <fs.h>
#include <time.h>

#define NOR_BLKSIZ          (64 * 1024)
#define NOR_FILE_NUM        256

/* nor access mode */
#define NOR_MODE_UNKNOWN    0
#define NOR_MODE_RDONLY     1
#define NOR_MODE_WRONLY     2

/* nor record type */
#define NOR_TYPE_PHOTO          (MEDIA_PIC << 16)
#define NOR_TYPE_AUDIO          (MEDIA_AUMSG << 16)
#define NOR_TYPE(FILENAME)      ((FILENAME) & 0xffff0000)
#define NOR_SERIAL(FILENAME)    ((FILENAME) & 0x0000ffff)

#define NOR_PHOTO_FILE(SERIAL)  (NOR_TYPE_PHOTO | ((SERIAL) & 0xffff))
#define NOR_AUDIO_FILE(SERIAL)  (NOR_TYPE_AUDIO | ((SERIAL) & 0xffff))

/*
 * Time format
 *  bits 0-4    2-second count, valid value range 0-29 inclusive (0-58 seconds).
 *  bits 5-10   minutes, valid value range 0-59 inclusive.
 *  bits 11-15  hours, valid value range 0-23 inclusive.
 */
typedef unsigned short norfs_dire_time;

#define FAT_GET_SEC(x)      (((x) & 0x1F) * 2)
#define FAT_GET_MIN(x)      (((x) >> 5) & 0x3F)
#define FAT_GET_HOUR(x)     (((x) >> 11) & 0x1F)
#define FAT_SET_TIME(h,m,s) ((((h) & 0x1f) << 11) | (((m) & 0x3f) << 5) | ((s) / 2))

/*
 * Date format
 *  bits 0-4    day of month, valid value range 1-31 inclusive.
 *  bits 5-8    month of year, 1=January, valid value range 1-12 inclusive.
 *  bits 9-15   count of years from 1980, valid value range 0-127 inclusive.
 *              (1980-2107)
 */
typedef unsigned short norfs_dire_date;

#define FAT_GET_DAY(x)      ((x) & 0x1F)
#define FAT_GET_MONTH(x)    (((x) >> 5) & 0x0F)
#define FAT_GET_YEAR(x)     ((((x) >> 9) & 0x7F) + 1980)
#define FAT_SET_DATE(y,m,d) (((((y) - 1980) & 0x7f) << 9) | (((m) & 0x0f) << 5) | ((d) & 0x1f))

/* norfs record header (16 bytes)
 *  [3:0]   magic number    (0xdeadbeef)
 *  [5:4]   fileid          (not used)
 *  [6]     attribute       (user defined attribute, flagdown only)
 *  [7]     reserved
 *  [9:8]   create time
 *  [11:10] create date
 *  [15:12] record length   (0 ~ 4G-1)
 *  [63:16] file name
 */
#define NORFS_MAGIC   0xdeadbeef
struct norfs_header {
    unsigned int    magic;
    unsigned int    fileid;
    unsigned char   attribute;
    unsigned char   reserved;
    unsigned short  time;
    unsigned short  date;
    unsigned int    length;
    char            name[48];
} __attribute__ ((__packed__, aligned (4)));
#define NORFS_HEADER_SIZE   sizeof(struct norfs_header)

struct norfs_dirent {
    unsigned int    magic;
    unsigned int    fileid;
    unsigned char   attribute;
    unsigned char   reserved;
    unsigned short  time;
    unsigned short  date;
    unsigned int    length;
    char            name[NAMELEN+1];
    char            unused2[8]; /* for norfs_header alignment */

    unsigned int    address;    /* access NOR start address */

    /* double linked list */
    unsigned int    index;
    unsigned int    prev;
    unsigned int    next;
} __attribute__ ((__packed__, aligned (4)));
#define NORFS_DIRENT_SIZE   sizeof(struct norfs_dirent)

struct norfs_fd {
    struct norfs_dirent dirent[1];

    /* private */
    unsigned int    offset;     /* access offset */
    unsigned char   mode;       /* access mode */
} __attribute__ ((__packed__, aligned (4)));

struct norfs_dir {
    int             blk_start;
    int             file_num;   /* max file number by mconfig */
    int             file_len;   /* max file length by mconfig (byte) */

    /* cache for sorting file */
    unsigned char   file_index[NOR_FILE_NUM];
    unsigned char   file_count;
    unsigned int    file_lastid;

    /* double linked list */
    unsigned short  total;      /* number of valid files */
    unsigned int    curr;       /* current file block index */
    unsigned int    head;       /*  oldest file block index */
    unsigned int    tail;       /*  newest file block index */
};

struct norfs_stat {
    unsigned int    st_size;    /* file size */
    unsigned short  mtime;      /* time of last modification */
    unsigned short  mdate;      /* date of last modification */
    unsigned int    attribute;  /* user defined attribute */
    unsigned int    start;      /* access NOR start address */
};

struct norfs_partition {
    unsigned int magic;

    int start;    /* block start */
    int count;    /* block count */

    int file_num;
    int file_len; /* byte */
};
typedef struct norfs_partition NORFS_PARTITION;

/******************************************************************************
 * NOR Filesystem API
 *****************************************************************************/

/* debug only */
int nor_read (int offset, int length, char *buf);

/* directory */
int norfs_opendir (struct norfs_dir *dir, const char *name);
int norfs_closedir (struct norfs_dir *dir);
int norfs_cleandir (struct norfs_dir *dir);
struct norfs_dirent *norfs_readdir (struct norfs_dir *dir, struct norfs_dirent *dirent);

int norfs_gettotal (struct norfs_dir *dir, int *total, int *npick, int filter, int mask);
int norfs_getdirent (struct norfs_dir *dir, struct norfs_dirent *dirent, int index);
int norfs_setdirent (struct norfs_dir *dir, struct norfs_dirent *dirent, int index);

/* file */
int norfs_open (struct norfs_fd *fd, const char *name, int size, int mode);
int norfs_close (struct norfs_fd *fd);
int norfs_remove (const char *name);
int norfs_read (struct norfs_fd *fd, void *buf, int size);
int norfs_write (struct norfs_fd *fd, void *buf, int size);
int norfs_seek (struct norfs_fd *fd, int offset);
int norfs_stat (const char *name, struct norfs_stat *st);
int norfs_setattr (const char *name, int attribute);
int norfs_clearattr (const char *name, int attribute);

#define MAP_FAILED  ((void *) -1)
void *norfs_mmap (struct norfs_fd *fd, int offset);

/* filesystem */
int norfs_set_partition (int media, struct norfs_partition *p);
int norfs_get_partition (int media, struct norfs_partition *p);
int norfs_format_partition (void);

void norfs_init (void);

/* deprecated API */
int norfs_forward (struct norfs_dir *dir);
int norfs_backward (struct norfs_dir *dir);
void norfs_pos2head (void);
void norfs_pos2tail (void);

#endif /* __NORFS_H */
