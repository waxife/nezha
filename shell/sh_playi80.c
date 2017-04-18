/**
 *  @file   sh_playi80.c
 *  @brief 
 *  $Id: sh_playi80.c,v 1.2 2015/07/24 13:29:25 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2015 Terawins Inc. All rights reserved.
 * 
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
#include <ls.h>
#include <time.h>
#include <display.h>
#include <keymap.h>

#define DBG_LEVEL   0
#include <debug.h>

#include <event.h>
#include <command.h>
#include "codec_eng/avifmt.h"
#include "codec_eng/playi80.h"

command_init (sh_playi80, "playi80", "playi80 <name> [<sync> <start> <stop>]");
command_init (sh_playi80ls, "playi80ls", "playi80ls [<sync> <start> <stop>]");

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
static short show_time;
static int last_state;

static void playfile_status (void)
{
    struct sysc_status_t status;
    struct tm *tm;
    int rc;

    rc = playi80_status (&status);
    if (rc < 0) {
        ERROR ("playi80_status(), rc=%d\n", rc);
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

void playi80ls_help(void)
{
    printf("n: next video\n");
    printf("v: previous video\n");
    printf("p: pause/resume\n");
    printf("t: enable/disable show time\n");
    printf("s: print status\n");
    printf("q: quit\n");
    printf("h: help\n");
}

static int playi80_cmd = 0;

static int playi80_cmd_callback (void **arg)
{
    struct sysc_status_t status;
    int cmd = 0;
    int ch = 0;
    int rc = 0;
    time_t frm_time = 0;
    static time_t old_frm_time = 0;

    if (show_time && playi80_get_time(&frm_time) == 0) {
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
            playi80_cmd = CMD_QUIT;
            break;
        case 's':
            cmd = CMD_STOP;
            break;
        case 'p':
            playi80_status(&status);
            cmd = (status.state == PAUSE) ? CMD_PLAY : CMD_PAUSE;
            break;
        case '.':
            cmd = CMD_STEP;
            break;
        case 'v':
            cmd = CMD_PREVIOUS;
            playi80_cmd = CMD_PREVIOUS;
            break;
        case 'n':
            cmd = CMD_NEXT;
            playi80_cmd = CMD_NEXT;
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

    rc = playi80_status(&status);
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

static int sh_playi80 (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
    struct play_opt_t opt;
    char msgbuf[ERR_MSG_LEN];
    int rc;

    if (argc < 2)
        goto EXIT;

    fname = argv[1];
    opt.sync = argc > 2 ? atoi(argv[2]) : 0;    /* default: 1 (sync) */
    opt.start = argc > 3 ? atoi(argv[3]) : 0;   /* default: 0 (play) */
    opt.stop = argc > 4? atoi(argv[4]) : 0;     /* default: 0 (quit) */
    printf ("[Option]\n"
            "  sync: %d\n"
            "  start: %d\n"
            "  stop: %d\n",
            opt.sync, opt.start, opt.stop);

    last_state = PLAY;
    show_time = 1;

    /* adjust display */
    struct av_property prop;
    rc = playback_get_property (fname, &prop);
    if (rc < 0) {
        ERROR ("playback_get_video_property(), rc = %d\n", rc);
        goto EXIT;
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

#if 0
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
#endif

    /* starting decode */
    rc = playi80(fname, &opt, playi80_cmd_callback);
    printf ("playi80 return 0x%x (%s)\n", rc, key2str (ksCmd, ksCmdNum, rc));
    if (rc == CMD_STOP_ERR) {
        rc = get_err(msgbuf);
        printf("error code: %d, message: %s\n", rc, msgbuf); 
    }

    playfile_status ();
    return 0;

EXIT:
    print_usage (sh_playi80);
    return -1;
}

static int sh_playi80ls (int argc, char **argv, void **result, void *sys_ctx)
{
    HLS *hls = NULL;
    int rc = 0;
    char *fname = NULL;
    int fattrib = 0;
    int cmd = 0;
    struct play_opt_t opt;
    struct av_property prop;
    struct sysc_status_t status;
	
    playi80ls_help();

    opt.sync = 1;
    opt.start = 0;
    opt.stop = 0;

    show_time = 1;

    hls = openls(VIDEO_LS, FATFS);
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

    playi80_cmd = 0;
    while (1) {
        rc = playback_get_property (fname, &prop);
        if (rc < 0) {
            ERROR ("playback_get_video_property(), rc = %d\n", rc);
            goto EXIT;
        }

        printf("file name: %s\n", fname);
        cmd = playi80(fname, &opt, playi80_cmd_callback);
        if (cmd < 0) {
            ERROR("playi80 error\n");
            goto EXIT;
        }

        playi80_status(&status);
        if ((status.fno * 1000 / prop.v_info.fps) < (prop.duration - 1000)) {
            ERROR("decode no enough frames: only %d frames in %d ms\n.", status.fno, prop.duration);
            goto EXIT;
        }

        if (playi80_cmd == CMD_QUIT) {
            goto EXIT;
        } if (playi80_cmd == CMD_PREVIOUS) {
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
                if (playi80_cmd == CMD_NEXT) {
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
                } else {
                    break;
                }
            }
        }

        playi80_cmd = 0;
    }

EXIT:
    if (hls)
        closels(hls);

    return 0;
}
