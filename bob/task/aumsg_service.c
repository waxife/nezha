/**
 *  @file   aumsg_service.c
 *  @brief
 *  $Id: aumsg_service.c,v 1.3 2014/05/29 05:25:08 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codec.h>
#include <codec_eng/aumsg.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "aumsg_service.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * static variable of audio massage task for store statistic bar
 */
static int aumsg_bar_cnt = 0;
static int aumsg_bar_time = 0;

/**
 * @func    aumsg_bar_update
 * @brief   draw statistic bar OSD when time update
 * @param   clear	1 to clear this OSD
 * 			times	millisecond
 * @return  none
 */
#define AUMSG_BAR_STEP	32	// set one step is 0.5s
static void aumsg_bar_update (int clear, int times)
{
	if(!clear) {
		// show audio statistic bar
		if(aumsg_bar_time == 0) {
			aumsg_bar_time = 1;
			aumsg_bar_cnt = 1;
			show_audmsg_rec_time(aumsg_bar_cnt);
		} else
		if((times - aumsg_bar_time) >= 500) {
			aumsg_bar_cnt++;
			if(aumsg_bar_cnt <= AUMSG_BAR_STEP) {
				show_audmsg_rec_time(aumsg_bar_cnt);
			}
			aumsg_bar_time = times;
		}
	} else {
		// clear audio statistic bar
		aumsg_bar_cnt = 0;
		aumsg_bar_time = 0;
		show_audmsg_rec_time(0);
	}
}

/**
 * @func    encau_get_status
 * @brief   check encau service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
static int encau_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = encau_status(&status);
    if (rc < 0) {
        printf("Failed to get enc audio file status. (%d)\n", rc);
        goto EXIT;
    }

    printf("state = %d\n", status.state);
    printf("fno = %d\n", status.fno);
    printf("time = %d\n", status.time);
    printf("quality = %d\n", status.quality);

EXIT:
    return rc;
}

/**
 * @func    set_aumsg_config
 * @brief   setting encau service of option.
 * @param   eau_opt_t	option structure
 * 			media		media option
 * @return  none
 */
static void set_aumsg_config (struct eau_opt_t *eau_opt, FS dev, MEDIA media)
{
	/* audio massage service of option */
	/*
	 * num    /     format     /  data rate    / max. time
	 * 0      /  G.726-16Kbits /  2 Kbytes/s   / 30 sec
	 * 1      /  PCM           /  16 Kbytes/s  / 4 sec
	 * 2      /  U-Law         /  8 Kbytes/s   / 8 sec
	 * 3      /  A-Law         /  8 Kbytes/s   / 8 sec
	 * 4      /  G.726-32Kbits /  4 Kbytes/s   / 15 sec
	 */
	eau_opt->dev = dev;
	eau_opt->media = media;	// can select MEDIA_USER0 or MEDIA_USER1 if created
	eau_opt->format = 4;	// recommend
}

/**
 * @func    aumsg_cmd_callback
 * @brief   command callback function for encau service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int aumsg_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check encode of status */
	rc = encau_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF) {
			cmd = CMD_STOP;
			dbg(0, "Capture OK!\n\r");
    	} else/* get system of event */
    	    cmd = get_sys_cmd(ST_ENCAU);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_ENCAU);
    	//return cmd;
    }

    /* process event */
    switch(cmd) {
    case CMD_DELETE:
    	dbg(0, "delete this file!\n");
//    	cmd = CMD_DELETE;
		cmd = CMD_STOP;
    	break;
    case CMD_DOORBELL:
    	//cmd = CMD_DOORBELL;
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

    aumsg_bar_update(SHOW_THIS_OSD, status.time);

    return cmd;
}

/*
 * audio massage task for encau service
 */
void aumsg_srv (struct aumsgsrv_opt_t *aumsgsrv_opt)
{
	HLS *hls = NULL;
	int rc = 0;
    int cmd = 0;
    int fno = 0;
    struct eau_opt_t aumsg_opt;
    char* aumsg_fname;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    dbg(0, "Start of audio massage task\n");

    memset(&aumsg_opt, 0, sizeof(struct eau_opt_t));
    /* setting encode audio configure */
	set_aumsg_config(&aumsg_opt, aumsgsrv_opt->dev, MEDIA_AUMSG);

    if(aumsg_opt.dev == FATFS) {
		/* check SD card of status */
		rc = chk_sd_status();
		if(rc != SDC_MOUNTED) {
			/* show UI for SD status */
			show_sd_issue(SHOW_THIS_OSD);
			/* show error UI and waiting CMD_QUIT commend or
			 * timeout will auto quit this task */
			err_bk = 1;
			ui_err_timeout_cnt();
			goto WAIT_TASK_CMD;
		}
	}

    /* open audio massage list of NOR/FAT file system */
    hls = openls(aumsg_opt.media, aumsg_opt.dev);
    if (hls == NULL) {
    	/* show UI for NOR status */
    	if(aumsg_opt.dev == FATFS)
    		show_file_issue(SHOW_THIS_OSD);
    	else
    		show_nor_issue(SHOW_THIS_OSD);
    	/* show error UI and waiting CMD_QUIT commend or
    	 * timeout will auto quit this task */
    	err_bk = 1;
    	ui_err_timeout_cnt();
    	goto WAIT_TASK_CMD;
    }

#ifdef NUMBER_FILE_NAME_TYPE
    fno = get_latest_fno(hls);
    if(fno < 0) {
    	/* show UI for file issue */
		show_file_issue(SHOW_THIS_OSD);
		goto WAIT_TASK_CMD;
    }
#endif

	/* encode a audio file */
    cmd = CMD_REC;//CMD_ENC;

    /* task of main loop */
    while (1) {
		switch (cmd) {
		case CMD_REC://CMD_ENC:
			/* generate a new file and get it's name */
			aumsg_fname = get_audio_fname(fno);
			rc = getnewls(hls, &aumsg_fname, aumsg_fname, 1/* victim */, FATTR_PROTE_BIT, FATTR_READ_BIT);
			if (rc < 0) {
				ERROR("generate audio massage file name: %s is fail!\n", aumsg_fname);
				show_file_issue(SHOW_THIS_OSD);
				err_bk = 1;
				ui_err_timeout_cnt();
				goto WAIT_TASK_CMD;
			}
			break;
		default:
			dbg(0, "unknown command to record %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
			break;
        }

        dbg(0, "audio massage file name %s\n", aumsg_fname);
        dbg(0, "Capture...\n\r");

        /* display UI for REC */
        show_aumsg_icon(SHOW_THIS_OSD);
        aumsg_bar_update(CLEAR_THIS_OSD, 0);

        /* call encau service */
        cmd = enc_aumsg(aumsg_fname, &aumsg_opt, &aumsg_cmd_callback);

        /* clear REC icon */
        show_aumsg_icon(CLEAR_THIS_OSD);
        aumsg_bar_update(CLEAR_THIS_OSD, 0);

        encau_get_status();	// get encode audio of status

        /* set new audio flag, default of all bits is '1' */
		if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
			/* you can mark for something in the file attribute */
			//mark_fattrib
		}

		/* process a return command from service */
		switch(cmd) {
        case CMD_QUIT:  		/* quit task */
        case CMD_STOP:
	        goto EXIT_TASK;
        case CMD_STOP_ERR:		/* if any error occurred */
        	show_message_ui(SHOW_THIS_OSD);
            rc = get_err(msgbuf);
            if (rc) {
                ERROR("error %d, message : %s\n", rc, msgbuf);
            }
			err_bk = 1;
			ui_err_timeout_cnt();
        	goto WAIT_TASK_CMD;
        case CMD_DELETE:
        	dbg(2, "delete this file!\n");
        	cmd = CMD_STOP;
			goto WAIT_TASK_CMD;
        	break;
        case CMD_DOORBELL:
        	goto EXIT_TASK;		/* quit task if door bell in this sample */
        default:
            dbg(0, "wrong cmd=%d\n", cmd);
            //show_message_ui(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

WAIT_TASK_CMD:
        cmd = 0;
        /* waiting command loop */
		while(cmd == 0) {
			/* get sys command to know next step */
	        rc = get_sys_cmd(ST_ENCAU);
	    	if(rc <= 0)
	    		continue;
	    	/* waiting quit command or timeout */
	    	if(err_bk) {
	    		if(rc != CMD_QUIT) continue;
	    		else ui_err_clear_cnt();
	    	}

	        switch (rc) {
	        case CMD_STOP:		/* quit task */
	        case CMD_QUIT:
	        	goto EXIT_TASK;
			case CMD_REC:
				cmd = CMD_REC;
				fno++;			// maybe record audio again
				break;
	        case CMD_DELETE:
	        	dbg(2, "delete this file!\n");
	        	goto EXIT_TASK;
				break;
	        case CMD_DOORBELL:
	        	goto EXIT_TASK;	/* quit task if door bell in this sample */
			default:
				dbg(0, "unknown cmd=%d\n", rc);
				break;
            }
        } /* end of task wait command loop */
    } /* end of task main loop */

EXIT_TASK:
	/* close file list */
	if(hls)
		rc = closels(hls);

//EXIT:
	clear_massage_ui();
	dbg(0, "End of audio massage task\n");
}
