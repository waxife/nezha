/**
 *  @file   sh_ls.c
 *  @brief  file manipulate command
 *  $Id: sh_ls.c,v 1.22 2014/08/18 05:47:13 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.22 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/26  hugo      New file.
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
#include <ls.h>
#include <norfs.h>
#include <fat32.h>

command_init (sh_gettotalls, "gettotalls", "gettotalls <fs>/<media> [<mask>]");
command_init (sh_fwls, "fwls", "fwls <fs>/<media> [<attribute>]");
command_init (sh_bwls, "bwls", "bwls <fs>/<media> [<attribute>]");
command_init (sh_addls, "addls", "addls <fs>/<media> <fname> [<alt>] [<mask>]");
command_init (sh_attrls, "attrls", "attrls <fs>/<media> <pos> <attr>");

static int add_nor_file (char *name, int length);
static int add_fat_file (char *name, int length);

int parse_media (char *type);
char *media_name (MEDIA media);

int parse_filesystem (char *type);
char *filesystem_name (FS fs);

static int
parse_filesystem_media (char *str, int *filesystem, int *media)
{
    char *ptr;
    int f = -1, m = -1;
    
    ptr = strchr (str, '/');
    if (ptr == NULL) {
        /* bad format */
        return -1;
    }
    *ptr++ = '\0';

    f = parse_filesystem (str);
    if (f < 0) {
        /* bad filesystem */
        return -1;
    }

    m = parse_media (ptr);
    if (m < 0) {
        /* bad media */
        return -1;
    }

    *filesystem = f;
    *media = m;
    return 0;
}

static int
sh_gettotalls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media;
    int filter, mask;
    int nls, npick;
    int rc;

    if (argc < 2)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;

    filter = argc > 2 ? 1 : 0;
    mask = argc > 2 ? strtol (argv[2], NULL, 0) : 0;

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    rc = gettotalls (hls, &nls, &npick, filter, mask);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        goto EXIT;
    }
    printf ("nls: %d, npick: %d\n", nls, npick);

    closels (hls);
    return 0;
EXIT:
    print_usage (sh_gettotalls);
    return -1;
}

static int
sh_fwls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media, attribute;
    int nls, npick;
    char *pos;
    int rc;

    if (argc < 2)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;

    attribute = argc > 2 ? strtol (argv[2], NULL, 0) : 0;

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    rc = gettotalls (hls, &nls, &npick, 0, 0);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        goto EXIT;
    }
    printf ("nls: %d, npick: %d\n", nls, npick);

    if (nls == 0)
        goto DONE;

    rc = pos2headls (hls);
    if (rc < 0) {
        ERROR ("pos2headls(), rc=%d\n", rc);
        goto EXIT;
    }

    while (getnextls (hls, &pos, &attribute) == 0) {
        printf ("%s, attr:%02x\n", pos, attribute);
    }

DONE:
    closels (hls);
    return 0;
EXIT:
    print_usage (sh_fwls);
    return -1;
}

static int
sh_bwls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media, attribute;
    int nls, npick;
    char *pos;
    int rc;

    if (argc < 2)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;

    attribute = argc > 2 ? strtol (argv[2], NULL, 0) : 0;

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    rc = gettotalls (hls, &nls, &npick, 0, 0);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        goto EXIT;
    }
    printf ("nls: %d, npick: %d\n", nls, npick);

    if (nls == 0)
        goto DONE;

    rc = pos2taills (hls);
    if (rc < 0) {
        ERROR ("pos2taills(), rc=%d\n", rc);
        goto EXIT;
    }

    while (getprevls (hls, &pos, &attribute) == 0) {
        printf ("%s, attr:%02x\n", pos, attribute);
    }

DONE:
    closels (hls);
    return 0;
EXIT:
    print_usage (sh_bwls);
    return -1;
}

static int
sh_addls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media;
    char *dirname, *filename, *pos;
    char fname[64];
    int victim, alt, mask;
    int rc;

    if (argc < 3)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;
    filename = argv[2];
    victim = (argc > 3);
    alt  = argc > 3 ? strtol (argv[3], NULL, 0) : 0;
    mask = argc > 4 ? strtol (argv[4], NULL, 0) : 0;

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    rc = getnewls (hls, &pos, filename, victim, alt, mask);
    if (rc < 0) {
        ERROR ("getnewls(), rc=%d\n", rc);
        goto EXIT;
    }

    dirname = media_dirname (filesystem, media);
    sprintf (fname, "%s/%s", dirname, pos);
    printf ("add %s\n", fname);
    if (hls->filesystem == NORFS) {
        rc = add_nor_file (fname, 1024);
        if (rc < 0) {
            ERROR ("add_nor_file(), rc=%d\n", rc);
            return rc;
        }
    } else {
        rc = add_fat_file (fname, 1024);
        if (rc < 0) {
            ERROR ("add_fat_file(), rc=%d\n", rc);
            return rc;
        }
    }

    closels (hls);
    return 0;
EXIT:
    print_usage (sh_addls);
    return -1;
}

static int
sh_attrls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media;
    char *pos;
    int attribute;
    int rc;

    if (argc < 4)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;

    pos = argv[2];
    attribute = strtol (argv[3], NULL, 0);

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    rc = setattrls (hls, pos, attribute);
    if (rc < 0) {
        ERROR ("setattrls(), rc=%d\n", rc);
        goto EXIT;
    }

    closels (hls);
    return 0;
EXIT:
    print_usage (sh_attrls);
    return -1;
}

/*****************************************************************************/

command_init (sh_testls, "testls", "testls <fs>/<media>");
#define FLAG_READ       (1 << 0)
#define FLAG_PROTECT    (1 << 1)

static int
add_fat_file (char *name, int length)
{
    struct fd32 fd[1];
    char buf[512];
    int i, cnt;
    int rc;

    for (i = 0; i < 512; i++)
        buf[i] = i;

    rc = _open (fd, name, length, FS2_WRONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        return rc;
    }

    for (i = 0; i < length; i += cnt) {
        cnt = MIN (length - i, 512);
        rc = _write (fd, buf, cnt);
        if (rc < 0) {
            ERROR ("_write(), rc=%d\n", rc);
            return rc;
        }
    }

    _close (fd);
    return 0;
}

static int
add_nor_file (char *name, int length)
{
    struct norfs_fd fd[1];
    char buf[256];
    int i, cnt;
    int rc;

    for (i = 0; i < 256; i++)
        buf[i] = i;

    rc = norfs_open (fd, name, length, NOR_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        return rc;
    }

    for (i = 0; i < length; i += cnt) {
        cnt = MIN (length - i, 512);
        rc = norfs_write (fd, buf, cnt);
        if (rc < 0) {
            ERROR ("norfs_write(), rc=%d\n", rc);
            return rc;
        }
    }

    norfs_close (fd);
    return 0;
}

static int
test_add_file (HLS *hls, int idx)
{
    char name[16], fname[40];
    char *pos;
    int nls, npick;
    int rc;

    rc = gettotalls (hls, &nls, &npick, 1, 1);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        return rc;
    }
    dbg (0, "nls: %d, npick: %d\n", nls, npick);

    sprintf (name, "file%02d", idx);
    rc = getnewls (hls, &pos, name, 1, FLAG_PROTECT, FLAG_READ);
    if (rc < 0) {
        WARN ("getnewls(), rc=%d\n", rc);
        return 0;
    }

    sprintf (fname, "aumsg/%s", pos);
    dbg (0, "fname: %s\n", fname);
    if (hls->filesystem == NORFS) {
        rc = add_nor_file (fname, 1024);
        if (rc < 0) {
            ERROR ("add_nor_file(), rc=%d\n", rc);
            return rc;
        }
    } else {
        rc = add_fat_file (fname, 1024);
        if (rc < 0) {
            ERROR ("add_fat_file(), rc=%d\n", rc);
            return rc;
        }
    }

    if (idx % 2 == 0) {
        rc = setattrls (hls, pos, FLAG_READ);
        if (rc < 0) {
            ERROR ("setattrls(), rc=%d\n", rc);
            return rc;
        }
    }
    if (idx % 4 == 0) {
        rc = setattrls (hls, pos, FLAG_PROTECT);
        if (rc < 0) {
            ERROR ("setattrls(), rc=%d\n", rc);
            return rc;
        }
    }

    /* successful */
    return 1;
}

static int
test_delete_oldest (HLS *hls)
{
    char *pos;
    int attribute;
    int nls, npick;
    int rc;

    rc = gettotalls (hls, &nls, &npick, 0, 0);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        return rc;
    }
    dbg (0, "nls: %d, npick: %d\n", nls, npick);

    rc = pos2headls (hls);
    if (rc < 0) {
        ERROR ("pos2taills(), rc=%d\n", rc);
        return rc;
    }

    rc = getnextls (hls, &pos, &attribute);
    if (rc < 0) {
        WARN ("getnextls(), rc=%d\n", rc);
        return 0;
    }
    dbg (0, "pos: %s, attribute: %02x\n", pos, attribute);

    rc = deletels (hls);
    if (rc < 0) {
        ERROR ("deletels(), rc=%d\n", rc);
        return rc;
    }

    return 1;
}

static int
test_delete_newest (HLS *hls)
{
    char *pos;
    int attribute;
    int nls, npick;
    int rc;

    rc = gettotalls (hls, &nls, &npick, 0, 0);
    if (rc < 0) {
        ERROR ("gettotalls(), rc=%d\n", rc);
        return rc;
    }
    dbg (0, "nls: %d, npick: %d\n", nls, npick);

    rc = pos2taills (hls);
    if (rc < 0) {
        ERROR ("pos2taills(), rc=%d\n", rc);
        return rc;
    }

    rc = getprevls (hls, &pos, &attribute);
    if (rc < 0) {
        WARN ("getprevls(), rc=%d\n", rc);
        return 0;
    }
    dbg (0, "pos: %s, attribute: %02x\n", pos, attribute);

    rc = deletels (hls);
    if (rc < 0) {
        ERROR ("deletels(), rc=%d\n", rc);
        return rc;
    }

    return 1;
}

static int
sh_testls (int argc, char *argv[], void **result, void *sys_ctx)
{
    HLS *hls;
    int filesystem, media;
    int idx = 0;
    int rc;

    if (argc < 2)
        goto EXIT;

    if (parse_filesystem_media (argv[1], &filesystem, &media) < 0)
        goto EXIT;

    hls = openls (media, filesystem);
    if (hls == NULL) {
        ERROR ("openls()\n");
        goto EXIT;
    }

    for (idx = 0;; idx++) {
        rc = test_add_file (hls, idx);
        if (rc < 0) {
            ERROR ("test_add_file(), rc=%d\n", rc);
            goto EXIT;
        } else
        if (rc == 0) {
            WARN ("dir is full\n");
            break;
        }
    }

    while (1) {
        rc = test_delete_oldest (hls);
        if (rc < 0) {
            ERROR ("test_delete_oldest(), rc=%d\n", rc);
            goto EXIT;
        } else
        if (rc == 0) {
            WARN ("dir is empty\n");
            break;
        }

        rc = test_delete_newest (hls);
        if (rc < 0) {
            ERROR ("test_delete_newest(), rc=%d\n", rc);
            goto EXIT;
        } else
        if (rc == 0) {
            WARN ("dir is empty\n");
            break;
        }
    }

    closels (hls);
    return 0;
EXIT:
    print_usage (sh_testls);
    return -1;
}
