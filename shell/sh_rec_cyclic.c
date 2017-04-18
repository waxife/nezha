/**
 *  @file   sh_rec_cyclic.c
 *  @brief  cyclic record test
 *  $Id: sh_rec_cyclic.c,v 1.9 2015/08/05 10:06:22 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <codec.h>
#include <event.h>
#include <cache.h>
#include <debug.h>
#include <mconfig.h>
#include <ls.h>
#include <time.h>
#include <drivers/input/keypad.h>
#include <codec_eng/recfile.h>
#include "display.h"
#include "cvbs.h"
#include "./drivers/cq/cq.h"

command_init (sh_rec_cyclic, "rec_cyclic", "rec_cyclic");
command_init (sh_mk_hdr, "mk_hdr", "mk_hdr");

unsigned char user_info[32];
static struct rec_opt_t rec_opt_default = {
    MEDIA_VIDEO,    /* media type */
    720,            /* width */
    288,            /* height */
    AVIN_CVBS_PAL,  /* video standard */
    0,             /* record time (sec) */
    0,              /* record frame numbers (0 is ignore) */
    40 * 1024,      /* maximum frame size (bytes) */
    50,             /* quality (from 10 to 100) */
    0,              /* (1 / 2^fps_ratio) frame rate. only support 0 , 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps) */
    1,              /* set 1 to skip AVI header, or 0 */
    user_info,      /* user info data */
    0               /* user info data length */
};

int quit = 0;

static int rec_cyclic_cb(void **arg)
{
    struct sysc_status_t status;
    static int last_time = -1;
    static char disp_rec_time = 0;
    int cmd = 0;
    int ch = 0;
    int rc = 0;

    if (disp_rec_time) {
        rc = recfile_status(&status);
        if (rc < 0) {
            printf("Failed to get record file status. (%d)\n", rc);
        } else {
            if (status.time != last_time) {
                last_time = status.time;
                printf("%d (total %d frames)\n", status.time, status.fno);
            }
        }
    }

    ch = getb2();
    if (ch > 0) {
        switch (ch) {
        case 'q':
            cmd = CMD_QUIT;
            quit = 1;
            break;
        case 't':
            disp_rec_time = !disp_rec_time;
            if (!disp_rec_time)
                printf("not ");

            printf("print recording time...\n");
            break;
        }
    }

    return cmd;
}

static int sh_rec_cyclic(int argc, char **argv, void **result, void *sys_ctx)
{
    struct rec_opt_t *rec_opt = &rec_opt_default;
    HLS *hls = NULL;
    char fname[32] = { 0 }, *pfname = fname;
    struct sysc_status_t status;
    int rc = 0;
    char msgbuf[ERR_MSG_LEN] = { 0 };
    int file_cnt = -1;

    /* 0. set CVBS-in liveview */
    display_init();
    display_bklight_on();
    display_set_cvbs_full(VIDEO_STD_PAL, CH0);
    cvbs2_bluescreen_on(0,0,0,0);

    /* 1. open VIDEO list of file system */
    hls = openls(VIDEO_LS, FATFS);

    quit = 0;
    while (rc >= 0 && !quit) {
        /* 2. file name is current time */
	    struct tm time;
	    rtc_get_time(&time);
        file_cnt = (file_cnt == 9999) ? 0 : (file_cnt + 1);
	    snprintf(fname, 24, "%04d%02d%02d_%02d%02d%02d_%04d.avi",
                 time.tm_year + 1970, time.tm_mon + 1, time.tm_mday,
                 time.tm_hour, time.tm_min, time.tm_sec, file_cnt);

        /* 3. get free file in file system, replace oldest file if no free file */
        rc = getnewls(hls, &pfname, pfname, 1, 0, 0);
        if (rc < 0) {
            printf("Failed to generate record file name: %s.\n", fname);
            break;
        }

        /* 4. record file */
        rc = recfile(fname, rec_opt, rec_cyclic_cb);
        if (rc == CMD_STOP_ERR) {
            printf("recfile return error command %d.\n", rc);
            rc = get_err(msgbuf);
            if (rc) {
                printf("get error %d, message : %s\n", rc, msgbuf);
                break;
            }
        }

    }

    /* get end state */
    rc = recfile_status(&status);
    if (rc < 0) {
        printf("refile_status() return error %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    } else {
        printf("state = %d\n", status.state);
        printf("fno = %d\n", status.fno);
        printf("time = %d\n", status.time);
        printf("quality = %d\n", status.quality);
    }

    if (hls)
        closels(hls);

    return 0;
}

static int sh_mk_hdr(int argc, char **argv, void **result, void *sys_ctx)
{
    struct rec_opt_t *rec_opt = &rec_opt_default;
    HLS *hls = NULL;
    int rc = 0;
    char msgbuf[ERR_MSG_LEN];
    int fs = FATFS;
    int media = MEDIA_VIDEO;
    char dirname[] = "video";
    char fname[32] = { 0 }, *pfname = fname;
    int nfiles = 40;
    int file_size = 128 * 1024 * 1024;
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'v':
                    rec_opt->standard = atoi(argv[++i]);
                    if (rec_opt->standard == AVIN_CVBS_PAL) {
                        rec_opt->width = 720;
                        rec_opt->height = 288;
                    } else if (rec_opt->standard == AVIN_CVBS_NTSC) {
                        rec_opt->width = 720;
                        rec_opt->height = 240;
                    } else {
                        printf("Invalid video standard %d. (%d for PAL, %d for NTSC)\n",
                               rec_opt->standard, AVIN_CVBS_PAL, AVIN_CVBS_NTSC);
                        goto EXIT;
                    }

                    break;
                case 's':
                    rec_opt->size = atoi(argv[++i]) * 1024;
                    break;
                case 't':
                    rec_opt->time = atoi(argv[++i]);
                    break;
                case 'f':
                    rec_opt->fno = atoi(argv[++i]);
                    break;
                case 'r':
                    rec_opt->fps_ratio = atoi(argv[++i]);
                    if (rec_opt->fps_ratio < 0 || rec_opt->fps_ratio > 3) {
                        printf("Invalid frame rate ratio %d.\n", rec_opt->fps_ratio);
                        printf("Only support 0 (no change), 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps)\n");
                        goto EXIT;
                    }

                    break;
                case 'i':
                    rec_opt->user_info = argv[++i];
                    rec_opt->user_info_len = strlen(argv[i]); 
                    break;
                case 'n':
                    nfiles = atoi(argv[++i]);
                    break;
                case 'S':
                    file_size = atoi(argv[++i]);
                    break;
            }
        }
    }

    /* 1. media config */
    printf("media config: fs=%d, media=%d, directory=\"%s\", nfiles=%d, file_size=%d\n",
           fs, media, dirname, nfiles, file_size);
    rc = media_config(fs, media, dirname, nfiles, file_size);
    if (rc < 0) {
        printf("media_config(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = media_config_done();
    if (rc < 0) {
        printf("media_config_done, rc=%d\n", rc);
        goto EXIT;
    }

    /* 2. format SD */
    _umount();  /* can NOT format SD if SD is mounted */
    rc = _format ();
    if (rc < 0) {
        printf("format SD failed, rc=%d\n", rc);
        goto EXIT;
    }

    /* 3. mount SD */
    rc = sdc_status();
    if (rc != SDC_MOUNTED) {
        printf("mount SD failed, rc=%d\n", rc);
        goto EXIT;
    }

    hls = openls(VIDEO_LS, FATFS);

    /* 4. pre-open video files */
    printf("pre-open video %d files to cyclic record %d sec (%d.%03d fps) PAL video.\n",
           nfiles, rec_opt->time, 50 / (1 << rec_opt->fps_ratio), (50000 / (1 << rec_opt->fps_ratio) % 1000));
    for (i = 0; i < nfiles; i++) {
        snprintf(fname, 9, ".%04d.avi", i);
        rc = getnewls(hls, &pfname, pfname, 1, 0, 0);
        rc = mk_recfile_hdr(fname, rec_opt);
        if (rc < 0) {
            rc = get_err(msgbuf);
            if (rc) {
                printf("get error %d, message : %s\n", rc, msgbuf);
            }
        }

        printf(".");
    }

    for (i = 0; i < nfiles; i++) {
        snprintf(fname, 15, "%s/.%04d.avi", dirname, i);
        _remove(fname);
    }

    printf("\nprepare cyclic record files finish.\n");

EXIT:
    return rc;
}
