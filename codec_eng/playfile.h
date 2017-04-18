/**
 *  @file   playfile.h
 *  @brief  header file for decode process
 *  $Id: playfile.h,v 1.42 2015/10/29 02:47:05 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.42 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/06  hugo    movied from dec_process.h
 *
 */
#ifndef __PLAYFILE_H
#define __PLAYFILE_H

#include "avifmt.h"
#include "frm_header.h"
#include "mconfig.h"

struct play_opt_t {
    char sync;  // 0: not sync, 1: sync with fno
    char stop;  // 0: quit, 1: stay first frame, 2: stay last frame
    unsigned char start; // 0: play, N: stay in Nth frame (N=1-255)
    MEDIA media;        /* media config, default is MEDIA_VIDEO*/
};

struct video_info {
    int width;
    int height;
    int fps;
    int quality;
    int standard;       /* 1:NTSC, 2:PAL */
    char user_info[JPEG_USER_INFO_LEN];
};

struct audio_info {
    int format;         /* 0:g726_16k, 1:pcm, 2:ulaw, 3:alaw, 4:g726_32k */
    int sample_rate;    /* sample rate */
    int channels;       /* channels */
    int bit_per_sample; /* bit per sample */
};

struct av_property {
    struct video_info v_info;
    struct audio_info a_info;
    int duration;       /* ms */

    /* video time field */
    int year;
    int mon;
    int mday;
    int hour;
    int min;
    int sec;
};

/* API */
typedef int (*callback_fn) (void **arg);

int playfile2 (const char *filename, struct play_opt_t *opt, callback_fn cmd);
int play_status (struct sysc_status_t *status);
int playback_get_property (char *filename, struct av_property *prop);
int playback_get_time (time_t *time);
int playback_get_duration (char *filename, int *duration);
int playback_get_vfno(char *filename, int *vfno);
int playfile_get_property (MEDIA media, char *filename, struct av_property *prop);
int playfile_get_duration (MEDIA media, char *filename, int *duration);
int playfile_get_vfno(MEDIA media, char *filename, int *vfno);
#endif /* __PLAYFILE_H */
