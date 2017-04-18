/**
 *  @file   mvfile.c
 *  @brief	API for move files
 *  $Id: mvfile.c,v 1.5 2014/02/10 10:39:12 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2013/12/18  onejoe 		New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <ctype.h>
#include <codec.h>
#include <codec_eng/decpic.h>
#define DBG_LEVEL   0
#include <debug.h>

#include <norfs.h>
#include <fat32.h>
#include <mconfig.h>
#include "mvfile.h"

#define MIN(A, B)           ((A) < (B) ? (A) : (B))
#define MAX(A, B)           ((A) > (B) ? (A) : (B))

int copysd2nor(int media_type, char *sdfile, char *norfile)
{
	struct fd32 fd_sd[1];
	struct norfs_fd fd_nor[1];
	char src_path[20];

    unsigned char buf[SECT_SIZE];
    struct stat_t st;
    unsigned int length;
    unsigned int i;
    int rc = 0, cnt;

    /* get file status */
    rc = _stat (sdfile, &st);
    if (rc < 0) {
        ERROR ("_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    //skip = argc > 3 ? size512 (argv[3]) : 0;
    length = st.st_size;

	switch (media_type) {
	case MEDIA_PIC:
		strcpy (src_path, "/picture/");
		strcat (src_path, sdfile);
		break;
	case MEDIA_AUMSG:
		strcpy (src_path, "/aumsg/");
		strcat (src_path, sdfile);
		break;
	case MEDIA_VIDEO:
		strcpy (src_path, "/video/");
		strcat (src_path, sdfile);
		break;
	default:
		strcpy (src_path, sdfile);
		break;
	}

    /* open file */
    rc = _open (fd_sd, src_path, 0, FS2_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

	rc = norfs_open (fd_nor, norfile, length, NOR_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    for (i = 0; i < length; i += SECT_SIZE) {
        /* read data */
        rc = _read (fd_sd, buf, SECT_SIZE);
        if (rc < 0) {
            ERROR ("_read(), rc=%d\n", rc);
            goto EXIT;
        }

		cnt = MIN (SECT_SIZE, length - i);
        rc = norfs_write (fd_nor, buf, cnt);
        if (rc < 0) {
            ERROR ("norfs_write(), rc=%d\n", rc);
            goto EXIT;
        }
    }

    /* close file */
    rc = _close (fd_sd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
	norfs_close (fd_nor);
    return 0;

EXIT:
//print_usage (sh_hexdump);
    return rc;
}

int aumsg_nor2sd (char *fullname, char *sdfile)
{
	struct norfs_fd fd_nor[1];
	struct fd32 fd_sd[1];

    struct norfs_stat st;
    int len_nor;
    int len_sd;

    char buf[SECT_SIZE];
    int i, cnt;
    int rc = 0;

    rc = norfs_stat (fullname, &st);
    if (rc < 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        goto EXIT;
    }

    len_nor = st.st_size;
	len_sd = (len_nor + (512 - 1)) & ~(512 - 1);//align 512

	/* open file */
    rc = norfs_open (fd_nor, fullname, 0, NOR_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = _open (fd_sd, sdfile, len_nor, FS2_WRONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        goto EXIT;
    }

    for (i = 0; i < len_nor; i += cnt) {
        cnt = MIN (len_nor - i, SECT_SIZE);
        rc = norfs_read (fd_nor, buf, cnt);
        if (rc < 0) {
            ERROR ("norfs_read(), rc=%d\n", rc);
            goto EXIT;
        }
		rc = _write (fd_sd, buf, cnt);
		if (rc < 0) {
            ERROR ("_write(), rc=%d\n", rc);
            goto EXIT;
        }
    }

EXIT:
    /* close file */
	rc = _close (fd_sd);
    if (rc < 0) {
        ERROR ("_close(), rc=%d\n", rc);
        goto EXIT;
    }
    norfs_close (fd_nor);
    if (rc < 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        goto EXIT;
    }

    return rc;
}

int copynor2sd(int media_type, char *norfile, char *sdfile)
{
    int rc = 0;
    char *dirname;
    int nfiles = 0, maxsize = 0;
    unsigned char fullname[64];

    rc = media_get_config(NORFS, media_type, &dirname, &nfiles, &maxsize);
    if (rc < 0) {
        dbg(0, "[ERROR] media config error, %d\n", rc);
        set_err(rc, "config error");
        goto EXIT;
    }

    snprintf(fullname, 64, "%s/%s", dirname, norfile);
    INFO(">>> fullname :%s\n", fullname);

    switch (media_type) {
    case MEDIA_PIC:
    	rc = pic_nor2sd(fullname, sdfile);
    	break;
    case MEDIA_AUMSG:
    	rc = aumsg_nor2sd(fullname, sdfile);
    	break;
    case MEDIA_VIDEO:
    	INFO("NORFS not support video type!\n");
		break;
    default:
    	ERROR("unknown media type!\n");
    	rc = -1;
    	goto EXIT;
    	break;
    }

EXIT:
	return rc;
}

