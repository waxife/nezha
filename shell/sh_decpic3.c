/**
 *  @file   sh_decpic3.c
 *  @brief  picture decode
 *  $Id: sh_decpic3.c,v 1.7 2014/08/04 07:02:00 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 *
 *  @date   2014/07/22  hugo       New file.
 *
 */
#include <config.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <callback.h>
#include <mconfig.h>
#include <command.h>
#include <display.h>
#include <serial.h>
#include <fat32.h>
#include <decpic.h>
#include <keymap.h>
#include <debug.h>

command_init (sh_decpic3, "decpic3", "decpic3 <name> [-t <timeout>] [-f <field option>]");


static const KeyString ksCmd[] = {
    MAKE_KEY_STRING (CMD_QUIT),
    MAKE_KEY_STRING (CMD_STOP),
    MAKE_KEY_STRING (CMD_NEXT),
    MAKE_KEY_STRING (CMD_PREVIOUS),
    MAKE_KEY_STRING (CMD_STOP_ERR),
};
#define ksCmdNum    (sizeof(ksCmd)/sizeof(ksCmd[0]))

static int
decpic3_cmd_callback (void **arg)
{
    int ch = 0;
    int cmd = 0;

	ch = getb2 ();
    if (ch > 0) {
        switch (toupper (ch)) {
            case 'Q':
                cmd = CMD_QUIT;
                break;
            case 'S':
                cmd = CMD_STOP;
                break;
            case 'N':
                cmd = CMD_NEXT;
                break;
            case 'P':
                cmd = CMD_PREVIOUS;
                break;
			case 'E':
                cmd = CMD_STOP_ERR;
                break;
        }
    }

    if (cmd > 0) {
        printf("%s\n", key2str (ksCmd, ksCmdNum, cmd));
    }

    return cmd;
}

static int
sh_decpic3 (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
    int rc;
	
	struct dpic_opt_t opt;
	char *ch;
	char msgbuf[ERR_MSG_LEN];
	
	if(argc < 2)
		goto EXIT;

	memset (&opt, 0, sizeof(struct dpic_opt_t));
	opt.dev = FATFS;
	
	fname = argv[1];
	ch = strchr (fname, '.');
	if (ch == NULL || (strncmp (ch + 1, "jpg", 3) != 0)) {
		goto EXIT;
	}
	
    if (argc > 2) {
        int idx = 2;
        for (idx = 2; idx < argc; idx += 2) {
            if (strcmp (argv[idx], "-t") == 0 && (idx+1 < argc)) {
                opt.duration = atoi (argv[idx+1]);
            } else
            if (strcmp (argv[idx], "-f") == 0 && (idx+1 < argc)) {
                opt.fields = atoi (argv[idx+1]);
            }
        }
    }

    printf ("[Option]\n"
            "  dev: %d\n"
            "  effect: %d\n"
            "  duration: %d\n"
            "  threshold: %d\n"
            "  fields: %d\n",
            opt.dev, opt.effect, opt.duration, opt.threshold, opt.fields);
	
    /* picture property */
	struct pic_property property;
    rc = decpic3_pic_property (opt.dev, fname, &property);
    if (rc < 0) {
        ERROR ("pic_get_info(), rc=%d\n", rc);
        goto EXIT;
    }

    printf ("[picture]\n"
            "  resolution: %d x %d\n"
            "  time: %d/%d/%d %02d:%02d:%02d\n"
            "  length: %d\n"
            "  info: \"%s\"\n\n",
            property.width, property.height,
            property.year, property.mon, property.mday,
            property.hour, property.min, property.sec,
            property.length, property.user_info);

    /* set display automatically */
    if (property.height == 288) {
        display_set_play_full (VIDEO_STD_PAL);
    } else {
        display_set_play_full (VIDEO_STD_NTSC);
    }

    /* starting decode */
	rc = decpic3 (fname, &opt, decpic3_cmd_callback);
    printf ("decpic return 0x%x\n", rc);
    if (rc == CMD_STOP_ERR) {
        rc = get_err (msgbuf);
        printf("error code: %d, message: %s\n", rc, msgbuf);
    }
    
	if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:
	print_usage(sh_decpic3);
    return -1;
}
