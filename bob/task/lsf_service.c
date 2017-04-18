/**
 *  @file   lsf_service.c
 *  @brief
 *  $Id: lsf_service.c,v 1.2 2014/04/01 07:35:19 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/13  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codec.h>
#include <codec_eng/decpic.h>
#include <codec_eng/playfile.h>
#include <codec_eng/aumsg.h>
#include <display.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>
#include "eyecatch.h"
#include "command.h"

#ifdef DBG_LEVEL
#undef DBG_LEVEL
#endif
#define DBG_LEVEL   2
#define DBG_HELPER
#include <debug.h>

#include "lsf_manip.h"
#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "lsf_service.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * static variable of lsf task
 */
static int lsf_standard = 0;
static int aud_dura = 0;
static int bar_cnt = 0;
static int bar_time = 0;

/**
 * @func    auplay_bar_update
 * @brief   draw statistic bar OSD when time update
 * @param   clear	1 to clear this OSD
 * 			times	millisecond
 * @return  none
 */
#define AUPLAY_BAR_STEP	32	// set one step is 0.5s
static void auplay_bar_update2 (int clear, int times)
{
	if(!clear) {
		// show audio statistic bar
		if(bar_time == 0) {
			bar_time = 1;
			bar_cnt += ((AUPLAY_BAR_STEP*1024)/aud_dura);
            if(bar_cnt > AUPLAY_BAR_STEP || bar_cnt < 0)
                bar_cnt = AUPLAY_BAR_STEP;
			show_audmsg_rec_time2(bar_cnt);
		} else
		if((times - bar_time) >= 1000) {
			if(bar_cnt < AUPLAY_BAR_STEP) {
				bar_cnt += ((AUPLAY_BAR_STEP*1024)/aud_dura);
				if(bar_cnt > AUPLAY_BAR_STEP || bar_cnt < 0)
					bar_cnt = AUPLAY_BAR_STEP;
				show_audmsg_rec_time2(bar_cnt);
			}
			bar_time = times;
		}
	} else {
		// clear audio statistic bar
		bar_cnt = 0;
		bar_time = 0;
		show_audmsg_rec_time2(0);
	}
}

static void disp_re_lsf (void)
{
	show_ui_enable();
    display_set_play_lmod(lsf_standard);
}

/**
 * @func    display_lsf
 * @brief   check file of standard and configuring display.
 * @param   media		media
 * 			fname		filename
 * @return  0 for successful, minus if any error occurred
 */
static int display_lsf (int media, int fs, char* fname)
{
	int rc = 0;
	int std = 0;
	int height = 0;
	struct pic_property prop;
	struct av_property prop2;

	if(media == PIC_LS) {
		/* get file of property */
		memset(&prop, 0, sizeof(struct pic_property));
		rc = pic_get_info(fs, fname, &prop);
		if (rc < 0) {
			ERROR("pic_get_info(), rc = %d\n", rc);
			return -1;
		}
		height = prop.height;
	} else if (media == VIDEO_LS) {
	    /* get file of property */
		memset(&prop2, 0, sizeof(struct av_property));
	    rc = playback_get_property(fname, &prop2);
	    if (rc < 0) {
	        ERROR("playback_get_video_property(), rc = %d\n", rc);
	        return -1;
	    }
	    height = prop2.v_info.height;
	} else {
		ERROR("unknown media!\n");
		return -1;
	}

    /* check file of standard base on frame of height */
    if(height == 240) {
    	dbg(2, "file of standard is NTSC\n");
    	std = AVIN_CVBS_NTSC;
    } else if(height == 288) {
    	dbg(2, "file of standard is PAL\n");
    	std = AVIN_CVBS_PAL;
    } else {
		ERROR("unknown standard or signal loss!\n");
		return -1;
	}

	ect_black_screen(FD_FAST,FD_NONE);
	display_set_play_full(std);
	show_ui_disable();

	lsf_standard = std;

	return rc;
}

/**
 * @func    auplaylsf_cmd_callback
 * @brief   command callback function for decpic service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int auplaylsf_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

	/* check decode audio of status */
	rc = decau_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
//    	else if(status.state == STOP_EOF)
//			cmd = CMD_STOP;	// no repeat // cmd = CMD_NEXT;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_LSF);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_LSF);
    }

    /* process event */
    switch(cmd) {
    case CMD_DOORBELL:
    	//cmd = CMD_DOORBELL;
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

    auplay_bar_update2(SHOW_THIS_OSD, status.time);

	return cmd;
}

/* request picture view service */
static int req_dec_aumsg(HLS *hls, int fs, char* fname, int fattrib)
{
	int rc = 0;
	int cmd = 0;
	struct dau_opt_t dau_opt;
	struct aumsg_property prop;

	dbg(2, "Start of audio play task\n");

    memset(&dau_opt, 0, sizeof(struct dau_opt_t));
    dau_opt.dev = fs;
	memset(&prop, 0, sizeof(struct aumsg_property));
	aud_dura = 0;
	rc = aumsg_get_info(fs, fname, &prop);
	if (rc < 0) {
		ERROR("aumsg_get_info(), rc = %d\n", rc);
		goto EXIT;
	}
	aud_dura = prop.duration;

    dbg(2, "audio play file name %s\n", fname);

    /* call decau service */
	cmd = dec_aumsg(fname, &dau_opt, &auplaylsf_cmd_callback);

    /* mark read in the file attribute */
    if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
    	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
			mark_fattrib(hls, fname, fattrib , FATTR_READ_BIT);
    	}
    }

    /* clear bar */
    auplay_bar_update2(1, 0);

    return cmd;

EXIT:
	return rc;
}

/**
 * @func    playlsf_cmd_callback
 * @brief   command callback function for playfile service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int playlsf_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check playback of status */
	rc = play_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
//    	else if(status.state == STOP_EOF)
//			cmd = CMD_NEXT;
//    	else if(status.state == STOP_SOF)
//			cmd = CMD_PREVIOUS;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_LSF);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_LSF);
    }

    /* process event */
    switch(cmd) {
    case CMD_DOORBELL:
    	//cmd = CMD_DOORBELL;
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

	return cmd;
}

/* request play file service */
static int req_playfile(HLS *hls, int fs, char* fname, int fattrib)
{
	int rc = 0;
	int cmd = 0;
	struct play_opt_t play_opt;

	dbg(2, "Start of playback task\n");

    memset(&play_opt, 0, sizeof(struct play_opt_t));

	/* playfile service of option */
	play_opt.sync = 1;		// 0: not sync, 1: sync with fno
	play_opt.stop = 2;		// 0: quit, 1: stay first frame, 2: stay last frame

    dbg(2, "video file name %s\n", fname);

    /* setting picture view mode with standard and display */
    rc = display_lsf(VIDEO_LS, fs, fname);
    if (rc < 0) {
        //show_standard_issue(SHOW_THIS_OSD);
        goto EXIT;
    }

    /* call playfile service */
	cmd = playfile2(fname, &play_opt, &playlsf_cmd_callback);

    /* mark a read flag in file attribute */
    if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
    	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
    		mark_fattrib(hls, fname, fattrib , FATTR_READ_BIT);
    	}
    }

    disp_re_lsf();

	return cmd;

EXIT:
	return rc;
}

/**
 * @func    pviewlsf_cmd_callback
 * @brief   command callback function for decpic service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int pviewlsf_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check decode picture of status */
	rc = decpic_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_LSF);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_LSF);
    }

    /* process event */
    switch(cmd) {
    case CMD_DOORBELL:
    	//cmd = CMD_DOORBELL;
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

	return cmd;
}

/* request picture view service */
static int req_decpic(HLS *hls, int fs, char* fname, int fattrib)
{
	int rc = 0;
	int cmd = 0;
	struct dpic_opt_t dpic_opt;

	dbg(2, "Start of picture view task\n");

    memset(&dpic_opt, 0, sizeof(struct dpic_opt_t));

    dpic_opt.dev = fs;
	dpic_opt.duration = 0;
	dpic_opt.effect = 0;

    dbg(2, "photo file name %s\n", fname);

    /* setting picture view mode with standard and display */
    rc = display_lsf(PIC_LS, dpic_opt.dev, fname);
    if (rc < 0) {
        //show_standard_issue(SHOW_THIS_OSD);
        goto EXIT;
    }

    /* call decpic service */
    cmd = decpic(fname, &dpic_opt, &pviewlsf_cmd_callback);

    /* mark read in the file attribute */
    if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
    	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
			mark_fattrib(hls, fname, fattrib , FATTR_READ_BIT);
    	}
    }

    disp_re_lsf();

	return cmd;

EXIT:
	return rc;
}

/**
 * @func    get_curr_lsf
 * @brief   get filename and attribute at the current position of list file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_curr_lsf (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_curr_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
	}

	return rc;
}

/*
 * list file task
 */
void lsf_srv (struct lsfsrv_opt_t *lsfsrv_opt)
{
    int fattrib;
	int rc = 0;
    int cmd = 0;
    char* lsf_fname = NULL;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    finder_t f;
    extern char *_filesystem_name(int);
    extern char *_media_name(int);

    dbg(2, "Start of picture view lsf task\n");

    memset(&f, 0, sizeof(f));
    f.fs = lsfsrv_opt->dev;
    f.media = lsfsrv_opt->media;
    f.order = 1; /* descending */

    if(lsfsrv_opt->dev == FATFS) {
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

//RELOAD:
    rc = open_list(&f);
    if (rc < 0) {
        if(lsfsrv_opt->dev == FATFS)
    		show_file_issue(SHOW_THIS_OSD);
    	else
    		show_nor_issue(SHOW_THIS_OSD);
        ERROR("Can't open ls fs %s media %s rc = %d\n", _filesystem_name(f.fs),
            _media_name(f.media), rc);
        goto EXIT_TASK;
    }
    //print_page(&f, 0);
    findfirstunread(&f);
    goto WAIT_TASK_CMD;

	/* task of main loop */
	while (1) {
		switch (cmd) {
        case CMD_PLAY:
        	rc = get_curr_lsf(f.hls, &lsf_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;           
        default:
			dbg(0, "unknow command to picture view %d\n", cmd);
			//show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
        }

        dbg(2, "lsf file name %s\n", lsf_fname);

        switch (f.media) {
        case VIDEO_LS:
        	req_playfile(f.hls, f.fs, lsf_fname, fattrib);
        	break;
        case PIC_LS:
        	req_decpic(f.hls, f.fs, lsf_fname, fattrib);
        	break;
        case AUMSG_LS:
        	req_dec_aumsg(f.hls, f.fs, lsf_fname, fattrib);
        	break;
        default:
        	//show_message_ui(SHOW_THIS_OSD);
        	ERROR("unknown file system!\n");
        	goto WAIT_TASK_CMD;
        	break;

        }

        /* process a return command from service */
        switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:          /* blank screen and wait event */
        	goto WAIT_TASK_CMD;
        case CMD_STOP_ERR:		/* if any error occurred */
        	rc = get_err(msgbuf);
			if (rc) {
				ERROR("error %d, message : %s\n", rc, msgbuf);
			}
        	ERROR ("service()\n");
			err_bk = 2;
        	goto WAIT_TASK_CMD;
        case CMD_DOORBELL:		/* quit task if door bell in this sample */
        	goto EXIT_TASK;
        default:
            //dbg(0, "wrong cmd=%d\n", cmd);
            //show_message_ui(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

WAIT_TASK_CMD:
        cmd = 0;
        /* waiting command loop */
	    while(cmd == 0) {
	    	/* get sys command to know next step */
	    	rc = get_sys_cmd(ST_LSF);
	    	if(rc <= 0)
	    		continue;
	    	/* waiting quit command or timeout */
	    	if(err_bk) {
	    		if(rc != CMD_QUIT) continue;
	    		else ui_err_clear_cnt();
	    	} else if(err_bk == 2) {
	    		if(rc == CMD_NEXT || rc == CMD_PREVIOUS) {
	    			err_bk = 0;
	    			clear_massage_ui();
	    		}
	    	}

	        switch (rc) {
	        case CMD_QUIT:		/* quit task */
	        	goto EXIT_TASK;
			case CMD_PLAY:		/* review again */
				cmd = CMD_PLAY;
				break;
	        case CMD_NEXT:
	            rc = next_pos(&f);
	            if (rc < 0) {
	            	ERROR ("next_pos(), rc=%d\n", rc);
	            	//show_message_ui(SHOW_THIS_OSD);
	            }
	            break;
	        case CMD_PREVIOUS:
	            rc = prev_pos(&f);
	            if (rc < 0){
	            	ERROR ("next_pos(), rc=%d\n", rc);
	            	//show_message_ui(SHOW_THIS_OSD);
	            }
				break;
			case CMD_USER_COPY:	/* copy from NOR to SD card */
				if(lsfsrv_opt->dev == NORFS) {
					show_copying_ui(SHOW_THIS_OSD);
					if(lsfsrv_opt->media == PIC_LS)
						rc = pic_backup2sd(PIC_LS, lsf_fname);
					else if(lsfsrv_opt->media == AUMSG_LS)
						rc = aumsg_backup2sd(AUMSG_LS, lsf_fname);
					//show_copying_ui(CLEAR_THIS_OSD);
					if(rc < 0) {
						ERROR ("copyls(), rc=%d\n", rc);
						//show_file_issue(SHOW_THIS_OSD);
					}
				}
				break;
	        case CMD_DELETE:
                rc = deletecurrent(&f);
	        	if(rc < 0) { 		/* unknown error */
	        		ERROR ("deletels(), rc=%d\n", rc);
	            	//show_message_ui(SHOW_THIS_OSD);
	        	}
	        	break;
			default:
				dbg(0, "unknown cmd=%d\n", rc);
				break;
            }
        } /* end of task wait command loop */
    } /* end of task main loop */

//REOPEN:
//    closels(f.hls);
//    f.hls = NULL;
//    goto RELOAD;

EXIT_TASK:
	/* close file list */
	if(f.hls){
		closels(f.hls);
    }
    lsf_clear_page();

//EXIT:
	clear_massage_ui();
    dbg(2, "End of list file task\n");
}
