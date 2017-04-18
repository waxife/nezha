/**
 *  @file   errcode.h
 *  @brief  error code
 *  $Id: errcode.h,v 1.7 2014/04/15 01:39:39 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2013/09/23  hugo    New file.
 *
 */
#ifndef __ERRCODE_H
#define __ERRCODE_H

enum {
    E_UNKNOWN = 0,

    /* Filesystem */
    E_TW_SIGNATURE = 400,       /* Can not find TW signature */
    E_SD_UNSUPPORTED = 401,     /* SD card unsupported */
    E_FS_UNSUPPORTED = 402,     /* file system unsupported */
    E_FS_BUSY = 403,            /* file system is already mounted */
    E_FS_NOT_MOUNT = 404,       /* file system is not mounted */
    E_FS_MOUNT_CNT = 405,       /* file system count unmatched */
    E_FS_POLLUTED = 406,        /* file system is polluted */
    E_FS_SECT_SIZE = 407,       /* sector size is not 512 bytes */
    E_FS_OUT_OF_RANGE = 408,    /* access out of range in FAT table */
    E_FS_NO_SPACE = 409,        /* not enought space for reserved files */
    E_FS_ACCESS_DENY = 410,     /* no right to create file or mkdir */

    /* File */
    E_FILE_SIGNATURE = 420,     /* file without TW signature */
    E_FILE_LIMIT = 421,         /* too many files are opened */
    E_FILE_FLAG = 422,          /* invalid operation flags */
    E_FILE_BAD_FD = 423,        /* bad file descriptor */
    E_FILE_EXIST = 424,         /* file exists */
    E_FILE_NOT_EXIST = 425,     /* file doesn't exist */
    E_FILE_NAME = 426,          /* illegal file name */
    E_FILE_SIZE = 427,          /* illegal file size */
    E_FILE_OFFSET = 428,        /* illegal file offset */
    E_FILE_OVERFLOW = 429,      /* access out of file content */
    E_FILE_BAD_ATTRIB = 430,    /* bad file attribute */
    E_FILE_BAD_END = 431,       /* the last FAT entry is not EOC */
    E_FILE_NO_REGION = 432,     /* out of region for file */
    E_FILE_NO_SPACE = 433,      /* no free space for file */
    E_FILE_PROTECT = 434,       /* file cannot be removed */
    E_FILE_MODE = 435,          /* invalid access mode */
    E_FILE_OPEN = 436,          /* file is opened */
    E_FILE_NOT_OPEN = 437,      /* file is not opened */
    E_FILE_EXCEED = 438,        /* access out of range */
    E_FILE_IO = 439,            /* access I/O error */

    /* Dir */
    E_DIR_SIGNATURE = 440,      /* dir without TW signature */
    E_DIR_EXIST = 441,          /* dir exists */
    E_DIR_NOT_EXIST = 442,      /* dir doesn't exist */
    E_DIR_NAME = 443,           /* illegal dir name */
    E_DIR_NOT_EMPTY = 444,      /* dir contains entries other than . and .. */
    E_DIR_OUT_OF_RANGE = 445,   /* access out of range in dir */
    E_DIR_NO_SPACE = 446,       /* out of space for dir */
    E_DIR_NOT_RSV = 447,        /* create a resvert file none resvert directory */

    /* Codec */
    E_CODEC_FORMAT = 460,       /* bad format */
    E_CODEC_DATA = 461,         /* bad decode data */
    E_CODEC_AUDIO = 462,        /* audio decode failed */
    E_CODEC_VIDEO = 463,        /* video decode failed */

    /* Mise */
    E_INVALID_ARG = 480,        /* invalid arguments */
};

#endif /* __ERRCODE_H */
