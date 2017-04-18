/**
 *  @file   sh_mconfig.c
 *  @brief  Medium Storage Config
 *  $Id: sh_mconfig.c,v 1.12 2014/08/27 06:26:41 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.12 $
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
#define DBG_LEVEL   4
#include <debug.h>
#include <mconfig.h>

command_init (sh_mconfig, "mconfig", "mconfig <fs> <media> <dirname> <nfiles> <maxsize>");
command_init (sh_mconfig_done, "mdone", "mdone");
command_init (sh_mconfig_reset, "mreset", "mreset");
command_init (sh_mconfig_all, "mall", "mall");
command_init (sh_mconfig_show, "mconfig_show", "mconfig_show");


int parse_media (char *type);
char *media_name (MEDIA media);

int parse_filesystem (char *type);
char *filesystem_name (FS fs);


static int
sh_mconfig (int argc, char *argv[], void **result, void *sys_ctx)
{
    int fs;
    int media;
    char *dirname;
    int nfiles;
    int maxsize;
    int rc;

    if (argc < 6)
        goto EXIT;

    fs = parse_filesystem (argv[1]);
    media = parse_media (argv[2]);
    dirname = argv[3];
    nfiles = atoi (argv[4]);
    maxsize = size (argv[5]);

    rc = media_config (fs, media, dirname, nfiles, maxsize);
    if (rc < 0) {
        ERROR ("media_config(), rc=%d\n", rc);
        goto EXIT;
    }

    return 0;
EXIT:
    print_usage (sh_mconfig);
    return -1;
}

static int
sh_mconfig_done (int argc, char *argv[], void **result, void *sys_ctx)
{
    int rc;

    rc = media_config_done();
    if (rc < 0) {
        ERROR("media_config_done, rc=%d\n", rc);
        goto EXIT;
    }

    media_config_show();

    return 0;
EXIT:
    print_usage(sh_mconfig_done);
    return -1;
}

static int
sh_mconfig_reset (int argc, char *argv[], void **result, void *sys_ctx)
{
    return media_config_reset ();
}

static int
sh_mconfig_all(int argc, char *argv[], void **result, void *sys_ctx)
{
    int rc;

    rc = media_config(FATFS, MEDIA_PIC, "picture", 1023, 128*1024);
    if (rc < 0) {
       ERROR ("media_config(), rc=%d\n", rc);
       goto EXIT;
    }
    rc = media_config(FATFS, MEDIA_AUMSG, "aumsg", 64, 64*1024);
    if (rc < 0) {
       ERROR ("media_config(), rc=%d\n", rc);
       goto EXIT;
    }

    rc = media_config(FATFS, MEDIA_VIDEO, "video", -1, 32*1024*1024);
    if (rc < 0) {
       ERROR ("media_config(), rc=%d\n", rc);
       goto EXIT;
    }

    rc = media_config(NORFS, MEDIA_AUMSG, "aumsg", 10, 64*1024);
    if (rc < 0) {
        ERROR ("media_config(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = media_config(NORFS, MEDIA_PIC, "picture", -1, 64*1024);
    if (rc < 0) {
       ERROR ("media_config(), rc=%d\n", rc);
       goto EXIT;
    }

    rc = media_config_done();
    if (rc < 0) {
        ERROR("media_config_done, rc = %d\n", rc);
        goto EXIT;
    }

    media_config_show(); 
   
    
    return 0;
EXIT:
    print_usage (sh_mconfig_all);
    return -1;
}



static int
sh_mconfig_show (int argc, char *argv[], void **result, void *sys_ctx)
{
    media_config_show(); 

    return 0;
}

/******************************************************************************
 * helper functions
 ******************************************************************************/

static KeyString ksMedia[] = {
    {MEDIA_PIC,     "pic"},
    {MEDIA_AUMSG,   "aumsg"},
    {MEDIA_VIDEO,   "video"},
    {MEDIA_USER0,   "user0"},
    {MEDIA_USER1,   "user1"},
};
#define ksMediaNum  (sizeof(ksMedia)/sizeof(ksMedia[0]))

static KeyString ksFS[] = {
    {NORFS,     "nor"},
    {FATFS,     "fat"},
};
#define ksFSNum  (sizeof(ksFS)/sizeof(ksFS[0]))

int parse_media (char *type)
{
    int rc;
    rc = str2key (ksMedia, ksMediaNum, type);
    if (rc < 0) {
        printf ("available media:\n");
        ksList (ksMedia, ksMediaNum);
    }
    return rc;
}

char *media_name (MEDIA media)
{
    return key2str (ksMedia,ksMediaNum, media);
}

int parse_filesystem (char *type)
{
    int rc;
    rc = str2key (ksFS, ksFSNum, type);
    if (rc < 0) {
        printf ("available filesystem:\n");
        ksList (ksFS, ksFSNum);
    }
    return rc;
}

char *
filesystem_name (FS fs)
{
    return key2str (ksFS, ksFSNum, fs);
}
