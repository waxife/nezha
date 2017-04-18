/**
 *  @file   sh_playfile.c
 *  @brief  playfile command
 *  $Id: sh_playfile.c,v 1.55 2015/10/29 02:47:05 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.55 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/06  hugo    moved from sh_dec.c
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <shell.h>
#include <codec.h>
#include <fourcc.h>
#include <sys.h>
#include <ctype.h>
#include <serial.h>
#include <time.h>
#include <display.h>
#include <keymap.h>
#include <ls.h>

#define DBG_LEVEL   0
#include <debug.h>

#include <event.h>
#include <command.h>
#include "codec_eng/avifmt.h"
#include "codec_eng/playfile.h"

command_init (sh_playfile2, "playfile2", "playfile2 <name> [<sync> <start> <stop>]");
command_init (sh_playls, "playls", "playls");
command_init (sh_magic, "magic", "magic [<format>]");
command_init (sh_prop, "prop", "prop <name>");
command_init (sh_duration, "duration", "duration <name>");
command_init (sh_vfno, "vfno", "vfno <name>");

static const KeyString ksStatus[] = {
    {ST_NONE,       "ST_NONE"},
    {ST_PLAYFILE,   "ST_PLAYFILE"},
    {ST_RECFILE,    "ST_RECFILE"},
    {ST_DECPIC,     "ST_DECPIC"},
    {ST_ENCPIC,     "ST_ENCPIC"},
    {ST_DECAU,      "ST_DECAU"},
    {ST_ENCAU,      "ST_ENCAU"},
    {ST_SLIDE,      "ST_SLIDE"},
    {ST_MDVID,      "ST_MDVID"},
    {ST_LSF,        "ST_LSF"},
};
#define ksStatusNum (sizeof(ksStatus)/sizeof(ksStatus[0]))

static const KeyString ksState[] = {
    {NONE,          "NONE"},
    {PLAY,          "PLAY"},
    {REC,           "REC"},
    {PAUSE,         "PAUSE"},
    {STOP_SOF,      "STOP_SOF"},
    {STOP_EOF,      "STOP_EOF"},
    {STOP_USER,     "STOP_USER"},
    {STOP_MD,       "STOP_MD"},
    {STOP_ERR,      "STOP_ERR"},
};
#define ksStateNum  (sizeof(ksState)/sizeof(ksState[0]))

static const KeyString ksCmd[] = {
    MAKE_KEY_STRING (CMD_QUIT),
    MAKE_KEY_STRING (CMD_STOP),
    MAKE_KEY_STRING (CMD_PLAY),
    MAKE_KEY_STRING (CMD_PAUSE),
    MAKE_KEY_STRING (CMD_STEP),
    MAKE_KEY_STRING (CMD_PREVIOUS),
    MAKE_KEY_STRING (CMD_NEXT),
    MAKE_KEY_STRING (CMD_DELETE),
    MAKE_KEY_STRING (CMD_FFW),
    MAKE_KEY_STRING (CMD_FBW),
    MAKE_KEY_STRING (CMD_DOORBELL),
    MAKE_KEY_STRING (CMD_QUIT_ERR),
};
#define ksCmdNum    (sizeof(ksCmd)/sizeof(ksCmd[0]))

static const KeyString ksStd[] = {
    {1, "NTSC"},
    {2, "PAL"},
};
#define ksStdNum   (sizeof(ksStd)/sizeof(ksStd[0]))

/*****************************************************************************
 * dec video
 *****************************************************************************/
static char show_time;
static int last_state;

static void playfile_status (void)
{
    struct sysc_status_t status;
    struct tm *tm;
    int rc;

    rc = play_status (&status);
    if (rc < 0) {
        ERROR ("play_status(), rc=%d\n", rc);
        return;
    }

    tm = localtime ((time_t *)&status.time);

    printf ("status=%s, state=%s, fno=%d, time=%4d/%02d/%02d %02d:%02d:%02d\n",
            key2str (ksStatus, ksStatusNum, status.status),
            key2str (ksState, ksStateNum, status.state),
            status.fno,
            tm->tm_year + 1970, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

}

static int playfile_cmd_callback (void **arg)
{
    struct sysc_status_t status;
    int cmd = 0;
    int ch = 0;
    int rc = 0;
    time_t frm_time = 0;
    static time_t old_frm_time = 0;

    if (show_time && playback_get_time(&frm_time) == 0) {
        if (old_frm_time != frm_time) {
            struct tm *sys_tm;
            old_frm_time = frm_time;
            sys_tm = localtime (&frm_time);
            printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                    sys_tm->tm_year + 1970, sys_tm->tm_mon + 1, sys_tm->tm_mday,
                    sys_tm->tm_hour, sys_tm->tm_min, sys_tm->tm_sec);
        }
    }

    ch = getb2();
    if (ch > 0) {
        switch (ch) {
        case 'q':
            cmd = CMD_QUIT;
            break;
        case 's':
            cmd = CMD_STOP;
            break;
        case 'p':
            play_status(&status);
            cmd = (status.state == PAUSE) ? CMD_PLAY : CMD_PAUSE;
            break;
        case '.':
            cmd = CMD_STEP;
            break;
        case 'v':
            cmd = CMD_PREVIOUS;
            break;
        case 'n':
            cmd = CMD_NEXT;
            break;
        case 'd':
            cmd = CMD_DELETE;
            break;
        case 'f':
            cmd = CMD_FFW;
            break;
        case 'b':
            cmd = CMD_FBW;
            break;
        case 'B':
            cmd = CMD_DOORBELL;
            break;
        case 't':
            show_time = !show_time;
            break;
        case 'S':   // upper case
            playfile_status ();
            break;
        default:
            cmd = 0;
            break;
        }
    }

    if (cmd > 0) {
        printf("%s\n", key2str (ksCmd, ksCmdNum, cmd));
    }

    rc = play_status(&status);
    if (rc == 0) {
        if (status.state != last_state) {
            printf("state: %s\n", key2str (ksState, ksStateNum, status.state));
            last_state = status.state;
            if (status.state == STOP_ERR) {
                printf("get STOP_ERR and send CMD_STOP_ERR\n");
                cmd = CMD_STOP_ERR;
            }
        }
    }

    return cmd;
}

static int sh_playfile2 (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
    struct play_opt_t opt;
    char msgbuf[ERR_MSG_LEN];
    int rc;

    if (argc < 2)
        goto EXIT;

    fname = argv[1];
    opt.sync = argc > 2 ? atoi(argv[2]) : 1;    /* default: 1 (sync) */
    opt.start = argc > 3 ? atoi(argv[3]) : 0;   /* default: 0 (play) */
    opt.stop = argc > 4 ? atoi(argv[4]) : 0;     /* default: 0 (quit) */
    opt.media = argc > 5 ? atoi(argv[5]) : 2;    /* default: 2 (MEDIA_VIDEO) */
    printf ("[Option]\n"
            "  sync: %d\n"
            "  start: %d\n"
            "  stop: %d\n"
            "  media: %d (2:MEDIA_VIDEO, 3:MEDIA_USER0, 4:MEDIA_USER1)\n",
            opt.sync, opt.start, opt.stop, opt.media);

    last_state = PLAY;
    show_time = 1;

    /* adjust display */
    struct av_property prop;
    if (opt.media != MEDIA_VIDEO) {
        rc = playfile_get_property (opt.media, fname, &prop);
        if (rc < 0) {
            ERROR ("playfile_get_property(), rc = %d\n", rc);
            goto EXIT;
        }
    } else {
        rc = playback_get_property (fname, &prop);
        if (rc < 0) {
            ERROR ("playback_get_video_property(), rc = %d\n", rc);
            goto EXIT;
        }
    }

    printf ("[video]\n"
            "  resolution: %d x %d\n"
            "  fps: %d\n"
            "  quality: %d\n"
            "  standard: %d (%s)\n"
            "  user_info: %s\n",
            prop.v_info.width, prop.v_info.height,
            prop.v_info.fps,
            prop.v_info.quality,
            prop.v_info.standard, key2str (ksStd, ksStdNum, prop.v_info.standard),
            prop.v_info.user_info);
    printf ("[audio]\n"
            "  format: %d\n"
            "  sample_rate: %d\n"
            "  channels: %d\n"
            "  bit_per_sample: %d\n",
            prop.a_info.format,
            prop.a_info.sample_rate,
            prop.a_info.channels,
            prop.a_info.bit_per_sample);
    printf ("time: %04d/%02d/%02d-%02d:%02d:%02d\n",
            prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec);
    printf ("duration: %d ms\n\n", prop.duration);

    /* adjust display */
    if (prop.v_info.standard == 1) {
        display_set_play_full (VIDEO_STD_NTSC);
    } else
    if (prop.v_info.standard == 2) {
        display_set_play_full (VIDEO_STD_PAL);
    } else {
        WARN ("Unknown video standard, guess according to fps\n");
        display_set_play_full (prop.v_info.fps == 50 ? VIDEO_STD_PAL : VIDEO_STD_NTSC);
    }

    /* starting decode */
    rc = playfile2(fname, &opt, playfile_cmd_callback);
    printf ("playfile2 return 0x%x (%s)\n", rc, key2str (ksCmd, ksCmdNum, rc));
    if (rc == CMD_STOP_ERR) {
        rc = get_err(msgbuf);
        printf("error code: %d, message: %s\n", rc, msgbuf); 
    }

    playfile_status ();
    return 0;

EXIT:
    print_usage (sh_playfile2);
    return -1;
}
/*****************************************************************************
 * decode magic helper
 *****************************************************************************/
#include "display.h"
#include "drivers/cq/cq.h"
#include "drivers/scaler/scaler.h"
static int
sh_magic (int argc, char **argv, void **result, void *sys_ctx)
{
    int format;

    if (argc > 1 && strcasecmp (argv[1], "ntsc"))
        format = VIDEO_STD_NTSC;
    else
        format = VIDEO_STD_PAL;

    scaler_open();
    cq_init();
    display_init();
    display_set_play_full(format);
    display_bklight_on();
    writel (0x07, 0xb0400910); /* CVD auto detection */

    return 0;
}


/*****************************************************************************
 * video property
 *****************************************************************************/
static int
sh_prop (int argc, char **argv, void **result, void *sys_ctx)
{
    struct av_property prop;
    MEDIA media;
    int rc = 0;

    memset(&prop, 0, sizeof(struct av_property));
    media = argc > 2 ? atoi(argv[2]) : MEDIA_VIDEO;
    if (media != MEDIA_VIDEO) {
        rc = playfile_get_property (media, argv[1], &prop);
        if (rc < 0) {
            ERROR ("playfile_get_property(), rc = %d\n", rc);
            goto EXIT;
        }
    } else {
        rc = playback_get_property (argv[1], &prop);
        if (rc < 0) {
            ERROR ("playback_get_video_property(), rc = %d\n", rc);
            goto EXIT;
        }
    }

    printf("%s : %dx%d@%d, %d ms, %04d/%02d/%02d-%02d:%02d:%02d\"%s\"\n", argv[1],
            prop.v_info.width, prop.v_info.height, prop.v_info.fps, prop.duration,
            prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec,
            prop.v_info.user_info);

    return 0;

EXIT:
    print_usage (sh_prop);
    return -1;
}


/*****************************************************************************
 * video duration
 *****************************************************************************/
static int
sh_duration (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned long long start_t = 0, end_t = 0;
    int duration = 0;
    MEDIA media;
    int rc = 0;

    if (argc != 2) {
        printf("Invalid argument.\n");
        goto EXIT;
    }

    start_t = read_cpu_count();

    media = argc > 2 ? atoi(argv[2]) : MEDIA_VIDEO;
    if (media != MEDIA_VIDEO) {
        rc = playfile_get_duration (media, argv[1], &duration);
        if (rc < 0) {
            ERROR ("playfile_get_property(), rc = %d\n", rc);
            goto EXIT;
        }
    } else {
        rc = playback_get_duration (argv[1], &duration);
        if (rc < 0) {
            ERROR ("playback_get_video_property(), rc = %d\n", rc);
            goto EXIT;
        }
    }

    end_t = read_cpu_count();

    printf("video [%s] duration : %d ms\n", argv[1], duration);

    printf("(function time : %ld ms)\n", hz2sec(end_t - start_t) * 1000 + hz2msec(end_t - start_t));

    return 0;

EXIT:
    print_usage (sh_duration);
    return -1;
}


/*****************************************************************************
 * video duration
 *****************************************************************************/
static int
sh_vfno (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned long long start_t = 0, end_t = 0;
    int vfno = 0;
    MEDIA media;
    int rc = 0;

    if (argc != 2) {
        printf("Invalid argument.\n");
        goto EXIT;
    }

    start_t = read_cpu_count();
    media = argc > 2 ? atoi(argv[2]) : MEDIA_VIDEO;
    if (media != MEDIA_VIDEO) {
        rc = playfile_get_vfno (media, argv[1], &vfno);
        if (rc < 0) {
            ERROR ("playfile_get_vfno(), rc = %d\n", rc);
            goto EXIT;
        }
    } else {
        rc = playback_get_vfno (argv[1], &vfno);
        if (rc < 0) {
            ERROR ("playback_get_vfno(), rc = %d\n", rc);
            goto EXIT;
        }
    }

    end_t = read_cpu_count();

    printf("video [%s] has %d frame(s).\n", argv[1], vfno);

    printf("(function time : %ld ms)\n", hz2sec(end_t - start_t) * 1000 + hz2msec(end_t - start_t));

    return 0;

EXIT:
    print_usage (sh_duration);
    return -1;
}

void playls_help(void)
{
    printf("n: next video\n");
    printf("v: previous video\n");
    printf("p: pause/resume\n");
    printf("t: enable/disable show time\n");
    printf("s: print status\n");
    printf("q: quit\n");
    printf("h: help\n");
}

static int playls_cmd = 0;

static int playls_cmd_callback (void **arg)
{
    struct sysc_status_t status;
    int cmd = 0;
    int ch = 0;
    int rc = 0;
    time_t frm_time = 0;
    static time_t old_frm_time = 0;

    if (show_time && playback_get_time(&frm_time) == 0) {
        if (old_frm_time != frm_time) {
            struct tm *sys_tm;
            old_frm_time = frm_time;
            sys_tm = localtime (&frm_time);
            printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                   sys_tm->tm_year + 1970, sys_tm->tm_mon + 1, sys_tm->tm_mday,
                   sys_tm->tm_hour, sys_tm->tm_min, sys_tm->tm_sec);
        }
    }

    ch = getb2();
    if (ch > 0) {
        switch (ch) {
        case 'q':
            cmd = CMD_QUIT;
            playls_cmd = CMD_QUIT;
            break;
        case 'p':
            play_status(&status);
            cmd = (status.state == PAUSE) ? CMD_PLAY : CMD_PAUSE;
            break;
        case 'v':
            cmd = CMD_PREVIOUS;
            playls_cmd = CMD_PREVIOUS;
            break;
        case 'n':
            cmd = CMD_NEXT;
            playls_cmd = CMD_NEXT;
            break;
        case 't':
            show_time = !show_time;
            break;
        case 's': 
            playfile_status ();
            break;
        case 'h':
            playls_help();
            break;
        default:
            cmd = 0;
            break;
        }
    }

    if (cmd > 0) {
        printf("%s\n", key2str (ksCmd, ksCmdNum, cmd));
    }

    rc = play_status(&status);
    if (rc == 0) {
        if (status.state != last_state) {
            printf("state: %s\n", key2str (ksState, ksStateNum, status.state));
            last_state = status.state;
            if (status.state == STOP_ERR) {
                printf("get STOP_ERR and send CMD_STOP_ERR\n");
                cmd = CMD_STOP_ERR;
            }
        }
    }

    return cmd;
}

static int sh_playls (int argc, char **argv, void **result, void *sys_ctx)
{
    HLS *hls = NULL;
    int rc = 0;
    char *fname = NULL;
    int fattrib = 0;
    int cmd = 0;
    struct play_opt_t opt;
    struct sysc_status_t status;
    struct av_property prop;
	
    playls_help();

    opt.sync = 1;
    opt.start = 0;
    opt.stop = 0;
    opt.media = argc > 1 ? atoi(argv[1]) : 2;    /* default: 2 (MEDIA_VIDEO) */

    show_time = 1;

    hls = openls(opt.media, FATFS);
    if (hls == NULL) {
        goto EXIT;
    }

    rc = pos2headls (hls);
    if (rc < 0) {
        ERROR ("pos2headls(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = getnextls(hls, &fname, &fattrib);
    if (rc < 0) {
        ERROR("no video files.\n");
        goto EXIT;
    }

    playls_cmd = 0;
    while (1) {
        printf("file name: %s\n", fname);
        cmd = playfile2(fname, &opt, playls_cmd_callback);
        if (cmd < 0) {
            ERROR("playfile2 error\n");
            goto EXIT;
        }

        rc = play_status(&status);
        if (rc == 0) {
            if (status.state == STOP_EOF) {
                if (opt.media != MEDIA_VIDEO) {
                    rc = playfile_get_property (opt.media, fname, &prop);
                    if (rc < 0) {
                        ERROR ("playfile_get_property(), rc = %d\n", rc);
                        goto EXIT;
                    }
                } else {
                    rc = playback_get_property (fname, &prop);
                    if (rc < 0) {
                        ERROR ("playback_get_video_property(), rc = %d\n", rc);
                        goto EXIT;
                    }
                }
            }
        }

        if (playls_cmd == CMD_QUIT) {
            goto EXIT;
        } if (playls_cmd == CMD_PREVIOUS) {
            rc = getprevls(hls, &fname, &fattrib);
            if (rc < 0) {
                WARN("in tail now and to head!\n");
                rc = pos2taills (hls);
                if (rc < 0) {
                    ERROR ("pos2taills(), rc=%d\n", rc);
                    goto EXIT;
                }

                rc = getprevls(hls, &fname, &fattrib);
                if (rc < 0) {
                    ERROR ("getprevls(), rc=%d\n", rc);
                    goto EXIT;
                }
            }
        } else {
            rc = getnextls(hls, &fname, &fattrib);
            if (rc < 0) {
                WARN("in tail now and to head!\n");
                rc = pos2headls (hls);
                if (rc < 0) {
                    ERROR ("pos2headls(), rc=%d\n", rc);
                    goto EXIT;
                }

                rc = getnextls(hls, &fname, &fattrib);
                if (rc < 0) {
                    ERROR ("getnextls(), rc=%d\n", rc);
                    goto EXIT;
                }
            }
        }

        playls_cmd = 0;
    }

EXIT:
    if (hls)
        closels(hls);

    return 0;
}
