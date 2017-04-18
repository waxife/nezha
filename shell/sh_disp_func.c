/**
 *  @file   sh_disp_func.c
 *  @brief  panel test command
 *  $Id: sh_disp_func.c,v 1.9 2014/02/10 04:18:15 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/18  C.N.Yu  New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <debug.h>
#include <sys.h>
#include "display.h"
#include "cvbs.h"
#include "./drivers/cq/cq.h"
command_init (sh_disp_init, "disp_init", "disp_init");
command_init (sh_disp_on, "disp_on", "disp_on");
command_init (sh_disp_off, "disp_off", "disp_off");
command_init (sh_disp_set_cmos, "disp_set_cmos", "disp_set_cmos");
command_init (sh_disp_set_cvbs, "disp_set_cvbs", "disp_set_cvbs [ntsc|pal]");
command_init (sh_disp_set_cpu, "disp_set_cpu", "disp_set_cpu");
command_init (sh_disp_set_play, "disp_set_play", "disp_set_play [ntsc|pal]");
command_init (sh_disp_set_picview, "disp_set_picview", "disp_set_picview [ntsc|pal]");
command_init (sh_disp_set_playback, "disp_set_playback", "disp_set_playback [ntsc|pal]");
command_init (sh_disp_set_liveview, "disp_set_liveview", "disp_set_liveview [ntsc|pal]");
command_init (sh_disp_set_gdbar, "disp_set_gdbar", "disp_set_gdbar [r|g|b|w]");

static int
sh_disp_init (int argc, char **argv, void **result, void *sys_ctx)
{
    display_init();
    display_bklight_on();
    //Enable Color Bar
    cq_write_byte_issue(CQ_P0, 0x91, 0xB8, CQ_TRIGGER_SW);
   
    return 0;
}

static int
sh_disp_on (int argc, char **argv, void **result, void *sys_ctx)
{
    display_bklight_on();

    return 0;
}

static int
sh_disp_off (int argc, char **argv, void **result, void *sys_ctx)
{
    display_bklight_off();

    return 0;
}

static int
sh_disp_set_cmos (int argc, char **argv, void **result, void *sys_ctx)
{
 //   display_set_cmos();

    return 0;
}

static int
sh_disp_set_cvbs (int argc, char **argv, void **result, void *sys_ctx)
{
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        if (strcasecmp (argv[1], "ntsc") == 0) {
            format = VIDEO_STD_NTSC;
        } else if (strcasecmp (argv[1], "pal") == 0) {
            format = VIDEO_STD_PAL;
        } else {
            printf("ERROR: unknow format %s\n", argv[1]);
            goto EXIT;
        }
    }
    display_set_cvbs_full(format, CH0);
    cvbs2_bluescreen_on(0,0,0,0);
    return 0;

EXIT:
    print_usage (sh_disp_set_cvbs);
    return 0;
}

static int
sh_disp_set_cpu (int argc, char **argv, void **result, void *sys_ctx)
{
//    display_set_cpu();

    return 0;
}

static int
sh_disp_set_play (int argc, char **argv, void **result, void *sys_ctx)
{
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        if (strcasecmp (argv[1], "ntsc") == 0) {
            format = VIDEO_STD_NTSC;
        } else if (strcasecmp (argv[1], "pal") == 0) {
            format = VIDEO_STD_PAL;
        } else {
            printf("ERROR: unknow format %s\n", argv[1]);
            goto EXIT;
        }
    }
    display_set_play_full(format);

    return 0;

EXIT:
    print_usage (sh_disp_set_play);
    return 0;
}

static int
sh_disp_set_picview (int argc, char **argv, void **result, void *sys_ctx)
{
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        if (strcasecmp (argv[1], "ntsc") == 0) {
            format = VIDEO_STD_NTSC;
        } else if (strcasecmp (argv[1], "pal") == 0) {
            format = VIDEO_STD_PAL;
        } else {
            printf("ERROR: unknow format %s\n", argv[1]);
            goto EXIT;
        }
    }
    display_set_play_full(format);

    return 0;

EXIT:
    print_usage (sh_disp_set_picview);
    return 0;
}

static int
sh_disp_set_playback (int argc, char **argv, void **result, void *sys_ctx)
{
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        if (strcasecmp (argv[1], "ntsc") == 0) {
            format = VIDEO_STD_NTSC;
        } else if (strcasecmp (argv[1], "pal") == 0) {
            format = VIDEO_STD_PAL;
        } else {
            printf("ERROR: unknow format %s\n", argv[1]);
            goto EXIT;
        }
    }
    display_set_play_full(format);

    return 0;

EXIT:
    print_usage (sh_disp_set_play);
    return 0;
}

static int
sh_disp_set_liveview (int argc, char **argv, void **result, void *sys_ctx)
{
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        if (strcasecmp (argv[1], "ntsc") == 0) {
            format = VIDEO_STD_NTSC;
        } else if (strcasecmp (argv[1], "pal") == 0) {
            format = VIDEO_STD_PAL;
        } else {
            printf("ERROR: unknow format %s\n", argv[1]);
            goto EXIT;
        }
    }
    display_set_cvbs_full(format, CH0);

    return 0;

EXIT:
    print_usage (sh_disp_set_play);
    return 0;
}

static int
sh_disp_set_gdbar(int argc, char **argv, void **result, void *sys_ctx)
{
    int c = 'B';
    int format = VIDEO_STD_PAL;

    if (argc > 1) {
        c = *argv[1];
    }


    display_bklight_on();
    display_set_cvbs_full(format, CH0);

    if (c == 'r') {
        writel(0xa2, 0xb0400910);
    } else if (c == 'g') {
        writel(0xa3, 0xb0400910);
    } else if (c == 'b') {
        writel(0xa4, 0xb0400910);
    } else if (c == 'w') {
        writel(0xa5, 0xb0400910);
    } else {
        writel(0xa1, 0xb0400910);
    }

    writel(0x40, 0xb0401b80);

    return 0;
}

