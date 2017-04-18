/**
 *  @file   sh_mconfig_test.c
 *  @brief  Medium Storage Config
 *  $Id: sh_fat32tt.c,v 1.8 2014/08/07 06:06:40 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  hugo      New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <keymap.h>
#include <sys.h>
#include <ctype.h>
#define DBG_LEVEL   0
#include <debug.h>
#include <mconfig.h>
#include <fat32.h>

command_init (sh_fat32tt, "fat32tt", "fat32tt");
#define BUF_SIZE 8192

int mkrsvfile(char *dir, char *name, int size, char data)
{
    int rc;
    struct fd32 fd[1];
    char *buf= (char *) CONFIG_STACK_ADDR_RSV_BUF;
    char fname[64];

    memset(fd, 0, sizeof(struct fd32));
    
    strncpy(fname, dir, 64);
    strncat(fname, "/", 64);
    strncat(fname, name, 64);

    printf("mkrsvfile %s %d", fname, size);

    rc = _open(fd, fname, 0, FD_WRONLY);
    if (rc < 0) {
	printf("_open, rc = %d\n", rc);
	goto EXIT;
    }
    memset(buf, data, BUF_SIZE);
    for (; size > BUF_SIZE; size -= BUF_SIZE) {
	putchar('.');
        rc = _write(fd, buf, BUF_SIZE);
        if (rc < 0) {
            printf("_write(), rc = %d\n", rc);
            goto EXIT;
        }
    }
    if (size > 0) {
	putchar('<');
        rc = _write(fd, buf, size);
        if (rc < 0) {
	    ERROR("_write(), rc = %d\n", rc);
	    goto EXIT;
	}
    }
    printf("  ok\n");
    _close(fd);
    return 0;

EXIT:
    _close(fd);
    return rc;
}

int memccmp(char *src, char data, int size)
{
    int i;
    for (i = 0; i < size ; i++) {
        if (src[i] != data) {
	    printf("src[%d] %x / %x\n", i, src[i], data);
            return -1;
	}
    }
    return 0;
}

int chkrsvfile(char *dir, char *name, int size, char data)
{
    int rc;
    struct fd32 fd[1];
    char *buf= (char *) CONFIG_STACK_ADDR_RSV_BUF;
    char fname[64];
    struct stat_t st;

    memset(fd, 0, sizeof(struct fd32));
    
    strncpy(fname, dir, 64);
    strncat(fname, "/", 64);
    strncat(fname, name, 64);

    printf("chkrsvfile %s %d ", fname, size);

    rc = _stat(fname, &st);
    if (rc < 0) {
        ERROR("_stat(), rc=%d\n", rc);
        goto EXIT;
    }
    assert(size == st.st_size);

    rc = _open(fd, fname, 0, FS2_RDONLY);
    if (rc < 0) {
        ERROR("_open(), rc = %d\n", rc);
        goto EXIT;
    }	

    for (; size > BUF_SIZE; size -= BUF_SIZE) {
        putchar('.');
        rc = _read(fd, buf, BUF_SIZE);
        if (rc < 0) {
            ERROR("_read(), rc = %d\n", rc);
            goto EXIT;
	}
        rc = memccmp(buf, data, BUF_SIZE);
	if (rc < 0) {
            ERROR("data error\n");
	    goto EXIT;
	}
    }
    if (size > 0) {
	putchar('<');
        rc = _read(fd, buf, size);
        if (rc < 0) {
            ERROR("_read(), rc = %d\n", rc);
            goto EXIT;
        }
        rc = memccmp(buf, data, size);
	if (rc < 0) {
            ERROR("data error\n");
            goto EXIT;
	}
    }

   printf("   pass\n");
  _close(fd);
  return 0;

EXIT:
   _close(fd);
   return rc;
}

static int
sh_fat32tt (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct sysc_t *sysc = sys_ctx;
    struct fat_t *fat = sysc->pfat; 
    int i;
    int rc;
    printf("mconfig test program is a test driver\n");
    printf("to test mconfig, fatfs and resv_file system\n");
   
    media_config(FATFS, MEDIA_PIC, "PICTURE", 1024, 64*1024);
    media_config(FATFS, MEDIA_AUMSG, "aumsg", 64, 128*1024);
    media_config(FATFS, MEDIA_VIDEO, "video", 128, 32*1024*1024);

    rc = _format();
    if (rc < 0) {
       ERROR("_format rc = %d\n", rc);
    }
    assert(rc == 0);
    rc = _mount(fat);
    assert(rc == 0);
   
    //rc = mkrsvfile("picture", "0", 18*1024, 0); 
    rc = mkrsvfile("picture", "2014-01-09 19:10:25-0", 18*1024, 0); 
    assert(rc == 0);

    //rc = chkrsvfile("picture", "0", 18*1024, 0); 
    rc = chkrsvfile("picture", "2014-01-09 19:10:25-0", 18*1024, 0);
    assert(rc == 0); 

    for (i= 0; i < 16; i++) {
	    char fname[64];
        snprintf(fname, 64, "2014-01-09 19:11:12-%d", i);
        //snprintf(fname, 64, "%d", i);
	    rc = mkrsvfile("aumsg", fname, 69*1024, i);
	    assert(rc == 0);
    }

    for (i = 0; i < 16; i++) {
        char fname[64];
        snprintf(fname, 64, "2014-01-09 19:11:12-%d", i);
        //snprintf(fname, 64, "%d", i);
        rc = chkrsvfile("aumsg", fname, 69*1024, i);
        assert(rc == 0);
    }

 
    return 0;
}

/*****************************************************************************/

command_init (sh_fatfs_test_rsvf, "fatfs_test_rsvf", "fatfs_test_rsvf");
command_init (sh_fatfs_test_function, "fatfs_test_function", "fatfs_test_function");
command_init (sh_fatfs_test_exception, "fatfs_test_exception", "fatfs_test_exception");
command_init (sh_fatfs_test_boundary, "fatfs_test_boundary", "fatfs_test_boundary");
command_init (sh_fatfs_test_performance, "fatfs_test_performance", "fatfs_test_performance");
command_init (sh_fatfs_test_longrun, "fatfs_test_longrun", "fatfs_test_longrun [<file num>]");
command_init (sh_fatfs_test_compatible, "fatfs_test_compatible", "fatfs_test_compatible");
command_init (sh_fatfs_test_open, "fatfs_test_open", "fatfs_test_open");

#define LFNAME      "longnamefile%04d.txt"
#define SFNAME      "file%04d.txt"

#define FILE_NUM    1024
#define FILE_SIZE   (64*1024)
#define NAMESIZE    128
#define MULTI_MAX   8

static int fat32_debug = 0;

#ifdef FAT32_DEBUG
#define STATISTICS_RESET    do { sd_rcnt = sd_wcnt = 0; } while (0)
#define STATISTICS_INFO     printf("R=%d, W=%d\n", sd_rcnt, sd_wcnt)
#else
static unsigned long ticks;
#define STATISTICS_RESET    do { ticks = read_c0_count32(); } while (0)
#define STATISTICS_INFO     do { ticks = read_c0_count32() - ticks; printf ("%4lums\n", ticks/(sys_cpu_clk/2/1000)); } while (0)
#endif

#ifndef FAT32_DEBUG
static int
test_continue (void)
{
    int rc = 0;

    printf ("\n!!! CAUTION !!!\n");
    printf ("fatfs test will destroy the content of SD card.\n");
    printf ("Do you want to continue? (y/n) ");
    while (1) {
        char c = getchar ();
        putchar (c);

        c = toupper (c);
        if (c == 'Y') {
            rc = 1;
            break;
        } else
        if (c == 'N') {
            rc = 0;
            break;
        }
    }
    printf ("\n");

    if (rc)
        _umount ();

    return rc;
}
#endif

static int
test_format (void)
{
    int rc;

    printf ("# format\n");
    rc = _format ();
    if (rc < 0) {
        ERROR ("_format(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_mount (void)
{
    int rc;

    printf ("# mount\n");
    rc = _mount (NULL);
    if (rc < 0) {
        ERROR ("_mount(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_umount (void)
{
    int rc;

    printf ("# umount\n");
    rc = _umount ();
    if (rc < 0) {
        ERROR ("_umount(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_statfs (void)
{
    struct statfs_t st;
    int rc;

    printf ("# statfs\n");
    rc = _statfs (&st);
    if (rc < 0) {
        ERROR ("_statfs(), rc=%d\n", rc);
        return rc;
    }

    printf ("%10s %10s %10s %10s %10s\n",
            "Filesystem", "1K-blocks", "Used", "Available", " Use%");
    printf ("%10s %10u %10u %10u %9u%%", "/",
            st.f_blocks * (st.f_bsize/1024),
            (st.f_blocks - st.f_bfree) * (st.f_bsize/1024),
            st.f_bfree * (st.f_bsize/1024),
            (st.f_blocks - st.f_bfree) * 100 / st.f_blocks);
    printf ("\n");

    return 0;
}

static int
test_mkdir (char *name)
{
    int rc;

    printf ("# mkdir %s\n", name);
    rc = _mkdir (name);
    if (rc < 0) {
        ERROR ("_mkdir(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_rmdir (char *name)
{
    int rc;

    printf ("# rmdir %s\n", name);
    rc = _rmdir (name);
    if (rc < 0) {
        ERROR ("_rmdir(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_pwd (void)
{
    char buf[63+1], *cwd;

    printf ("# pwd\n");
    cwd = _getcwd (buf, 63);
    if (cwd == NULL) {
        ERROR ("_getcwd()\n");
        return -1;
    }
    printf ("  %s\n", cwd);

    return 0;
}

static int
test_cd (char *name)
{
    int rc;

    printf ("# cd %s\n", name);
    rc = _chdir (name);
    if (rc < 0) {
        ERROR ("_chdir(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_dir (char *name)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int num_file = 0, num_dir = 0;
    char *type;
    int rc;

    printf ("# dir %s\n", name);
    rc = _opendir (dir, name);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return rc;
    }

    while ((_readdir (dir, dirent))) {
        if (dirent->Attrib == ATTRIB_DIRECTORY) {
            num_dir++;
            type = "<DIR>";
        } else {
            num_file++;
            type = "     ";
        }

        printf ("%04d-%02d-%02d %02d:%02d:%02d  %8d  %8d  %s  %s\n",
            FAT_GET_YEAR (dirent->Date),
            FAT_GET_MONTH (dirent->Date),
            FAT_GET_DAY (dirent->Date),
            FAT_GET_HOUR (dirent->Time),
            FAT_GET_MIN (dirent->Time),
            FAT_GET_SEC (dirent->Time),
            dirent->Length, dirent->StartCluster, type, dirent->Name);
    }
    printf ("%d files\n", num_file);
    printf ("%d dirs\n", num_dir);

    rc = _closedir (dir);
    if (rc < 0) {
        ERROR ("_closedir(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

#define test_remove_sfn(idx, cnt)       test_remove(idx, cnt, 0)
#define test_remove_lfn(idx, cnt)       test_remove(idx, cnt, 1)
static int
test_remove (int idx, int cnt, int lfn)
{
    char name[NAMESIZE];
    int i;
    int rc;

    for (i = idx; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# remove %s\n", name);

        STATISTICS_RESET;
        rc = _remove (name);
        if (fat32_debug) STATISTICS_INFO;
        if (rc < 0) {
            ERROR ("_remove(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

#define test_stat_sfn(idx, cnt)         test_stat(idx, cnt, 0)
#define test_stat_lfn(idx, cnt)         test_stat(idx, cnt, 1)
static int
test_stat (int idx, int cnt, int lfn)
{
    char name[NAMESIZE];
    struct stat_t st;
    int i;
    int rc;

    for (i = 0; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# stat %s\n", name);

        rc = _stat (name, &st);
        if (rc < 0) {
            ERROR ("_stat(), rc=%d\n", rc);
            return rc;
        }

        printf ("    File: \'%s\'\n", name);
        printf ("    Size: %u\n", st.st_size);
        printf ("  Attrib: 0x%02x\n", st.attrib);
        printf (" UAttrib: 0x%02x\n", st.attribute);
        printf (" Cluster: %u\n", st.cluster);
        printf ("  Modify: %04d-%02d-%02d %02d:%02d:%02d\n",
                FAT_GET_YEAR (st.mdate),
                FAT_GET_MONTH (st.mdate),
                FAT_GET_DAY (st.mdate),
                FAT_GET_HOUR (st.mtime),
                FAT_GET_MIN (st.mtime),
                FAT_GET_SEC (st.mtime));
    }

    return 0;
}

#define FSIZE   FILE_SIZE
#define test_write_sfn(idx, cnt, p)     test_write(idx, cnt, FSIZE, FSIZE * p, 0)
#define test_write_lfn(idx, cnt, p)     test_write(idx, cnt, FSIZE, FSIZE * p, 1)
static int
test_write (int idx, int cnt, int open_sz, int write_sz, int lfn)
{
    struct fd32 fd[1];
    char name[NAMESIZE];
    char wbuf[SECT_SIZE];
    int i, j;
    int rc;

    for (i = idx; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# write %s\n", name);

        memset (fd, 0, sizeof (struct fd32));

        /* open WRONLY */
        STATISTICS_RESET;
        rc = _open (fd, name, open_sz, FD_WRONLY);
        if (fat32_debug) STATISTICS_INFO;
        if (rc < 0) {
            ERROR ("_open(), rc=%d\n", rc);
            return rc;
        }

        /* write */
        for (j = 0; j < (write_sz/SECT_SIZE); j++) {
            memset (wbuf, i + j, SECT_SIZE);

            rc = _write (fd, wbuf, SECT_SIZE);
            if (rc < 0) {
                ERROR ("_write(), rc=%d\n", rc);
                return rc;
            }
        }

        /* close */
        rc = _close (fd);
        if (rc < 0) {
            ERROR ("_close(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

#define test_mwrite_sfn(idx, cnt)       test_mwrite(idx, cnt, 0)
#define test_mwrite_lfn(idx, cnt)       test_mwrite(idx, cnt, 1)
static int
test_mwrite (int idx, int cnt, int lfn)
{
    struct fd32 fd[MULTI_MAX];
    char name[NAMESIZE];
    char wbuf[SECT_SIZE];
    int i, j;
    int rc;

    if (cnt > MULTI_MAX) {
        ERROR ("too many files (%d)\n", cnt);
        return -1;
    }

    memset (fd, 0, sizeof (struct fd32) * cnt);

    /* open WRONLY */
    for (i = idx; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# write %s\n", name);
        rc = _open (&fd[i-idx], name, FILE_SIZE, FD_WRONLY);
        if (rc < 0) {
            ERROR ("_open(), rc=%d\n", rc);
            return rc;
        }
    }

    /* write */
    for (i = idx; i < (idx + cnt); i++) {
        for (j = 0; j < FILE_SIZE; j += SECT_SIZE) {
            memset (wbuf, i + j, SECT_SIZE);
            rc = _write (&fd[i-idx], wbuf, SECT_SIZE);
            if (rc < 0) {
                ERROR ("_write(), rc=%d\n", rc);
                return rc;
            }
        }
    }

    /* close */
    for (i = idx; i < (idx + cnt); i++) {
        rc = _close (&fd[i-idx]);
        if (rc < 0) {
            ERROR ("_close(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

#define test_cwrite_sfn(idx, cnt)       test_cwrite(idx, cnt, 0)
#define test_cwrite_lfn(idx, cnt)       test_cwrite(idx, cnt, 1)
int
test_cwrite (int idx, int cnt, int lfn)
{
    struct fd32 fd[2], *fd_this, *fd_last;
    char name[NAMESIZE];
    char wbuf[SECT_SIZE];
    int i, j;
    int rc;

    /*      |<----------------->|<----------------->|<----------------->|
     *             ROUND 1             ROUND 2             ROUND 3
     *      
     *  ----|---------| <SKIP>
     *          3.W  4.C
     *
     * <F1> |---------|---------|---------|
     *     1.O  2.W       5.W       8.W  9.C
     *
     *                     <F2> |---------|---------|---------|
     *                         6.O  7.W      10.W      13.W 14.C
     *
     *                                       <SKIP> |---------|---------|----
     *                                            11.O 12.W      15.W
     */
    for (i = idx; i < (idx + cnt + 1); i++) {

        sprintf (name, lfn ? LFNAME : SFNAME, i);
        fd_this = fd + (i % 2);
        fd_last = fd + ((i-1) % 2);

        /* 1. Open This */
        if (i < (idx + cnt)) {
            printf ("# write %s\n", name);
            memset (fd_this, 0, sizeof (struct fd32));

            rc = _open (fd_this, name, FILE_SIZE, FD_WRONLY);
            if (rc < 0) {
                ERROR ("_open(), rc=%d\n", rc);
                return rc;
            }
        }

        /* 2. Write This */
        if (i < (idx + cnt)) {
            for (j = 0; j < SECT_SIZE; j += SECT_SIZE) {
                memset (wbuf, i + j, SECT_SIZE);

                rc = _write (fd_this, wbuf, SECT_SIZE);
                if (rc < 0) {
                    ERROR ("_write(), rc=%d\n", rc);
                    return rc;
                }
            }
        }

        /* 3. Write Last */
        if (i > idx) {
            for (j = (FILE_SIZE - SECT_SIZE); j < FILE_SIZE; j += SECT_SIZE) {
                memset (wbuf, i + j, SECT_SIZE);

                rc = _write (fd_last, wbuf, SECT_SIZE);
                if (rc < 0) {
                    ERROR ("_write(), rc=%d\n", rc);
                    return rc;
                }
            }
        }

        /* 4. Close Last */
        if (i > idx) {
            rc = _close (fd_last);
            if (rc < 0) {
                ERROR ("_close(), rc=%d\n", rc);
                return rc;
            }
        }

        /* 5. Write This */
        if (i < (idx + cnt)) {
            for (j = SECT_SIZE; j < (FILE_SIZE - SECT_SIZE); j += SECT_SIZE) {
                memset (wbuf, i + j, SECT_SIZE);

                rc = _write (fd_this, wbuf, SECT_SIZE);
                if (rc < 0) {
                    ERROR ("_write(), rc=%d\n", rc);
                    return rc;
                }
            }
        }
    }

    return 0;
}

#define test_read_sfn(idx, cnt)         test_read(idx, cnt, 0)
#define test_read_lfn(idx, cnt)         test_read(idx, cnt, 1)
static int
test_read (int idx, int cnt, int lfn)
{
    struct fd32 fd[1];
    char name[NAMESIZE];
    char rbuf[SECT_SIZE], wbuf[SECT_SIZE];
    int i, j;
    int rc;

    for (i = idx; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# read %s\n", name);

        memset (fd, 0, sizeof (struct fd32));

        /* open RDONLY */
        STATISTICS_RESET;
        rc = _open (fd, name, FILE_SIZE, FD_RDONLY);
        if (fat32_debug) STATISTICS_INFO;
        if (rc < 0) {
            ERROR ("_open(), rc=%d\n", rc);
            return rc;
        }

        /* read */
        for (j = 0; j < (FILE_SIZE/SECT_SIZE); j++) {
            memset (wbuf, i + j, SECT_SIZE);

            rc = _read (fd, rbuf, SECT_SIZE);
            if (rc < 0) {
                ERROR ("_read(), rc=%d\n", rc);
                return rc;
            }

            if (memcmp (rbuf, wbuf, SECT_SIZE) != 0) {
                ERROR ("data inconsistent\n");
                memdump (rbuf, 64);
                printf ("........\n");
                memdump (wbuf, 64);
                return -1;
            }
        }

        /* close */
        rc = _close (fd);
        if (rc < 0) {
            ERROR ("_close(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

#define test_mread_sfn(idx, cnt)        test_mread(idx, cnt, 0)
#define test_mread_lfn(idx, cnt)        test_mread(idx, cnt, 1)
static int
test_mread (int idx, int cnt, int lfn)
{
    struct fd32 fd[MULTI_MAX];
    char name[NAMESIZE];
    char rbuf[SECT_SIZE], wbuf[SECT_SIZE];
    int i, j;
    int rc;

    if (cnt > MULTI_MAX) {
        ERROR ("too many files (%d)\n", cnt);
        return -1;
    }

    memset (fd, 0, sizeof (struct fd32) * cnt);

    /* open RDONLY */
    for (i = idx; i < (idx + cnt); i++) {
        sprintf (name, lfn ? LFNAME : SFNAME, i);
        printf ("# read %s\n", name);

        rc = _open (&fd[i-idx], name, FILE_SIZE, FD_RDONLY);
        if (rc < 0) {
            ERROR ("_open(), rc=%d\n", rc);
            return rc;
        }
    }

    /* read */
    for (i = idx; i < (idx + cnt); i++) {
        for (j = 0; j < FILE_SIZE; j += SECT_SIZE) {
            memset (wbuf, i + j, SECT_SIZE);

            rc = _read (&fd[i-idx], rbuf, SECT_SIZE);
            if (rc < 0) {
                ERROR ("_read(), rc=%d\n", rc);
                return rc;
            }

            if (memcmp (wbuf, rbuf, SECT_SIZE)) {
                ERROR ("data inconsistent\n");
                memdump (rbuf, 32);
                printf ("........\n");
                memdump (wbuf, 32);
                return -1;
            }
        }
    }

    /* close */
    for (i = idx; i < (idx + cnt); i++) {
        rc = _close (&fd[i-idx]);
        if (rc < 0) {
            ERROR ("_close(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

#define test_cread_sfn(idx, cnt)        test_cread(idx, cnt, 0)
#define test_cread_lfn(idx, cnt)        test_cread(idx, cnt, 1)
int
test_cread (int idx, int cnt, int lfn)
{
    struct fd32 fd[2], *fd_this, *fd_last;
    char name[NAMESIZE];
    char rbuf[SECT_SIZE], wbuf[SECT_SIZE];
    int i, j;
    int rc;

    /*      |<----------------->|<----------------->|<----------------->|
     *             ROUND 1             ROUND 2             ROUND 3
     *      
     *  ----|---------| <SKIP>
     *          2.R  3.C
     *
     * <F1> |-------------------|---------|
     *     1.O       4.R            6.R  7.C
     *
     *                     <F2> |-------------------|---------|
     *                         5.O       8.R           10.R 11.C
     *
     *                                       <SKIP> |-------------------|----
     *                                             9.O       12.R
     */
    for (i = idx; i < (idx + cnt + 1); i++) {

        sprintf (name, lfn ? LFNAME : SFNAME, i);
        fd_this = fd + (i % 2);
        fd_last = fd + ((i-1) % 2);

        /* 1. Open This */
        if (i < (idx + cnt)) {
            printf ("# read %s\n", name);
            memset (fd_this, 0, sizeof (struct fd32));

            rc = _open (fd_this, name, FILE_SIZE, FD_RDONLY);
            if (rc < 0) {
                ERROR ("_open(), rc=%d\n", rc);
                return rc;
            }
        }

        /* 2. Read Last */
        if (i > idx) {
            for (j = (FILE_SIZE - SECT_SIZE); j < FILE_SIZE; j += SECT_SIZE) {
                memset (wbuf, i + j, SECT_SIZE);

                rc = _read (fd_last, rbuf, SECT_SIZE);
                if (rc < 0) {
                    ERROR ("_read(), rc=%d\n", rc);
                    return rc;
                }

                if (memcmp (rbuf, wbuf, SECT_SIZE) != 0) {
                    ERROR ("date inconsistent\n");
                    memdump (rbuf, 16);
                    printf ("........\n");
                    memdump (wbuf, 16);
                    return -1;
                }
            }
        }

        /* 3. Close Last */
        if (i > idx) {
            rc = _close (fd_last);
            if (rc < 0) {
                ERROR ("_close(), rc=%d\n", rc);
                return rc;
            }
        }

        /* 4. Read This */
        if (i < (idx + cnt)) {
            for (j = 0; j < (FILE_SIZE - SECT_SIZE); j += SECT_SIZE) {
                memset (wbuf, i + j, SECT_SIZE);

                rc = _read (fd_this, rbuf, SECT_SIZE);
                if (rc < 0) {
                    ERROR ("_read(), rc=%d\n", rc);
                    return rc;
                }

                if (memcmp (rbuf, wbuf, SECT_SIZE) != 0) {
                    ERROR ("date inconsistent\n");
                    memdump (rbuf, 16);
                    printf ("........\n");
                    memdump (wbuf, 16);
                    return -1;
                }
            }
        }
    }

    return 0;
}

static int
sh_fatfs_test_rsvf (int argc, char *argv[], void **result, void *sys_ctx)
{
#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    /* reserved file test */
    printf ("\n[ Reserved File Test]\n");
    test_format ();
    test_mount ();
    test_cd ("/aumsg");
    test_write_sfn (0, 4, 1);
    test_read_sfn (0, 4);
    test_write_lfn (0, 4, 1);
    test_read_lfn (0, 4);
    test_dir (".");
    test_remove_sfn (0, 4);
    test_remove_lfn (0, 4);
    test_cd ("..");
    test_cd ("/picture");
    test_write_sfn (0, 4, 1);
    test_read_sfn (0, 4);
    test_write_lfn (0, 4, 1);
    test_read_lfn (0, 4);
    test_dir (".");
    test_remove_sfn (0, 4);
    test_remove_lfn (0, 4);
    test_cd ("..");
    test_cd ("/video");
    test_write_sfn (0, 4, 1);
    test_read_sfn (0, 4);
    test_write_lfn (0, 4, 1);
    test_read_lfn (0, 4);
    test_dir (".");
    test_remove_sfn (0, 4);
    test_remove_lfn (0, 4);
    test_cd ("..");
    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_function (int argc, char *argv[], void **result, void *sys_ctx)
{
#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    /* function test of short file name */
    printf ("\n[ Short File Name Test ]\n");
    test_format ();
    test_mount ();
    test_write_sfn (0, 4, 1);
    test_read_sfn (0, 4);
    test_mwrite_sfn (4, 4);
    test_mread_sfn (4, 4);
    test_cwrite_sfn (8, 4);
    test_cread_sfn (8, 4);
    test_dir ("/");
    test_stat_sfn (0, 1);
    test_statfs ();
    test_remove_sfn (0, 12);
    test_umount ();

    /* function test of long file name */
    printf ("\n[ Long File Name Test ]\n");
    test_format ();
    test_mount ();
    test_write_lfn (0, 4, 1);
    test_read_lfn (0, 4);
    test_mwrite_lfn (4, 4);
    test_mread_lfn (4, 4);
    test_cwrite_lfn (8, 4);
    test_cread_lfn (8, 4);
    test_dir ("/");
    test_stat_lfn (0, 1);
    test_statfs ();
    test_remove_lfn (0, 12);
    test_umount ();

    /* function test of directory */
    printf ("\n[ Directory Test ]\n");
    test_format ();
    test_mount ();
    test_dir ("/");
    test_pwd ();
    test_mkdir ("/dir0");
    test_dir ("/");
    test_dir ("/dir0");
    test_cd ("/dir0");
    test_pwd ();
    test_write_sfn (0, 4, 1);
    test_read_sfn (0, 4);
    test_write_lfn (0, 4, 1);
    test_read_lfn (0, 4);
    test_dir (".");
    test_remove_sfn (0, 4);
    test_remove_lfn (0, 4);
    test_cd ("..");
    test_rmdir ("/dir0");
    test_dir ("/");
    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_exception (int argc, char *argv[], void **result, void *sys_ctx)
{
#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    /* exception test */
    printf ("[ Exception Test ]\n");
    test_format ();
    test_mount ();
    test_write_sfn (0, 2, 0);
    test_write_sfn (2, 2, 0.5);
    test_write_sfn (4, 2, 1);
    test_write_sfn (6, 2, 2);
    test_write_lfn (0, 2, 0);
    test_write_lfn (2, 2, 0.5);
    test_write_lfn (4, 2, 1);
    test_write_lfn (6, 2, 2);
    test_dir (".");
    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_boundary (int argc, char *argv[], void **result, void *sys_ctx)
{
    char pathbuf[128] = "";
    int rc;

#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    /* limitation test of directory */
    printf ("\n[ DIR Limitation Test ]\n");
    test_format ();
    test_mount ();
    do {
        strcat (pathbuf, "/dir");
        rc = test_mkdir (pathbuf);
    } while (rc == 0);
    test_umount ();

    /* limitation test of short file name */
    printf ("\n[ SNF Limitation Test ]\n");
    test_format ();
    test_mount ();
    test_write_sfn (0, 0x7fffffff, 1);
    test_umount ();

    /* limitation test of long file name */
    printf ("\n[ LNF Limitation Test ]\n");
    test_format ();
    test_mount ();
    test_write_lfn (0, 0x7fffffff, 1);
    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_performance (int argc, char *argv[], void **result, void *sys_ctx)
{
    int i;
#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    printf ("Format Test ==================================================\n");
    STATISTICS_RESET;
    test_format ();
    STATISTICS_INFO;

    printf ("Mount Test ===================================================\n");
    STATISTICS_RESET;
    test_mount ();
    STATISTICS_INFO;

    printf ("Write File Test ==============================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_write_sfn (i, 1, 1);
        STATISTICS_INFO;
    }
    
    printf ("Read File Test ===============================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_read_sfn (i, 1);
        STATISTICS_INFO;
    }

    printf ("Remove Test ==================================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_remove_sfn (i, 1);
        STATISTICS_INFO;
    }

    printf ("Write Test (Long Name) =======================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_write_lfn (i, 1, 1);
        STATISTICS_INFO;
    }

    printf ("Read Test (Long Name) ========================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_read_lfn (i, 1);
        STATISTICS_INFO;
    }

    printf ("Remove Test (Long Name) ======================================\n");
    for (i = 0; i < FILE_NUM; i++) {
        STATISTICS_RESET;
        test_remove_lfn (i, 1);
        STATISTICS_INFO;
    }

    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_longrun (int argc, char *argv[], void **result, void *sys_ctx)
{
    int i, file_count = (64 * FILE_NUM);

    if (argc > 1)
        file_count = atoi (argv[1]);

#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    printf ("\n[ Long Run Test ]\n");

    test_format ();
    test_mount ();

    for (i = 0; i < file_count; i++) {
        if (i >= FILE_NUM) {
            test_read_lfn (i - FILE_NUM, 1);
            test_remove_lfn (i - FILE_NUM, 1);
        }
        test_write_lfn (i, 1, 1);
    }

    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_compatible (int argc, char *argv[], void **result, void *sys_ctx)
{
    int i;

#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    printf ("\n[ Compatible Test ]\n");

    test_format ();
    test_mount ();
    for (i = 0; i < 3; i++) {
        test_pwd ();
        test_mkdir ("dir");
        test_write_sfn (0, 4, 1);
        test_dir (".");
        test_cd ("dir");
    }
    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}

static int
sh_fatfs_test_open (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct statfs_t st;
    int max;
    int i, file_count = (64 * FILE_NUM);
    int rc;

    if (argc > 1)
        file_count = atoi (argv[1]);

#ifndef FAT32_DEBUG
    if (!test_continue ())
        return 0;
#endif

    printf ("\n[ File Open Test ]\n");

    test_format ();
    test_mount ();

    rc = _statfs (&st);
    if (rc < 0) {
        ERROR ("_statfs(), rc=%d\n", rc);
        return -1;
    }
    max = st.f_bfree * (st.f_bsize/1024) / (32 * 1024);

    fat32_debug = 1;
    for (i = 0; i < file_count; i++) {
        if (i >= max) {
            test_read_lfn (i - max, 1);
            test_remove_lfn (i - max, 1);
        }

        test_write_lfn (i, 1, 1);
    }
    fat32_debug = 0;

    test_umount ();

    printf ("\n... test done.\n");
    return 0;
}
