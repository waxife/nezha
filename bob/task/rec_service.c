/**
 *  @file   rec_service.c
 *  @brief
 *  $Id: rec_service.c,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <command.h>
#include <codec_eng/recfile.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "rec_service.h"
#include "file_manip.h"
#include "srv_evt_cmd.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

static int recfile_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = recfile_status(&status);
    if (rc < 0) {
        printf("Failed to get record file status. (%d)\n", rc);
        goto EXIT;
    }

    printf("state = %d\n", status.state);
    printf("fno = %d\n", status.fno);
    printf("time = %d\n", status.time);
    printf("quality = %d\n", status.quality);

EXIT:
    return rc;
}

static int set_rec_config (struct rec_opt_t *rec_opt)
{
	int rc = 0;
	int std = 0;

	/* get current of signal status */
	std = cvbs_get_standard();

	if(std == AVIN_CVBS_NTSC) {
		dbg(2, "Record CVBS of NTSC standard\n");
		/* NTSC standard */
		rec_opt->width = 720;
		rec_opt->height = 240;
		rec_opt->standard = AVIN_CVBS_NTSC;
        rec_opt->time = 0;
        rec_opt->fno = 0;
        rec_opt->size = 40 * 1024;
        rec_opt->quality = 50;

	} else if(std == AVIN_CVBS_PAL) {
		dbg(2, "Record CVBS of PAL standard\n");
		/* PAL standard */
		rec_opt->width = 720;
		rec_opt->height = 288;
		rec_opt->standard = AVIN_CVBS_PAL;
        rec_opt->time = 0;
        rec_opt->fno = 0;
        rec_opt->size = 40 * 1024;
        rec_opt->quality = 50;

	} else {
		ERROR("unknown standard or signal loss!\n");
		return -1;
	}

	return rc;
}

static int rec_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check record of status */
	rc = recfile_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF) {
			cmd = CMD_STOP;
			dbg(0, "stop record because file size is max!\n\r");
    	} else/* get system of event */
    	    cmd = get_sys_cmd(ST_RECFILE);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_RECFILE);
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
//    case CMD_DOORBELL: 		/* blank screen and wait event */
//    	cmd = CMD_QUIT;		/* this sample, when door bell the exit service and task */
    	break;
    }

    return cmd;
}

/*
 * record task
 */
void rec_srv (struct recsrv_opt_t *recsrv_opt)
{
	HLS *hls = NULL;
	int rc = 0;
    int cmd = 0;
    struct rec_opt_t rec_opt;
    char *rec_fname = NULL;
    char msgbuf[ERR_MSG_LEN];
    int fno = 0;
    int err_bk = 0;

    dbg(0, "Start of record task\n");

    memset(&rec_opt, 0, sizeof(struct rec_opt_t));

    /* check SD card of status */
    rc = chk_sd_status();
    if(rc != SDC_MOUNTED) {
    	/* show UI for SD status */
    	show_sd_issue(SHOW_THIS_OSD);
    	err_bk = 1;
    	ui_err_timeout_cnt();
    	goto WAIT_TASK_CMD;
    }

    /* open video list of FAT file system */
    hls = openls(VIDEO_LS, FATFS);
    if (hls == NULL) {
    	/* show UI for file issue */
    	show_file_issue(SHOW_THIS_OSD);
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

	/* record a file */
	cmd = CMD_REC;

    while (1) {	/* REC loop for repeat record and overwrite if out max size */
		switch (cmd) {
		case CMD_REC:
			/* generate a new file and get it */
			rec_fname = get_video_fname(fno);
			rc = getnewls(hls, &rec_fname, rec_fname, 1/* victim */, FATTR_PROTE_BIT, FATTR_READ_BIT);
			if (rc < 0) {
				ERROR("generate record file name: %s is fail!\n", rec_fname);
				show_file_issue(SHOW_THIS_OSD);
				err_bk = 1;
				ui_err_timeout_cnt();
				goto WAIT_TASK_CMD;
			}
			break;
		default:
			dbg(0, "unknow command to record %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
			break;
        }

        dbg(0, "record file name %s\n", rec_fname);

        /* setting REC configure */
		rc = set_rec_config(&rec_opt);
        if (rc < 0) {
            show_standard_issue(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

        /* display UI for REC */
		show_rec_icon(SHOW_THIS_OSD);

        /* enter codec for record */
        cmd = recfile(rec_fname, &rec_opt, &rec_cmd_callback);

        /* clear REC icon */
        show_rec_icon(CLEAR_THIS_OSD);

        recfile_get_status();

		/* set unread video flag */
		if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
			//rc = setattrls(hls, (int *)rec_fname, 0);
		}

        switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:          /* blank screen and wait event */
        	//goto WAIT_TASK_CMD;
        	goto EXIT_TASK;		// record one file
        case CMD_STOP_ERR:
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
        default:
            dbg(0, "wrong cmd=%d\n", cmd);
            //show_message_ui(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

WAIT_TASK_CMD:
        cmd = 0;
	    /* get sys command to know next step */
	    while(cmd == 0) {
	        rc = get_sys_cmd(ST_RECFILE);
	    	if(rc <= 0)
	    		continue;
	    	if(err_bk) {
	    		if(rc != CMD_QUIT) continue;
	    		else ui_err_clear_cnt();
	    	}

	        switch(rc) {
	        case CMD_STOP:
	        case CMD_QUIT:
	        	goto EXIT_TASK;
			case CMD_REC://CMD_ENC:
				cmd = CMD_REC;//CMD_ENC;
				fno++;	// maybe record again
				break;
	        case CMD_DELETE:
	        	dbg(2, "delete this file!\n");
	        	goto EXIT_TASK;
				break;
//	        case CMD_DOORBELL:
//	        	goto EXIT_TASK;		/* this sample, when door bell the exit task */
			default:
				dbg(0, "unknown cmd=%d\n", rc);
				break;
            }
        } /* end of task wait command loop */
    } /* end of files loop */

EXIT_TASK:
	if(hls)
		rc = closels(hls);

//EXIT:
	clear_massage_ui();
    dbg(0, "End of record task\n");
}
