/**
 *  @file   md_service.c
 *  @brief
 *  $Id: md_service.c,v 1.2 2014/03/27 07:36:05 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2014/3/10  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codec.h>
#include <encpic.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "snap_service.h"

#include "mdvid.h"
#include "md_service.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * @func    show_photo_header
 * @brief   draw header(file number) OSD
 * @param   no			number of file
 * 			no_tal		total number of file
 * @return  none
 */
#define PVIEW_HEADER_LEN		9
static void show_photo_header (int no)
{
	char header_str[PVIEW_HEADER_LEN + 1] = { '\0' };

	if(no != 0) {
		// show photo file of header
		snprintf(header_str, PVIEW_HEADER_LEN, "%04d     ", no);
		show_file_header(header_str);
	} else {
		// clear photo file of header
		show_file_header(NULL);
	}
}

/**
 * @func    set_md_snap_config
 * @brief   setting encpic service of option.
 * @param   epic_opt	option structure
 * @return  none
 */
static int set_md_snap_config (struct epic_opt_t *epic_opt, struct md_opt_t *md_opt)
{
	int rc = 0;
	int std = 0;

	/* get current of signal status */
	std = cvbs_get_standard();

	/* setting option base on liveview of standard */
	if(std == AVIN_CVBS_NTSC) {
		dbg(3, "Snapshot CVBS of NTSC standard\n");
		/* NTSC standard */
		epic_opt->width = 720;
		epic_opt->height = 240;
		epic_opt->standard = AVIN_CVBS_NTSC;
		epic_opt->quality = 80;
		epic_opt->field_type = ONE_FIELD;

		md_opt->width = 720;
		md_opt->height = 240;
	} else if(std == AVIN_CVBS_PAL) {
		dbg(3, "Snapshot CVBS of PAL standard\n");
		/* PAL standard */
		epic_opt->width = 720;
		epic_opt->height = 288;
		epic_opt->standard = AVIN_CVBS_PAL;
		epic_opt->quality = 80;
		epic_opt->field_type = ONE_FIELD;

		md_opt->width = 720;
		md_opt->height = 288;
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

/**
 * @func    md_cmd_callback
 * @brief   command callback function for md service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int md_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check record of status */
	rc = md_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_MDVID);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_MDVID);
    	//return cmd;
    }

    /* process event */
    switch(cmd) {
    case CMD_CVD_PAL:
    case CMD_CVD_NTSC:
    	dbg(0, "standard is change!\n");
    	cmd = CMD_STOP_ERR;
    	break;
    case CMD_CVD_LOST:
    	dbg(0, "signal is lost!\n");
    	cmd = CMD_STOP_ERR;
    	break;
    case CMD_DOORBELL:
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

    return cmd;
}

/*
 * task for motion detection service
 */
void md_srv (struct md_opt_t *md_opt)
{
	HLS *hls = NULL;
	int rc = 0;
    int cmd = 0;
    int fno = 0;
    int md_pic = 0;
    struct epic_opt_t snap_opt;
    struct sysc_status_t status;
    char* snap_fname;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;
    unsigned char userinfo[4];

    dbg(0, "Start of MD task\n");

    memset(&snap_opt, 0, sizeof(struct epic_opt_t));
    snap_opt.dev = md_opt->dev;

    if(snap_opt.dev == FATFS) {
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

    /* open photo list of NOR/FAT file system */
    hls = openls(PIC_LS, snap_opt.dev);
    if (hls == NULL) {
    	/* show UI for NOR status */
    	if(snap_opt.dev == FATFS)
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

	/* image of motion detection */
    cmd = CMD_MD_VID;

    /* task of main loop */
    while (1) {
		switch (cmd) {
		case CMD_MD_VID:
	        /* setting MD/REC configure */
			rc = set_md_snap_config(&snap_opt, md_opt);
	        if (rc < 0) {
	            show_standard_issue(SHOW_THIS_OSD);
	            goto WAIT_TASK_CMD;
	        }
	        /* clear REC icon */
	        show_snap_icon(CLEAR_THIS_OSD);
	        cmd = mdvid(md_opt, &md_cmd_callback);
			if(cmd == CMD_STOP) {
				md_status(&status);
	        	if(status.state == STOP_MD) {
					dbg(3, "Motion detect!\n\r");
					userinfo[0] = md_opt->mt_x;
					userinfo[1] = md_opt->mt_y;
					userinfo[2] = md_opt->mt_w;
					userinfo[3] = md_opt->mt_h;
					snap_opt.user_info = userinfo;
					snap_opt.user_info_len = 4;
					dbg(3, " md_opt->mt_x = %d \n", md_opt->mt_x);
					dbg(3, " md_opt->mt_y = %d \n", md_opt->mt_y);
					dbg(3, " md_opt->mt_w = %d \n", md_opt->mt_w);
					dbg(3, " md_opt->mt_h = %d \n", md_opt->mt_h);
					cmd = CMD_REC;
					continue;
				}
			}
			break;
		case CMD_REC:
			/* generate a new file and get it's name */
			snap_fname = get_photo_fname(fno);
			rc = getnewls(hls, &snap_fname, snap_fname, 1/* victim */, FATTR_PROTE_BIT, FATTR_READ_BIT);
			if (rc < 0) {
				ERROR("generate snapshot file name: %s is fail!\n", snap_fname);
				show_file_issue(SHOW_THIS_OSD);
				err_bk = 1;
				ui_err_timeout_cnt();
				goto WAIT_TASK_CMD;
			}
	        dbg(3, "snapshot file name %s\n", snap_fname);
	        dbg(3, "Capture...\n\r");
	        /* call encpic service */
			cmd = encpic(snap_fname, &snap_opt, &snap_cmd_callback);
	        /* display UI for REC */
			show_snap_icon(SHOW_THIS_OSD);	// show icon after encpic
			/* set new photo flag, default of all bits is '1' */
			if (cmd == CMD_STOP) {
				encpic_status(&status);
				/* you can mark for something in the file attribute */
				if(status.state != STOP_USER) {
					//mark_fattrib
					/* one shot and MD */
					cmd = CMD_MD_VID;
					md_pic++;
					show_photo_header(md_pic);
					continue;
				}
			}

			break;
		default:
			dbg(0, "unknow command to record %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
			break;
        }

		/* process a return command from service */
		switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:
        	goto WAIT_TASK_CMD;
        	break;
        case CMD_STOP_ERR:		/* if any error occurred */
        	show_message_ui(SHOW_THIS_OSD);
            rc = get_err(msgbuf);
            if (rc) {
                ERROR("error %d, message : %s\n", rc, msgbuf);
            }
			err_bk = 1;
			ui_err_timeout_cnt();
        	goto WAIT_TASK_CMD;
        	break;
        case CMD_DOORBELL:
        	goto EXIT_TASK;		/* quit task if door bell in this sample */
        default:
            dbg(0, "wrong cmd=%d\n", cmd);
            goto WAIT_TASK_CMD;
        }

WAIT_TASK_CMD:
        cmd = 0;
        /* waiting command loop */
	    while(cmd == 0) {
	    	/* get sys command to know next step */
	        rc = get_sys_cmd(ST_MDVID);
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
	show_photo_header(0);
	clear_massage_ui();
    dbg(0, "End of MD task\n");
}
