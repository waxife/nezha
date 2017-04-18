/**
 *  @file   sh_auplay.c
 *  @brief  audio play command
 *  $Id: sh_auplay.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2012/06/20  joanthan    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <codec.h>
#include <event.h>
#include <cache.h>
#include <debug.h>
#include <drivers/input/keypad.h>
#include "rtc.h"

command_init (sh_auplay, "auplay", "auplay [-f 0:g726_16k | 1:pcm | 2:ulaw | 3:alaw | 4:g726_32k] [<name>]");

extern int decode_audio_start(void *sys_ctx, const char *fname, int format, evt_callback_t evt_callback, cmd_callback_t cmd_callback);

int sh_auplay (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0; 
    const char *fname = NULL;
    int i, format = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'f':
                    format = atoi(argv[++i]);
                    break;
            }
        } else {
            fname = argv[i];
        }
    }
    if (format < 0 || format > 4)
        goto EXIT;

    printf("AuPlay, format %d, filename %s\n", format, fname);

    rc = decode_audio_start(sys_ctx, fname, format, NULL, NULL);
    if (rc < 0)
        goto EXIT;

    return 0;

EXIT:
    print_usage(sh_auplay);
    return -1;
}

