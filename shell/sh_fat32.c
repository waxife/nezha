/**
 *  @file   sh_fat32.c
 *  @brief  fat test command
 *  $Id: sh_fat32.c,v 1.56 2014/08/20 02:18:46 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.56 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/02/08  sherman   New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include <fat32.h>
#include <mconfig.h>
#include <heap.h>
#include <ls.h>

#define BUFSIZ  1024

/* debug only */
command_init (sh_fate_used, "fate_used", "fate_used");
command_init (sh_fate_free, "fate_free", "fate_free");
command_init (sh_fate_dump, "fate_dump", "fate_dump [<count>] [<start>]");
command_init (sh_fate_trace, "fate_trace", "fate_trace <filename>");

command_init (sh_fat_check, "fat_check", "fat_check <on|off>");
command_init (sh_fat_info, "fat_info", "fat_info");
command_init (sh_clus2sect, "clus2sect", "clus2sect <clus>");
command_init (sh_hackdll, "hackdll", "hackdll <filename> <prev> <next>");

/* filesystem */
command_init (sh_format, "format", "format [<rsvf_len>] [<rsvf_num>]");
command_init (sh_mount, "mount", "mount");
command_init (sh_umount, "umount", "umount");
command_init (sh_remount, "remount", "remount");
command_init (sh_df, "df", "df");

/* directory */
command_init (sh_mkdir, "mkdir", "mkdir <dirname>");
command_init (sh_rmdir, "rmdir", "rmdir <dirname>");
command_init (sh_cd, "cd", "cd <dirname>");
command_init (sh_pwd, "pwd", "pwd");
command_init (sh_dir, "dir", "dir [-t] [-n] [<dirname>]");
command_init (sh_dir_ls, "dirls", "dirls [<dirname>]");
command_init (sh_scandir, "scandir", "scandir <dirname>");
command_init (sh_rebuild, "rebuild", "rebuild <dirname>");

/* file */
command_init (sh_dd, "dd", "dd <filename> <length>");
command_init (sh_diff, "diff", "diff <src filename> <dst filename>");
command_init (sh_hexdump, "hexdump", "hexdump <filename> [<length>] [<skip>]");
command_init (sh_rm, "rm", "rm <filename>");
command_init (sh_mv, "mv", "mv <src filename> <dst filename>");
command_init (sh_recycle, "recycle", "recycle <src filename> <dst filename>");
command_init (sh_stat, "stat", "stat <filename>");
command_init (sh_attr, "attr", "attr <filename> <value>");
command_init (sh_create, "create", "create <filename> <length> [clear=0,1]");

static int
sh_fate_used (int argc, char *argv[], void **result, void *sys_ctx)
{
    fate_used ();
    return 0;
}

static int
sh_fate_free (int argc, char *argv[], void **result, void *sys_ctx)
{
    fate_free ();
    return 0;
}

static int
sh_fate_dump (int argc, char *argv[], void **result, void *sys_ctx)
{
    unsigned int start;
    int count;

    count = (argc > 1) ? atoi (argv[1]) : -1;
    start = (argc > 2) ? atoi (argv[2]) : 0;

    fate_dump (start, count);
    return 0;
}

static int
sh_fate_trace (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct fd32 fd[1];
    char *name;
    unsigned int clus;
    int i, count = 0;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* open file */
    rc = _open (fd, name, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    /* show region info */
    printf ("Region info:\n");
    for (i = 0; i < fd->region_cnt; i++) {
        printf ("  R[%d] %6u - %6u : %6u\n", i,
                fd->region[i].start,
                fd->region[i].start + fd->region[i].count - 1,
                fd->region[i].count);
        count += fd->region[i].count;
    }
    printf ("  ---------------------- %6d\n", count);

    /* trace fate */
    clus = fd->start_clus;
    fate_trace (clus);

    /* close file */
    rc = _close (fd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_fate_trace);
    return rc;
}

static int
sh_fat_check (int argc, char *argv[], void **result, void *sys_ctx)
{
    if (argc < 2)
        goto EXIT;

    if (!strcmp (argv[1], "on"))
        fat_check_tw_signature (1);
    else
    if (!strcmp (argv[1], "off"))
        fat_check_tw_signature (0);
    else
        goto EXIT;

    return 0;
EXIT:
    print_usage (sh_fat_check);
    return -1;
}

static int
sh_fat_info (int argc, char *argv[], void **result, void *sys_ctx)
{
    fat_info ();
    return 0;
}

#define CLUS2SECT(CLUS) (fat->root_start + ((CLUS) - FIRST_ROOTCLUSTER) * fat->sect_per_clus)
static int
sh_clus2sect (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat32 *fat = sys->pfat->fat;
    int clus;

    if (argc < 2)
        goto EXIT;

    clus = atoi (argv[1]);
    printf ("%d\n", CLUS2SECT (clus));

    return 0;
EXIT:
    print_usage (sh_clus2sect);
    return 1;
}

int hack_dll (const char *name, unsigned short prev, unsigned short next, int total);
static int
sh_hackdll (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    unsigned short prev, next;
    int total;
    int rc = 0;

    if (argc < 4)
        goto EXIT;
    name = argv[1];
    prev = strtol (argv[2], NULL, 0);
    next = strtol (argv[3], NULL, 0);
    total = argc > 4 ? atoi (argv[4]) : 0;

    rc = hack_dll (name, prev, next, total);
    if (rc < 0) {
        ERROR ("hack_dll(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_hackdll);
    return rc;
}

static int
sh_format (int argc, char *argv[], void **result, void *sys_ctx)
{
    int rc = 0;

    rc = _format ();
    if (rc < 0) {
        ERROR ("_format(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_format);
    return rc;
}


static int
sh_mount (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat_t *fat = sys->pfat;
    int rc = 0;

    rc = _mount (fat);
    if (rc < 0) {
        ERROR ("_mount(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_mount);
    return rc;
}

static int
sh_umount (int argc, char *argv[], void **result, void *sys_ctx)
{
    int rc = 0;

    rc = _umount ();
    if (rc < 0) {
        ERROR ("_umount(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_umount);
    return rc;
}

static int
sh_remount (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat_t *fat = sys->pfat;
    int rc = 0;

    _umount ();

    rc = _mount (fat);
    if (rc < 0) {
        ERROR ("_mount(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_remount);
    return rc;
}

static int
sh_df (int argc, char **argv, void **result, void *sys_ctx)
{
    struct statfs_t st;
    int i;
    int rc = 0;

    rc = _statfs (&st);
    if (rc < 0) {
        ERROR ("_statfs(), rc=%d\n", rc);
        goto EXIT;
    }

    printf ("%10s %10s %10s %10s %10s\n",
            "Filesystem", "1K-blocks", "Used", "Available", " Use%");
    printf ("%10s %10d %10d %10d %9d%%", "/",
            st.f_blocks * (st.f_bsize/1024),
            (st.f_blocks - st.f_bfree) * (st.f_bsize/1024),
            st.f_bfree * (st.f_bsize/1024),
            (st.f_blocks - st.f_bfree) * 100 / st.f_blocks);
    printf ("\n\n");
    printf ("Rsvf Dir  File Num  File Len\n");
    printf ("========  ========  ========\n");
    for (i = 0; i < RSVDIR_NUM; i++) {
        printf ("%8d  %8d  %6dKB\n", i, st.e[i].rsvf_num, st.e[i].rsvf_len>>10);
    }

    return 0;

EXIT:
    print_usage (sh_df);
    return rc;
}

static int
sh_mkdir (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = _mkdir (name);
    if (rc < 0) {
        ERROR ("_mkdir(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_mkdir);
    return rc;
}

static int
sh_rmdir (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = _rmdir (name);
    if (rc < 0) {
        ERROR ("_rmdir(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_rmdir);
    return rc;
}

static int
sh_cd (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = _chdir (name);
    if (rc < 0) {
        ERROR ("_chdir(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_cd);
    return rc;
}

static int
sh_pwd (int argc, char *argv[], void **result, void *sys_ctx)
{
    char buf[63+1], *cwd;

    cwd = _getcwd (buf, 63);
    if (!cwd) {
        ERROR ("_getcwd()\n");
        goto EXIT;
    }
    printf ("%s\n", cwd);
    return 0;

EXIT:
    print_usage (sh_pwd);
    return -1;
}

typedef struct {
    char Name[NAMELEN+1];
    unsigned int    Date;
    unsigned int    Time;
    unsigned short  dire_index;
} __attribute__((packed)) fileInfo_t;

#define DateTime(file) (((file)->Date << 16) | ((file)->Time << 0))
static int
compare_byTime (const void *src, const void *dst)
{
    return DateTime((fileInfo_t *)src) - DateTime ((fileInfo_t *)dst);
}

static int
compare_byName (const void *src, const void *dst)
{
    return strcmp (((fileInfo_t *)src)->Name, ((fileInfo_t *)dst)->Name);
}

static void
print_dirent (struct dirent *dirent)
{
    char type = (dirent->Attrib & ATTRIB_DIRECTORY) ? 'd' : '-';

    printf ("%04d-%02d-%02d %02d:%02d:%02d %10d %8d %02x %02x %2c  %s\n",
        FAT_GET_YEAR (dirent->Date),
        FAT_GET_MONTH (dirent->Date),
        FAT_GET_DAY (dirent->Date),
        FAT_GET_HOUR (dirent->Time),
        FAT_GET_MIN (dirent->Time),
        FAT_GET_SEC (dirent->Time),
        dirent->Length, dirent->StartCluster,
        dirent->Attribute, dirent->signature, type,
        dirent->Name);
}

#define SORT_BY_TIME    (1 << 0)
#define SORT_BY_NAME    (1 << 1)
static int
sh_dir (int argc, char **argv, void **result, void *sys_ctx)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char *name = ".";
    int c, sort = 0;
    int num_dir = 0, num_file = 0;
    int rc = 0;

    while ((c = getopt (argc, argv, "-tn")) != -1) {
        switch (c) {
            case 't':
                sort |= SORT_BY_TIME;
                break;

            case 'n':
                sort |= SORT_BY_NAME;
                break;

            case 1:
                name = optarg;
                break;
        }
    }

    rc = _opendir (dir, name);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        goto EXIT;
    }

    printf ("%19s %10s %8s %2s %2s %2s  %s\n",
            "DateTime", "Length", "Cluster", "A", "S", "T", "Name");
    printf ("------------------- ---------- -------- -- -- --  ----\n");

    if (sort) {
        char *buf = heap_alloc (sizeof (fileInfo_t) * 1024);
        fileInfo_t *files = (fileInfo_t *) buf;

        int idx = 0;
        while ((_readdir (dir, dirent)) && idx < 1024) {
            fileInfo_t *file = &files[idx++];
            strcpy (file->Name, dirent->Name);
            file->Date = dirent->Date;
            file->Time = dirent->Time;
            file->dire_index = dirent->dire_index;
        }

        int cnt = idx;
        if ((sort & SORT_BY_TIME))
            qsort (files, cnt, sizeof (fileInfo_t), compare_byTime);
        if ((sort & SORT_BY_NAME))
            qsort (files, cnt, sizeof (fileInfo_t), compare_byName);

        for (idx = 0; idx < cnt; idx++) {
            dirent_find_by_index (dir, files[idx].dire_index, dirent);

            if ((dirent->Attrib & ATTRIB_DIRECTORY))
                num_dir++;
            else
                num_file++;

            print_dirent (dirent);
        }

        heap_release (buf);
    } else {
        while ((_readdir (dir, dirent))) {
            if ((dirent->Attrib & ATTRIB_DIRECTORY))
                num_dir++;
            else
                num_file++;

            print_dirent (dirent);
        }
    }

    printf ("%d files\n", num_file);
    printf ("%d dirs\n", num_dir);

    rc = _closedir (dir);
    if (rc < 0) {
        ERROR ("_closedir(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;

EXIT:
    print_usage (sh_dir);
    return rc;
}

static char *
direlink (struct dirent *dirent, char *buf)
{
    if ((dirent->dire_prev & LINK_NULL) == LINK_NULL)
        sprintf (buf + 0, "   - ");
    else
        sprintf (buf + 0, "%4d ", dirent->dire_prev);
    if ((dirent->dire_index & LINK_NULL) == LINK_NULL)
        sprintf (buf + 5, "   - ");
    else
        sprintf (buf + 5, "%4d ", dirent->dire_index);
    if ((dirent->dire_next & LINK_NULL) == LINK_NULL)
        sprintf (buf + 10, "   -");
    else
        sprintf (buf + 10, "%4d", dirent->dire_next);

    return buf;
}

static int
sh_dir_ls (int argc, char **argv, void **result, void *sys_ctx)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char *name, buf[20];
    int num_dir = 0, num_file = 0;
    int rc = 0;

    name = argc > 1 ? argv[1] : ".";

    rc = _opendir (dir, name);
    if (rc < 0) {
        ERROR ("_opendir, rc=%d\n", rc);
        goto EXIT;
    }

    printf ("%19s %10s %8s %4s %4s %4s  %s\n",
            "DateTime", "Length", "Cluster", "Prev", "This", "Next", "Name");
    printf ("------------------- ---------- -------- ---- ---- ----  ----\n");
    while ((_readdir (dir, dirent))) {
        if ((dirent->Attrib & ATTRIB_DIRECTORY)) {
            num_dir++;
        } else {
            num_file++;
        }

        printf ("%04d-%02d-%02d %02d:%02d:%02d %10d %8d %14s  %s\n",
            FAT_GET_YEAR (dirent->Date),
            FAT_GET_MONTH (dirent->Date),
            FAT_GET_DAY (dirent->Date),
            FAT_GET_HOUR (dirent->Time),
            FAT_GET_MIN (dirent->Time),
            FAT_GET_SEC (dirent->Time),
            dirent->Length, dirent->StartCluster,
            direlink (dirent, buf),
            dirent->Name);
    }
    printf ("%d files\n", num_file);
    printf ("%d dirs\n", num_dir);

    rc = _closedir (dir);
    if (rc < 0) {
        ERROR ("_closedir(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_dir_ls);
    return rc;
}

static int
sh_scandir (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = scan_dll (name);
    if (rc < 0) {
        ERROR ("scan_dll(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_scandir);
    return rc;
}

static int
sh_rebuild (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = rebuild_dll (name);
    if (rc < 0) {
        ERROR ("rebuild_dll(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_rebuild);
    return rc;
}

static int
sh_dd (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct fd32 fd[1];
    char *name, buf[BUFSIZ];
    unsigned int length, index, cnt;
    int rc = 0;

    if (argc < 3)
        goto EXIT;
    name = argv[1];
    length = size (argv[2]);

    for (index = 0; index < BUFSIZ; index++)
        buf[index] = index;

    rc = _open (fd, name, length, FD_WRONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    for (index = 0; index < length; index += cnt) {
        cnt = MIN (length - index, BUFSIZ);
        rc = _write (fd, buf, cnt);
        if (rc < 0) {
            ERROR ("_write(), rc=%d\n", rc);
            goto EXIT;
        }
    }

    rc = _close (fd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_dd);
    return rc;
}

static int
sh_diff (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *srcPath, *dstPath;
    struct fd32 srcFd[1], dstFd[1];
    char srcBuf[BUFSIZ], dstBuf[BUFSIZ];
    struct stat_t srcSt[1], dstSt[1];
    unsigned int offset, length, len;
    int rc = 0;

    if (argc < 3)
        goto EXIT;

    srcPath = argv[1];
    dstPath = argv[2];

    /* check length */
    rc = _stat (srcPath, srcSt);
    if (rc < 0) {
        ERROR ("_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = _stat (dstPath, dstSt);
    if (rc < 0) {
        ERROR ("_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    if (srcSt->st_size != dstSt->st_size) {
        ERROR ("%s and %s differ\n", srcPath, dstPath);
        goto EXIT;
    }
    length = srcSt->st_size;

    /* open files */
    rc = _open (srcFd, srcPath, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = _open (dstFd, dstPath, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    /* check data */
    for (offset = 0; offset < length; offset += len) {
        len = MIN (length - offset, BUFSIZ);

        rc = _read (srcFd, srcBuf, len);
        if (rc < 0) {
            ERROR ("_read(), rc=%d\n", rc);
            goto EXIT;
        }

        rc = _read (dstFd, dstBuf, len);
        if (rc < 0) {
            ERROR ("_read(), rc=%d\n", rc);
            goto EXIT;
        }

        if (memcmp (srcBuf, dstBuf, len) != 0) {
            ERROR ("%s and %s differ\n", srcPath, dstPath);
            goto EXIT;
        }
    }

    /* close files */
    _close (dstFd);
    _close (srcFd);

    return 0;
EXIT:
    print_usage (sh_diff);
    return -1;
}

static int
sh_hexdump (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct fd32 fd[1];
    char *name;
    unsigned char buf[SECT_SIZE];
    struct stat_t st;
    unsigned int skip, length, cnt, ccc;
    unsigned int i, j, k;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* get file status */
    rc = _stat (name, &st);
    if (rc < 0) {
        ERROR ("_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    skip = argc > 3 ? size (argv[3]) : 0;
    length = argc > 2 ? MIN (size (argv[2]), st.st_size - skip) : st.st_size - skip;
    printf ("dump %d-%d\n", skip, skip+length-1);

    /* open file */
    rc = _open (fd, name, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    /* skip data */
    rc = _seek (fd, skip);
    if (rc < 0) {
        ERROR ("_seek(), rc=%d\n", rc);
        goto EXIT;
    }

    for (i = 0; i < length; i += cnt) {
        cnt = MIN (length - i, SECT_SIZE);

        /* read data */
        rc = _read (fd, buf, cnt);
        if (rc < 0) {
            ERROR ("_read(), rc=%d\n", rc);
            goto EXIT;
        }

        /* hexdump */
        for (j = 0; j < cnt; j += 16) {
            printf ("%08x  ", skip + i + j);

            /* line dump */
            ccc = MIN (16, cnt - j);
            for (k = 0; k < 16; k++) {
                if (k < ccc)
                    printf ("%02x ", buf[j+k]);
                else
                    printf ("   ");
            }
            printf ("| ");
            for (k = 0; k < 16; k++) {
                if (k < ccc)
                    putchar ((buf[j+k] >= 0x20 && buf[j+k] < 0x7f) ? buf[j+k] : '.');
                else
                    putchar (' ');
            }
            putchar ('\n');
        }
    }
    putchar ('\n');

    /* close file */
    rc = _close (fd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_hexdump);
    return rc;
}

static int
sh_rm (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    rc = _remove (name);
    if (rc < 0) {
        ERROR ("_remove(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_rm);
    return rc;
}

static int
sh_recycle (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *src, *dst;
    int rc = 0;

    if (argc < 3)
        goto EXIT;
    src = argv[1];
    dst = argv[2];

    rc = _recycle (src, dst);
    if (rc < 0) {
        ERROR ("_recycle(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_mv);
    return rc;
}

static int
sh_mv (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *src, *dst;
    int rc = 0;

    if (argc < 3)
        goto EXIT;
    src = argv[1];
    dst = argv[2];

    rc = _rename (src, dst);
    if (rc < 0) {
        ERROR ("_rename(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_mv);
    return rc;
}

static int
sh_stat (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    struct stat_t st;
    int rc = 0;

    if (argc < 2)
        goto EXIT;

    name = argv[1];
    rc = _stat (name, &st);
    if (rc < 0) {
        ERROR ("_stat(), rc=%d\n", rc);
        goto EXIT;
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

    return 0;
EXIT:
    print_usage (sh_stat);
    return rc;
}

static int
sh_attr (int argc, char *argv[], void **result, void *sys_ctx)
{
    char *name;
    int attr;
    int rc = 0;

    if (argc < 3)
        goto EXIT;

    name = argv[1];
    attr = strtol (argv[2], NULL, 0);

    rc = _setattr (name, attr);
    if (rc < 0) {
        ERROR ("_setattr(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_attr);
    return rc;
}

static int
sh_create (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct fd32 fd[1];
    char *name;
    int length, clear;
    int rc = 0;

    if (argc < 3)
        goto EXIT;

    name = argv[1];
    length = size (argv[2]);
    clear = argc > 3 ? atoi (argv[3]) : 0;

    rc = _open (fd, name, length, FD_WRONLY | (clear ? FD_CLEAR1K : 0));
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = _seek (fd, length);
    if (rc < 0) {
        ERROR ("_seek(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = _close (fd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;

EXIT:
    print_usage (sh_create);
    return rc;
}
