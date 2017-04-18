/**
 *  @file   mconfig
 *  @brief  header file for Medium Storage Config
 *  $Id: mconfig.h,v 1.7 2014/08/12 08:23:50 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/27  hugo    New file.
 *
 */
#ifndef __MCONFIG_H
#define __MCONFIG_H

typedef enum {NORFS, FATFS} FS;
#define FS_NUM      2

typedef enum {MEDIA_PIC, MEDIA_AUMSG, MEDIA_VIDEO, MEDIA_USER0, MEDIA_USER1} MEDIA;
#define MEDIA_NUM   5

#define MCONFIG_MAX_NFILES      1279
#define MCONFIG_OTHERS          -1

int media_config (FS fs, MEDIA media, char *dirname, int nfiles, int maxsize);
char *media_dirname (FS fs, MEDIA media);
int media_get_config(FS fs, MEDIA media, char **dirname, int *nfiles, int *maxsize);
int media_get_realconfig(FS fs, MEDIA media, char **dirname, int *nfiles, int *maxsize);
int media_config_done(void);
int media_config_reset(void);
void media_config_show(void);

#endif /* __MCONFIG_H */
