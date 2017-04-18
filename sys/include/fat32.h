/**
 *  @file   fat32.h
 *  @brief  header file for simple FAT32 filesystem 
 *  $Id: fat32.h,v 1.73 2016/05/11 11:56:51 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.73 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/02/06  New file.
 *
 */
#ifndef __FAT32_H
#define __FAT32_H

#include <sys.h>
#include <sd.h>
#include <fs.h>
#include <device.h>
#include <errcode.h>

#define FATE_SIZE           4
#define DIRE_SIZE           32
#define SECT_SIZE           512
#define CLUS_SIZE           (64*512)                    // 32K
#define DIR_SIZE            (160*1024)                  // 160K
#define DIR_SIZE_NOSIG      CLUS_SIZE                   // 32K
#ifdef CONFIG_FAT_SEG_SIZE
#define SEG_SIZE            (ROUNDUP(CONFIG_FAT_SEG_SIZE, 64) * 1024)
#else
#define SEG_SIZE            (64*1024)                   // 32K
#endif

#define MAX_FILE_CNT        8   /* the max number of opening files */
#define MAX_DIR_CNT         16  /* the max number of dirs */
#define REGION_NUM          8   /* the max number of spare regions */
#define RSVDIR_NUM          8   /* the max number of reserved directories */

#define FATE_PER_SECT       (SECT_SIZE / FATE_SIZE)     // 512/4    = 128
#define DIRE_PER_SECT       (SECT_SIZE / DIRE_SIZE)     // 512/32   = 16
#define SECT_PER_CLUS       (CLUS_SIZE / SECT_SIZE)     // 32K/512  = 64
#define CLUS_PER_DIR        (DIR_SIZE / CLUS_SIZE)      // 160K/32K = 5
#define SECT_PER_DIR        (DIR_SIZE / SECT_SIZE)      // 160K/512 = 320
#define DIRE_PER_DIR        (DIR_SIZE / DIRE_SIZE)      // 160K/32  = 5120
/* for no signature directory, support only 1 cluster dirsize */
#define CLUS_PER_DIR_NOSIG  (CLUS_SIZE / CLUS_SIZE)     // 32K/32K  = 1
#define SECT_PER_DIR_NOSIG  (CLUS_SIZE / SECT_SIZE)     // 32K/512  = 64
#define DIRE_PER_DIR_NOSIG  (CLUS_SIZE / DIRE_SIZE)     // 32K/32   = 1024

#define SECT_PER_SEG        (SEG_SIZE / SECT_SIZE)      // 32M/512  = 65536
#define CLUS_PER_SEG        (SEG_SIZE / CLUS_SIZE)      // 32M/32K  = 1024

#define FIRST_ROOTCLUSTER   2   /* the first data cluster is 2 */

#define XBUF_SIZE           (16*1024)                   // 16K
#define SECT_PER_XBUF       (XBUF_SIZE / SECT_SIZE)     // 16K/512  = 32
#define FATE_PER_XBUF       (XBUF_SIZE / FATE_SIZE)     // 16K/4    = 4096
#define DIRE_PER_XBUF       (XBUF_SIZE / DIRE_SIZE)     // 16K/32   = 512

/* partition table structure */
struct partition {
    unsigned int    BootIndicator:8;
    unsigned int    StartingHead:8;
    unsigned int    StartingSector:6;
    unsigned int    StartingCylinder:10;
    
    unsigned int    SystemID:8;
    unsigned int    EndingHead:8;
    unsigned int    EndingSector:6;
    unsigned int    EndingCylinder:10;
    
    unsigned int    RelativeSector;
    unsigned int    TotalSector;
};

/* Boot Sector and BPB (BIOS Parameter Block) Structure */
struct bootsector {
    unsigned char   JumpCommand[3];             //0
    unsigned char   SystemIdentifier[8];        //3
    unsigned short  SectorSize;                 //11
    unsigned char   SectorPerCluster;           //13
    unsigned short  ReservedSectorCount;        //14
    unsigned char   FATNumber;                  //16
    unsigned short  RootDirEntryNumber;         //17
    unsigned short  TotalSector16;              //19
    unsigned char   MediumIdentifier;           //21
    unsigned short  FATSz16;                    //22
    unsigned short  SectorPerTrack;             //24
    unsigned short  SlideNumber;                //26
    unsigned int    HiddenSectorNumber;         //28
    unsigned int    TotalSector32;              //32

    /* FAT32 structure starting at offset 36 */
    unsigned int    FATSz32;                    //36
    unsigned short  ExtFlags;                   //40
    unsigned short  FSVer;                      //42
    unsigned int    RootClus;                   //44
    unsigned short  FSInfo;                     //48
    unsigned short  BkBootSec;                  //50
    unsigned char   Reserved2[12];              //52
    unsigned char   FAT32_PhyDiskNumber;        //64
    unsigned char   FAT32_Reserved0;            //65
    unsigned char   FAT32_ExtendSignature;      //66
    unsigned int    FAT32_VolumeID;             //67
    char            FAT32_VolumeLable[11];      //71
    char            FAT32_FileSystemType[8];    //82
    unsigned short  TWSignatureWord;            //90
    unsigned int    DirSize;                    //92    (num of clusters)
    unsigned int    DirNumber;                  //96
    unsigned int    SegmentSize;                //100   (num of clusters)
    unsigned int    SegmentNumber;              //104
    struct {
    	unsigned int    RsvFileSize;            //108   (num of clusters)
    	int             RsvFileNumber;          //112
    } e [RSVDIR_NUM];
    unsigned char   Reserved1[338];             //172
    unsigned short  SignatureWord;              //510
} __attribute__ ((__packed__, aligned (4)));


/* 
 * Time format
 *  bits 0-4    2-second count, valid value range 0-29 inclusive (0-58 seconds).
 *  bits 5-10   minutes, valid value range 0-59 inclusive.
 *  bits 11-15  hours, valid value range 0-23 inclusive.
 */
typedef unsigned short dire_time;

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
typedef unsigned short dire_date;

#define FAT_GET_DAY(x)      ((x) & 0x1F)
#define FAT_GET_MONTH(x)    (((x) >> 5) & 0x0F)
#define FAT_GET_YEAR(x)     ((((x) >> 9) & 0x7F) + 1980)
#define FAT_SET_DATE(y,m,d) (((((y) - 1980) & 0x7f) << 9) | (((m) & 0x0f) << 5) | ((d) & 0x1f))

/* FAT 32byte directory entry structure */
typedef struct dire {
    char            Name[8];            //0
    char            Extension[3];       //8
    unsigned char   Attrib;             //11
    unsigned short  Next;               //12    /* next dire */
    unsigned char   signature;          //14
    unsigned char   Attribute;          //15    /* user defined attribute */
    unsigned short  Prev;               //16    /* previous dire */
    unsigned short  Total;              //18    /* total files keeped in . */
    unsigned short  StartClusterEx;     //20
    unsigned short  Time;               //22
    unsigned short  Date;               //24
    unsigned short  StartCluster;       //26
    unsigned int    FileLength;         //28
} __attribute__((__packed__, aligned(4))) dire_t;

#define ATTRIB_READ_ONLY                    0x01
#define ATTRIB_HIDDEN                       0x02
#define ATTRIB_SYSTEM                       0x04
#define ATTRIB_VOLUME_ID                    0x08
#define ATTRIB_DIRECTORY                    0x10
#define ATTRIB_ARCHIVE                      0x20
#define ATTRIB_PROTECT                      0x40
#define ATTRIB_LONG_NAME                    0x0F

#define FAT_GET_CLUS(x) (((x)->StartClusterEx << 16) | ((x)->StartCluster))

struct dirent {
    /* mirror of dire */
    char            Name[NAMELEN+1];
    unsigned int    StartCluster;
    unsigned int    Length;
    dire_time       Time;
    dire_date       Date;
    unsigned char   Attrib;
    unsigned char   Attribute;

    /* double linked list */
    char            signature;
    char            dire_count;
    unsigned short  dire_index;
    unsigned short  dire_prev;
    unsigned short  dire_next;
    unsigned short  file_total;

    /* for dirent cache */
    unsigned int    dirCluster;
};

struct dir {
    struct dirent   dirent[1];

    /* double linked list */
    unsigned short  total;      /* number of valid files */
    unsigned short  curr;       /* current file dire index */
    unsigned short  head;       /*  oldest file dire index */
    unsigned short  tail;       /*  newest file dire index */
};

struct stat_t {
    unsigned int    st_size;    /* file size */
    unsigned short  mtime;      /* time of last modification */
    unsigned short  mdate;      /* date of last modification */
    unsigned int    cluster;    /* start cluster */
    unsigned char   attrib;     /* file attrib */
    unsigned char   attribute;  /* user defined attribute */
};

struct statfs_t {
    unsigned int    f_bsize;    /* optimal transfer block size */
    unsigned int    f_blocks;   /* total data blocks in file system */
    unsigned int    f_bfree;    /* free blocks in fs */
    unsigned int    f_namelen;  /* maximum length of filenames */

    struct {
        unsigned int    rsvf_len;   /* reserved file length */
        int             rsvf_num;   /* reserved file number */
    } e[RSVDIR_NUM];
};

struct region {
    unsigned int    start;
    unsigned int    count;
};

struct fd32 {
    unsigned int    magic;
    unsigned short  mnt_cnt;        /* to check fat is valid or invalid */

    unsigned int    length;         /* file size (4k-aligned) */
    unsigned int    size;           /* file size */
    unsigned int    offset;         /* read/write file offset */
    unsigned char   flag;
#define FD_UNKNOWN          0
#define FD_RDONLY           (1 << 0)
#define FD_WRONLY           (1 << 1)
#define FD_NOCHECKEXIST     (1 << 2)
#define FD_CLEAR1K          (1 << 3)

    unsigned int    dir_clus;       /* cluster of parent directory */
    unsigned int    dire_index;
    unsigned int    start_clus;
    unsigned int    start_lba;

    /* spare region */
    struct region   region[REGION_NUM];
    unsigned char   region_cnt;
};

struct fat32 {
    unsigned int    magic;
    unsigned int    sect_per_fat;       /* fat table size (sectors) */
    unsigned int    sect_per_clus;      /* cluster size (sectors) */
    unsigned int    lba_start;          /* 1st sector of reserved region */
    unsigned int    fat_start;          /* 1st sector of FAT region */
    unsigned int    root_start;         /* 1st sector of data region */
    unsigned int    nsector;            /* total sector number (disk size) */
    volatile unsigned long lock;        /* I/O lock */

    unsigned short  mnt_cnt;            /* to check fat is valid or invalid */

    char            cur_dir[PATHLEN+1]; /* current work directory */
    char            is_readonly;
    // char            check_tw_signature;

    /* reserved */
    struct {
    	unsigned int    rsvf_size;          /* # of clusters */
        int             rsvf_num;
    } e[RSVDIR_NUM];

    /* FAT table management */
    unsigned int    c_dir;              /* dir partition current cluster */
    unsigned int    s_dir;              /* dir partition start cluster */
    unsigned int    n_dir;              /* dir partition # of clusters */
    unsigned int    s_seg;              /* seg partition start cluster */
    unsigned int    n_seg;              /* seg partition # of clusters */
    unsigned int    s_rsv;              /* rsv partition start cluster */
    unsigned int    n_rsv;              /* rsv partition # of clusters */
    unsigned int    e_fat;              /* end of namaged fate */

    /* cache of FAT entries */
    unsigned int    fate_cache[FATE_PER_SECT] __attribute__ ((aligned (4)));
    unsigned int    fate_cache_sector;  /* sector # of fat entry cache */
    unsigned int    fate_cache_taint;

    /* memo next free dirent for search quickly */
    unsigned int    memo_startcluster;
    unsigned int    memo_updateindex;
};

/******************************************************************************
 * FAT32 API
 *****************************************************************************/

/* debug only */
void fate_used (void);
void fate_free (void);
void fate_dump (unsigned int clus, unsigned int count);
int fate_trace (unsigned int clus);

int fat_info (void);

int dirent_find_by_name (struct dir *dir, const char *name, struct dirent *dirent);
int dirent_find_by_index (struct dir *dir, const int index, struct dirent *dirent);

/* file system */
int _format (void);
int _mount (struct fat_t *fat);
int _upgrade_mount (struct fat_t *fat);
int _umount (void);
int _ismounted(void);
int _statfs (struct statfs_t *st);

/* directory */
int _mkdir (const char *name);
int _rmdir (const char *name);
int _chdir (const char *name);
char *_getcwd (char *buf, size_t size);
int _opendir (struct dir *dir, const char *name);
int _closedir (struct dir *dir);
struct dirent *_readdir (struct dir *dir, struct dirent *dirent);

int _gettotal (struct dir *dir, int *total, int *npick, int filter, int mask);
int _getdirent (struct dir *dir, struct dirent *dirent, int dire_index);
int _setdirent (struct dir *dir, struct dirent *dirent, int dire_index);

/* file */
int _open (struct fd32 *fd, const char *name, unsigned int size, unsigned char flag);
int _close (struct fd32 *fd);
int _remove (const char *name);
int _rename (const char *oldname, const char *newname);
int _recycle (const char *oldname, const char *newname);
int _read (struct fd32 *fd, void *buf, unsigned int size);
int _write (struct fd32 *fd, void *buf, unsigned int size);
int _seek (struct fd32 *fd, unsigned int offset);
int _stat (const char *name, struct stat_t *st);
int _setattr (const char *name, int attribute);
int _clearattr (const char *name, int attribute);

/* system */
int fat_init (void);
void fat_check_tw_signature (int on);

/* resvert file system */
int _rsv_create (struct fd32 *fd, const char *name);
int _rsv_close (struct fd32 *fd);

int _fatfs_get_partition(int media, int *pfiles, int *psize);
int scan_dll (const char *dirname);
int rebuild_dll (const char *dirname);


/******************************************************************************
 * FAT32 API (backward compatible)
 *****************************************************************************/

#include <stdint.h>
#define FS2_RDONLY  FD_RDONLY
#define FS2_WRONLY  FD_WRONLY

struct fat_t {
    struct fat32    fat[1];

    /********** backward compatible **********/
    struct sdc      hcard[1];
    drive_op_t      *op;
    uint32_t        root_start;
    uint32_t        sect_per_clus;
    volatile unsigned long lock;
};

struct region_t {
    uint32_t        start;
    uint32_t        end;
};

struct fs2_t {
    struct fd32     fd[1];

    /********** backward compatible **********/
    struct fat_t    *pfat;
    uint32_t        rw_size;            /* record read/write size in the file */
    uint32_t        file_size;          /* file size */
    uint32_t        cur_clus;           /* current cluster for wirte */
    uint32_t        cur_free_sect;      /* free sector # in current cluster */
    uint32_t        cur_lba;            /* current lba for encode engine */
    uint32_t        reg_rsize;          /* the residue size of current region */
    uint32_t        start_clus;

    struct region_t region[REGION_NUM]; /* info of spare region */
    uint32_t        valid_region_cnt;   /* valid spare count */
    uint8_t         cur_reg;            /* current region idx */
};

struct dire_info {
    char        LongName[NAMELEN+1];
    uint32_t    StartCluster;
    uint32_t    Length;
    dire_time   Time;
    dire_date   Date;
    uint8_t     Attrib;
    uint8_t     Attribute;
};

struct DIR {
    struct dir dir[1];
    struct dire_info dire_info[1];
};

/* system */
int fat_drv_init (void);

#endif
