/**
 *  @file   sh_norfs.c
 *  @brief  NOR Filesystem test command
 *  $Id: sh_norfs.c,v 1.20 2014/08/02 13:22:48 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.20 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/08/29  hugo        New file.
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
#define DBG_LEVEL   4
#include <debug.h>
#include <norfs.h>
#include <heap.h>

/* debug only */
command_init (sh_nor_dump, "nor_dump", "nor_dump <offset> <length>");
command_init (sh_nor_speed, "nor_speed", "nor_speed <length>");

/* partition */
command_init (sh_norfs_format, "norfs_format", "norfs_format");

/* directory */
command_init (sh_norfs_dir, "norfs_dir", "norfs_dir <dirname>");
command_init (sh_norfs_ls, "norfs_ls", "norfs_ls <dirname>");
command_init (sh_norfs_gettotal, "norfs_gettotal", "norfs_gettotal <dirname> [<mask>]");
command_init (sh_norfs_cleandir, "norfs_cleandir", "norfs_cleandir <dirname>");

/* file */
command_init (sh_norfs_dd, "norfs_dd", "norfs_dd <filename> <length>");
command_init (sh_norfs_hexdump, "norfs_hexdump", "norfs_hexdump <filename> [<length>] [<skip>]");
command_init (sh_norfs_hexdump2, "norfs_hexdump2", "norfs_hexdump2 <filename> [<length>] [<skip>]");
command_init (sh_norfs_rm, "norfs_rm", "norfs_rm <filename>");
command_init (sh_norfs_stat, "norfs_stat", "norfs_stat <filename>");
command_init (sh_norfs_attr, "norfs_attr", "norfs_attr <filename> <value>");

/* test */
command_init (sh_norfs_test, "norfs_test", "norfs_test");

static int
sh_nor_dump (int argc, char *argv[], void **result, void *sys_ctx)
{
    unsigned int offset, length, len, i;
    unsigned char buf[512];

    if (argc < 3)
        goto EXIT;

    offset = size (argv[1]);
    length = size (argv[2]);

    for (i = 0; i < length; i += len) {
        len = MIN (length - i, 512);
        nor_read (offset + i, len, buf);
        hexdump (offset + i, len, buf);
    }

    return 0;

EXIT:
    print_usage (sh_nor_dump);
    return -1;
}

static int
sh_nor_speed (int argc, char *argv[], void **result, void *sys_ctx)
{
    int length, offset;
    char *buf = heap_alloc(32*1024);
    int t1ms = sys_cpu_clk/2/1000;
    unsigned int tick_s, tick_e, ms;

    if (argc < 2)
        goto EXIT;

    length = size (argv[1]);

    tick_s = read_c0_count32();
    for (offset = 0; offset < length; offset += (32*1024)) {
        nor_read (offset, (32*1024), buf);
    }
    tick_e = read_c0_count32();
    ms = (tick_e - tick_s) / t1ms;

    printf ("length: %d KB, time: %u ms, speed: %u KB/s\n",
            length / 1024, ms, length / ms);

    return 0;
EXIT:
    print_usage (sh_nor_speed);
    return -1;
}

static int
sh_norfs_format (int argc, char *argv[], void **result, void *sys_ctx)
{
    return norfs_format_partition ();
}

static int
sh_norfs_dir (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char *dirname;
    int num_file = 0;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    dirname = argv[1];

    rc = norfs_opendir (dir, dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        goto EXIT;
    }

    while ((norfs_readdir (dir, dirent))) {
        printf ("%04d-%02d-%02d %02d:%02d:%02d  %10d  %02x  %s  (%d)\n",
                FAT_GET_YEAR (dirent->date),
                FAT_GET_MONTH (dirent->date),
                FAT_GET_DAY (dirent->date),
                FAT_GET_HOUR (dirent->time),
                FAT_GET_MIN (dirent->time),
                FAT_GET_SEC (dirent->time),
                dirent->length,
                dirent->attribute,
                dirent->name,
                dirent->fileid);
        num_file++;
    }
    printf ("%d files\n", num_file);

    norfs_closedir (dir);
    return 0;
EXIT:
    print_usage (sh_norfs_dir);
    return rc;
}

static int
compare (const void *p1, const void *p2)
{
    return strcmp (((struct norfs_dirent *)p1)->name, ((struct norfs_dirent *)p2)->name);
}

static int
sh_norfs_ls (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[64];
    char *dirname;
    int idx, num, num_file = 0;
    int rc = 0;

    if (argc < 2)
        goto EXIT;

    dirname = argv[1];

    rc = norfs_opendir (dir, dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        goto EXIT;
    }

    for (idx = 0; idx < 64; idx++) {
        if (norfs_readdir (dir, dirent + idx) == NULL)
            break;
    }
    num = idx;

    qsort (dirent, num, sizeof (struct norfs_dirent), compare);

    for (idx = 0; idx < num; idx++) {
        printf ("%04d-%02d-%02d %02d:%02d:%02d  %10d  %02x  %s\n",
                FAT_GET_YEAR (dirent[idx].date),
                FAT_GET_MONTH (dirent[idx].date),
                FAT_GET_DAY (dirent[idx].date),
                FAT_GET_HOUR (dirent[idx].time),
                FAT_GET_MIN (dirent[idx].time),
                FAT_GET_SEC (dirent[idx].time),
                dirent[idx].length,
                dirent[idx].attribute,
                dirent[idx].name);
        num_file++;
    }

    norfs_closedir (dir);
    return 0;
EXIT:
    print_usage (sh_norfs_ls);
    return rc;
}

static int
sh_norfs_gettotal (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_dir dir[1];
    char *name;
    int filter, mask;
    int total, npick;
    int rc;

    name = argv[1];
    filter = argc > 2 ? 1 : 0;
    mask = argc > 2 ? strtol (argv[2], NULL, 0) : 0;

    rc = norfs_opendir (dir, name);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_gettotal (dir, &total, &npick, filter, mask);
    if (rc < 0) {
        ERROR ("norfs_gettotal(), rc=%d\n", rc);
        goto EXIT;
    }

    norfs_closedir (dir);

    printf ("total: %d, npick: %d\n", total, npick);
    return 0;
EXIT:
    print_usage (sh_norfs_gettotal);
    return -1;

}

static int
sh_norfs_cleandir (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_dir dir[1];
    char *dirname;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    dirname = argv[1];

    rc = norfs_opendir (dir, dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_cleandir (dir);
    if (rc < 0) {
        ERROR ("norfs_cleandir(), rc=%d\n", rc);
        goto EXIT;
    }

    norfs_closedir (dir);
    return 0;
EXIT:
    print_usage (sh_norfs_cleandir);
    return rc;
}

static int
sh_norfs_dd (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_fd fd[1];
    char *pathname;
    int length;
    char buf[256];
    int i, cnt;
    int rc = 0;

    if (argc < 3)
        goto EXIT;
    pathname = argv[1];
    length = size (argv[2]);

    printf ("dump %s %d\n", pathname, length);

    rc = norfs_open (fd, pathname, length, NOR_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    for (i = 0; i < 256; i++)
        buf[i] = i;

    for (i = 0; i < length; i += cnt) {
        cnt = MIN (256, length - i);
        rc = norfs_write (fd, buf, cnt);
        if (rc < 0) {
            ERROR ("norfs_write(), rc=%d\n", rc);
            goto EXIT;
        }
    }

    norfs_close (fd);
    return 0;
EXIT:
    print_usage (sh_norfs_dd);
    return rc;
}

static int
sh_norfs_hexdump (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_fd fd[1];
    struct norfs_stat st;
    char *pathname;
    unsigned int offset, length, skip, len;
    char buf[256];
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    pathname = argv[1];

    rc = norfs_stat (pathname, &st);
    if (rc < 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    skip = argc > 3 ? size (argv[3]) : 0;
    length = argc > 2 ? MIN (size (argv[2]), st.st_size - skip) : st.st_size - skip;

    rc = norfs_open (fd, pathname, 0, NOR_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_seek (fd, skip);
    if (rc < 0) {
        ERROR ("norfs_seek(), rc=%d\n", rc);
        goto EXIT;
    }

    for (offset = 0; offset < length; offset += len) {
        len = MIN (256, length - offset);
        rc = norfs_read (fd, buf, len);
        if (rc < 0) {
            ERROR ("norfs_read(), rc=%d\n", rc);
            goto EXIT;
        }

        hexdump (skip + offset, len, buf);
    }
    putchar ('\n');

    norfs_close (fd);
    return 0;
EXIT:
    print_usage (sh_norfs_hexdump);
    return rc;
}

static int
sh_norfs_hexdump2 (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_fd fd[1];
    struct norfs_stat st;
    char *pathname;
    unsigned int length, skip;
    char *buf;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    pathname = argv[1];

    rc = norfs_stat (pathname, &st);
    if (rc < 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    skip = argc > 3 ? size (argv[3]) : 0;
    length = argc > 2 ? MIN (size (argv[2]), st.st_size - skip) : st.st_size - skip;

    rc = norfs_open (fd, pathname, 0, NOR_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    buf = norfs_mmap (fd, skip);
    if (buf == MAP_FAILED) {
        ERROR ("norfs_mmap()\n");
        goto EXIT;
    }

    hexdump (skip, length, buf);
    putchar ('\n');

    norfs_close (fd);
    return 0;
EXIT:
    print_usage (sh_norfs_hexdump2);
    return rc;
}

static int
sh_norfs_rm (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *pathname;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    pathname = argv[1];

    rc = norfs_remove (pathname);
    if (rc < 0) {
        ERROR ("norfs_remove(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_norfs_rm);
    return rc;
}

static int
sh_norfs_stat (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct norfs_stat st;
    char *pathname;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    pathname = argv[1];

    rc = norfs_stat (pathname, &st);
    if (rc < 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    printf ("    File: \'%s\'\n", pathname);
    printf ("    Size: %u\n", st.st_size);
    printf ("  Attrib: 0x%02x\n", st.attribute);
    printf ("   Start: 0x%08x\n", st.start);
    printf ("  Modify: %04d-%02d-%02d %02d:%02d:%02d\n",
            FAT_GET_YEAR (st.mdate),
            FAT_GET_MONTH (st.mdate),
            FAT_GET_DAY (st.mdate),
            FAT_GET_HOUR (st.mtime),
            FAT_GET_MIN (st.mtime),
            FAT_GET_SEC (st.mtime));

    return 0;
EXIT:
    print_usage (sh_norfs_stat);
    return rc;
}

static int
sh_norfs_attr (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int attr;
    int rc = 0;

    if (argc < 3)
        goto EXIT;

    name = argv[1];
    attr = strtol (argv[2], NULL, 0);

    rc = norfs_setattr (name, attr);
    if (rc < 0) {
        ERROR ("norfs_setattr(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_norfs_attr);
    return rc;
}

/*****************************************************************************/

static int
test_write (char *name, int length)
{
    struct norfs_fd fd[1];
    char buf[256];
    int i, cnt;
    int rc;

    printf ("test_write(%s, %d)\n", name, length);

    rc = norfs_open (fd, name, length, NOR_MODE_WRONLY);
    if (rc != 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        return rc;
    }

    for (i = 0; i < length; i += cnt) {
        cnt = MIN (256, length - i);
        rc = norfs_write (fd, buf, cnt);
        if (rc != 0) {
            ERROR ("norfs_write(), rc=%d\n", rc);
            return rc;
        }
    }

    rc = norfs_close (fd);
    if (rc != 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_read (char *name)
{
    struct norfs_fd fd[1];
    struct norfs_stat st;
    char buf[256];
    int length;
    int i, cnt;
    int rc;

    printf ("test_read(%s)\n", name);

    rc = norfs_stat (name, &st);
    if (rc != 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        return rc;
    }

    rc = norfs_open (fd, name, 0, NOR_MODE_RDONLY);
    if (rc != 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        return rc;
    }

    length = st.st_size;
    for (i = 0; i < length; i += cnt) {
        cnt = MIN (256, length - i);
        rc = norfs_read (fd, buf, cnt);
        if (rc != 0) {
            ERROR ("norfs_read(), rc=%d\n", rc);
            return rc;
        }
    }

    rc = norfs_close (fd);
    if (rc != 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
test_remove (char *name)
{
    int rc;

    printf ("test_remove(%s)\n", name);

    rc = norfs_remove (name);
    if (rc != 0) {
        ERROR ("norfs_remove(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
sh_norfs_test (int argc, char *argv[], void **result, void *sys_ctx)
{
    /* test aumsg */
    test_write ("aumsg/000", 32*1024);
    test_write ("aumsg/001", 32*1024);
    test_write ("aumsg/002", 32*1024);
    test_write ("aumsg/003", 32*1024);
    test_read  ("aumsg/000");
    test_read  ("aumsg/001");
    test_read  ("aumsg/002");
    test_read  ("aumsg/003");
    test_remove ("aumsg/000");
    test_remove ("aumsg/001");
    test_remove ("aumsg/002");
    test_remove ("aumsg/003");

    /* test picture */
    test_write ("picture/000", 32*1024);
    test_write ("picture/001", 32*1024);
    test_write ("picture/002", 32*1024);
    test_write ("picture/003", 32*1024);
    test_read  ("picture/000");
    test_read  ("picture/001");
    test_read  ("picture/002");
    test_read  ("picture/003");
    test_remove ("picture/000");
    test_remove ("picture/001");
    test_remove ("picture/002");
    test_remove ("picture/003");

    printf ("\n... test done.\n");
    return 0;
}
