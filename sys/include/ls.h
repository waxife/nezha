/**
 *  @file   ls.h
 *  @brief  header file for file manipulate
 *  $Id: ls.h,v 1.19 2014/05/08 09:41:41 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.19 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/26  hugo    New file.
 *
 */
#ifndef __LS_H
#define __LS_H

#include <mconfig.h>
#include <fs.h>

#define PIC_LS      MEDIA_PIC
#define AUMSG_LS    MEDIA_AUMSG
#define VIDEO_LS    MEDIA_VIDEO

typedef struct hls HLS;
typedef int (*gettotalls_fn) (HLS *hls, int *nls, int *npick, int filter, int mask);
typedef int (*getnextls_fn) (HLS *hls, char **pos, int *attribute);
typedef int (*getprevls_fn) (HLS *hls, char **pos, int *attribute);
typedef int (*getcurrls_fn) (HLS *hls, char **pos, int *attribute);
typedef int (*deletels_fn) (HLS *hls);
typedef int (*getnewls_fn) (HLS *hls, char **pos, const char *fname, int victim, unsigned char alt, unsigned char mask);
typedef int (*setattrls_fn) (HLS *hls, char *pos, int attribute);

struct hls {
    int magic;
#define HLS_MAGIC   0xdeadbeef

    char media;
    char filesystem;
    char *dirname;
    int nfiles;
    int maxsize;
    char filename[NAMELEN+1];

    unsigned short  current;    /* FAT dirent index, NOR block index */
    unsigned short  mark[3];    /* mark pointer */
#define LINK_NULL   0xfff0
#define HEAD_NULL   0xfff1
#define TAIL_NULL   0xfff2

    gettotalls_fn   gettotalls;
    getnextls_fn    getnextls;
    getprevls_fn    getprevls;
    getcurrls_fn    getcurrls;
    deletels_fn     deletels;
    getnewls_fn     getnewls;
    setattrls_fn    setattrls;
};


/* public API */
HLS *openls (int media, int filesystem);
int gettotalls (HLS *hls, int *nls, int *npick, int filter, int mask);
int getnextls (HLS *hls, char **pos, int *attribute);
int getprevls (HLS *hls, char **pos, int *attribute);
int getcurrls (HLS *hls, char **pos, int *attribute);
int deletels (HLS *hls);
int getnewls (HLS *hls, char **pos, const char *fname, int victim, unsigned char filter, unsigned char alter);
int closels (HLS *hls);
int setattrls (HLS *hls, char *pos, int attribute);
int markls(HLS *hls, int mark);
int seekmarkls(HLS *hls, int mark);
int matchmarkls(HLS *hls, int mark);
#define eols(hls)       ((hls)->current == TAIL_NULL)
#define sols(hls)       ((hls)->current == HEAD_NULL)

/* helper functions */
int pos2headls (HLS *hls);
int pos2taills (HLS *hls);

#endif /* __LS_H */
