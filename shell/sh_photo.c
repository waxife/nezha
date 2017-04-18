/**
 *  @file   sh_phpto.c
 *  @brief  photo test command
 *  $Id: sh_photo.c,v 1.1.1.1 2013/12/18 03:43:40 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/10/02  hugo        New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <shell.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <photo.h>
#include <norfs.h>
#include <debug.h>

command_init (sh_photo_record, "photo_record", "photo_record <filename>");
command_init (sh_photo_play, "photo_play", "photo_play <filename>");

static int
sh_photo_record (int argc, char **argv, void **result, void *sys_ctx)
{
    struct photo_ctx ph[1];
    struct norfs_fd nfd[1];
    char *name;
#ifdef CONFIG_RESERVE_CODEC_BUFFER
    char *buf = (char *) CONFIG_STACK_ADDR_RSV_BUF;
#else
    char buf[CONFIG_NORFS_RECORD_MAX_SIZE+16];
#endif
    int length;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* read buffer */
    rc = photo_open (ph, PH_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("photo_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = length = photo_read (ph, buf, CONFIG_NORFS_RECORD_MAX_SIZE);
    if (rc < 0) {
        ERROR ("photo_read(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = photo_close (ph);
    if (rc < 0) {
        ERROR ("photo_close(), rc=%d\n", rc);
        goto EXIT;
    }

    /* write buffer */
    rc = norfs_open (nfd, name, length, NOR_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_write (nfd, buf, length);
    if (rc < 0) {
        ERROR ("norfs_write(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_close (nfd);
    if (rc < 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;


    return 0;
EXIT:
    print_usage (sh_photo_record);
    return rc;
}

static int
sh_photo_play (int argc, char **argv, void **result, void *sys_ctx)
{
    struct photo_ctx ph[1];
    struct norfs_fd nfd[1];
    char *name;
#ifdef CONFIG_RESERVE_CODEC_BUFFER
    char *buf = (char *) CONFIG_STACK_ADDR_RSV_BUF;
#else
    char buf[CONFIG_NORFS_RECORD_MAX_SIZE+16];
#endif
    int length;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* read buffer */
    rc = norfs_open (nfd, name, length, NOR_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_read (nfd, buf, length);
    if (rc < 0) {
        ERROR ("norfs_read(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_close (nfd);
    if (rc < 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        goto EXIT;
    }

    /* write buffer */
    rc = photo_open (ph, PH_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("photo_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = photo_write (ph, buf, length);
    if (rc < 0) {
        ERROR ("photo_write(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = photo_close (ph);
    if (rc < 0) {
        ERROR ("photo_close(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_photo_play);
    return rc;
}
