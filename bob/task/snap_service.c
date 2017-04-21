/**
 *  @file   snap_service.c
 *  @brief
 *  $Id: snap_service.c,v 1.2 2014/03/27 07:36:05 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
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
#include <codec_eng/encpic.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "snap_service.h"

//#define SNAP_TWO_FIELD_MODE

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * @func    encpic_get_status
 * @brief   check encpic service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
static int encpic_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = encpic_status(&status);
    if (rc < 0) {
        printf("Failed to get snapshot file status. (%d)\n", rc);
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
 * @func    set_snap_config
 * @brief   setting encpic service of option.
 * @param   epic_opt	option structure
 * @return  none
 */
static int set_snap_config (struct epic_opt_t *epic_opt)
{
	int rc = 0;
	int std = 0;

	/* get current of signal status */
	//std = cvbs_get_standard();
	std = AVIN_CVBS_PAL;

	/* setting option base on liveview of standard */
	if(std == AVIN_CVBS_NTSC) {
		dbg(2, "Snapshot CVBS of NTSC standard\n");
		/* NTSC standard */
		epic_opt->width = 720;
		epic_opt->height = 240;
		epic_opt->standard = AVIN_CVBS_NTSC;
		epic_opt->quality = 100;/*previous 80*/
#ifdef SNAP_TWO_FIELD_MODE
		epic_opt->field_type = TWO_FIELD;
#else
		epic_opt->field_type = ONE_FIELD;
#endif

	} else if(std == AVIN_CVBS_PAL) {
		dbg(2, "Snapshot CVBS of PAL standard\n");
		/* PAL standard */
		epic_opt->width = 720;
		epic_opt->height = 288;
		epic_opt->standard = AVIN_CVBS_PAL;
		epic_opt->quality = 100;/*previous 80*/
		dbg(2, "snap quality is 100\r\n");
#ifdef SNAP_TWO_FIELD_MODE
		epic_opt->field_type = TWO_FIELD;
#else
		epic_opt->field_type = ONE_FIELD;
#endif

	} else {
		ERROR("unknown standard or signal loss!\n");
		return -1;
	}

	return rc;
}

/**
 * @func    snap_cmd_callback
 * @brief   command callback function for encpic service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int snap_cmd_callback (void **arg)
{
#if 0
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check record of status */
	rc = encpic_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF) {
			cmd = CMD_STOP;
			dbg(0, "Capture OK!\n\r");
    	} else/* get system of event */
    	    cmd = get_sys_cmd(ST_ENCPIC);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_ENCPIC);
    	//return cmd;
    }

    /* process event */
    switch(cmd) {
    case CMD_CVD_PAL:
    case CMD_CVD_NTSC:
    	dbg(0, "standard is change!\n");
    	cmd = CMD_STOP;
    	break;
    case CMD_CVD_LOST:
    	dbg(0, "signal is lost!\n");
//    	cmd = CMD_STOP_ERR;
    	cmd = CMD_STOP;
    	//err = lost_cvd;
    	break;
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

    return cmd;
#endif
}

/*
 * snapshot task for encpic service
 */
void snap_srv (struct snapsrv_opt_t *snapsrv_opt)
{
	HLS *hls = NULL;
	int rc = 0;
    int cmd = 0;
    int fno = 0;
    struct epic_opt_t snap_opt;
    char* snap_fname;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    dbg(0, "Start of snapshot task\n");

    memset(&snap_opt, 0, sizeof(struct epic_opt_t));

    if(snapsrv_opt->dev == FATFS) {
		/* check SD card of status */
		rc = chk_sd_status();
		if(rc != SDC_MOUNTED) {
			/* show UI for SD status */
			show_sd_issue(SHOW_THIS_OSD);
			/* show error UI and waiting CMD_QUIT commend or
			 * timeout will auto quit this task */
			err_bk = 1;
			ui_err_timeout_cnt();
			goto EXIT_TASK;/*modified*/
		}
    }

    /* open photo list of NOR/FAT file system */
    hls = openls(PIC_LS, snapsrv_opt->dev);
    if (hls == NULL) {
    	/* show UI for NOR status */
    	if(snapsrv_opt->dev == FATFS)
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

	/* snapshot a file */
    cmd = CMD_REC;

    /* task of main loop */
    while (1) {
		switch (cmd) {
		case CMD_REC:
			/* generate a new file and get it's name */
			snap_fname = get_photo_fname(fno);
			rc = getnewls(hls, &snap_fname, snap_fname, 1/* victim */, FATTR_PROTE_BIT, FATTR_READ_BIT);
			if (rc < 0) {
				ERROR("generate snapshot file name: %s is fail!\n", snap_fname);
				show_file_issue(SHOW_THIS_OSD);
				err_bk = 1;
				ui_err_timeout_cnt();
				goto EXIT_TASK;
			}
			break;
		default:
			dbg(0, "unknow command to record %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
			break;
        }

        dbg(0, "snapshot file name %s\n", snap_fname);

        /* setting REC configure */
        snap_opt.dev = snapsrv_opt->dev;
		rc = set_snap_config(&snap_opt);
        if (rc < 0) {
            show_standard_issue(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

        dbg(0, "Capture...\n\r");

        /* display UI for REC */
		show_snap_icon(SHOW_THIS_OSD);

        /* call encpic service */
		cmd = encpic(snap_fname, &snap_opt, &snap_cmd_callback);

        /* clear REC icon */
        show_snap_icon(CLEAR_THIS_OSD);

        encpic_get_status();	// get snapshot of status

		/* set new photo flag, default of all bits is '1' */
		if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
			/* you can mark for something in the file attribute */
			//mark_fattrib
		}

		/* process a return command from service */
		switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:          /* blank screen and wait event */
        	/* one shot in this sample */
        	//goto WAIT_TASK_CMD;
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
	        rc = get_sys_cmd(ST_ENCPIC);
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
				fno++;			// maybe snapshot again
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
    dbg(0, "End of snapshot task\n");
}
