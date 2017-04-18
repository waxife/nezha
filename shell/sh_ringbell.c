/**
 *  @file   sh_audio.c
 *  @brief  audio test command
 *  $Id: sh_ringbell.c,v 1.3 2014/03/07 03:36:03 lym Exp $
 *  $Author: lym $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/09/03  hugo        New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <shell.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <serial.h>
#include <ctype.h>
#include <audio.h>
#include <debug.h>
#include "irq.h"
#include "interrupt.h"
#include <codec_eng/ringbell.h>
#include "volume.h"

command_init (sh_ringbell, "ringbell", "ringbell <idx> [<count>]");
command_init (sh_stopbell, "stopbell", "stopbell");
command_init (sh_vol_ctrl, "volumectr", "volumectr [<volume>]");

static int sh_ringbell (int argc, char **argv, void **result, void *sys_ctx)
{
    int idx;
	int count = 0;
    int rc = 0;
    
    if(argc < 2)
        goto EXIT;
    
    idx   = atoi(argv[1]);
    count = (argc > 2) ? atoi(argv[2]) : 1;
    
    printf("ringbell idx = %d, repeat count = %d\n",idx,count);
    
    rc = ringbell(idx,count);
    
    return 0;
EXIT:
    print_usage(sh_ringbell);
    return rc;
}

static int sh_stopbell (int argc, char **argv, void **result, void *sys_ctx)
{
    stopbell();
    return 0;
}
static int  sh_vol_ctrl(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;    
    int vol = 16;
    int ch;
    if (argc >= 1)) {
        vol = atoi(argv[1]);
    }
    vol_init(vol);
    while(1){
        ch = getb2 ();
        if (ch > 0) {
            printf ("'%c'\n", ch);
            switch (toupper (ch)) {
                case 'U':                    
                    vol = vol_up();
                    printf ("Volume UP %d\n", vol);
                    break;

                case 'D':
                    vol = vol_down();
                    printf ("Volume DOWN %d\n", vol);
                    break;
                case 'S':
                    printf ("Set volume to 16.\n");
                    vol = vol_set(16);
                    break;
                case 'Q':
                    printf ("Exit Volume control.\n");
                    
                    goto EXIT;
                    break;
            }
        }
    }
    

EXIT:
    print_usage(sh_vol_ctrl);
    return rc;
}
