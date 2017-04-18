/**
 *  @file   play_service.c
 *  @brief
 *  $Id: play_service.c,v 1.3 2014/03/27 10:44:59 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/17  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <codec.h>
#include "codec_eng/playfile.h"
#include <display.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "play_service.h"
#include "eyecatch.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * static variable of playback task for store file numbers
 */
static int video_fno = 0;
static int video_nread = 0;
static int video_nls = 0;

/**
 * @func    play_header_update
 * @brief   draw header(frame time) OSD when frame time update
 * @param   none
 * @return  none
 * @note	1. Get current frame time value when normal play.
 * 			2. Only draw OSD when get difference frame time.
 * 			3. Time OSD format is "YYYY/MM/DD-hh:dd:ss".
 * 			4. Draw OSD in vertical blanking.
 */
static void play_header_update (void)
{
	time_t frm_time = 0;

	playback_get_time(&frm_time);

	if(frm_time != NULL)
		show_frm_update(frm_time);
}

/**
 * @func    show_play_header
 * @brief   draw header(file number) OSD
 * @param   no			number of file
 * 			no_tal		total number of file
 * @return  none
 */
#define PLAY_HEADER_LEN		9
static void show_play_header (int no, int no_tal)
{
	char header_str[PLAY_HEADER_LEN + 1] = { '\0' };

	if(no_tal != 0) {
		// show video file of header
		snprintf(header_str, PLAY_HEADER_LEN, "%04d/%04d", no, no_tal);
		show_file_header(header_str);
	} else {
		// clear video file of header
		show_file_header(NULL);
	}
}

/**
 * @func    get_total_video
 * @brief   scan and get a number of video file in file list
 * @param   hls			file list of handle
 * 			nls			total number of file
 * 			unread		number of unread file
 * @return  0 for successful, -1 if any error occurred, -2 if no video file
 */
static int get_total_video (void* hls, int* nls, int* nread)
{
	int rc = 0;

	video_fno = 0;

	rc = get_total_file(hls, nls, nread);
	if (rc < 0) {
		dbg(0, "file issue!\n");
		return -1;
	}
	if (*nls <= 0) {
		dbg(0, "no video file!\n");
		return -2;
	} else {
		INFO("total video: %d\n", *nls);
		INFO("total new video: %d\n", *nread);
	}

    video_fno = *nls;

	return rc;
}

/**
 * @func    get_curr_video
 * @brief   get filename and attribute at the current position of video file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_curr_video (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_curr_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
	}

	return rc;
}

/**
 * @func    get_next_video
 * @brief   get filename and attribute at the next position of video file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_next_video (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_next_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		video_fno = 0;
	} else if (rc == 1) {	/* in head now */
		video_fno = 1;
		rc = 0;
	} else {
		video_fno = (video_fno < video_nls)? (video_fno+1) : video_nls;
	}

	return rc;
}

/**
 * @func    get_prev_video
 * @brief   get filename and attribute at the previous position of video file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_prev_video (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_prev_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		video_fno = 0;
	} else if (rc == 1) {	/* in tail now */
		video_fno = video_nls;
		rc = 0;
	} else {
		video_fno = (video_fno > 1)? (video_fno-1) : 1;
	}

	return rc;
}

/**
 * @func    playfile_get_status
 * @brief   check playfile service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
static int __attribute__((unused))
playfile_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = play_status(&status);
    if (rc < 0) {
        printf("Failed to get play file status. (%d)\n", rc);
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
 * @func    get_video_total
 * @brief   get total number of video file from task.
 * @param   none
 * @return  total number
 */
int get_video_total (void)
{
	return video_nls;
}

/**
 * @func    get_video_nread
 * @brief   get unread number of video file from task.
 * @param   none
 * @return  unread number
 */
int get_video_nread (void)
{
	return video_nread;
}

/**
 * @func    get_play_srv_state
 * @brief   get playfile service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int get_play_srv_state (int *state)
{
	int rc = 0;
	struct sysc_status_t status;
	*state = 0;

    rc = play_status(&status);
    if (rc < 0) {
        goto EXIT;
    }
    *state = status.state;

EXIT:
	return rc;
}

/**
 * @func    get_play_srv_std
 * @brief   get video file of standard.
 * @param   none
 * @return  video standard
 */
static int play_standard = 0;
int get_play_srv_std (void)
{
	return play_standard;
}

/**
 * @func    delete_curr_video
 * @brief   delete current of video file
 * @param   hls			file list of handle
 * @return  0 for successful, minus if any error occurred
 */
int delete_curr_video (void* hls)
{
	int rc = 0;

	rc = delete_curr_file(hls);

	if(rc >= 0)
		if(video_nls > 0)
			video_nls--;

	if(video_nls <= 0)
		show_no_video_ui(SHOW_THIS_OSD);

	return rc;
}

/**
 * @func    display_playfile
 * @brief   check video file of standard and configuring display.
 * @param   fname		filename
 * @return  0 for successful, minus if any error occurred
 */
static int display_playfile (char *fname)
{
	int rc = 0;
	int std = 0;
	struct av_property prop;

    /* get file of property */
	memset(&prop, 0, sizeof(struct av_property));
    rc = playback_get_property(fname, &prop);
    if (rc < 0) {
        ERROR("playback_get_video_property(), rc = %d\n", rc);
        return -1;
    }

    /* check file of standard base on frame of height */
    if(prop.v_info.height == 240)
    	std = AVIN_CVBS_NTSC;
    else if(prop.v_info.height == 288)
    	std = AVIN_CVBS_PAL;

    /* compare with previous of state and setting display */
	if(std == AVIN_CVBS_NTSC) {
		dbg(2, "play file of standard is NTSC\n");
		if (play_standard != AVIN_CVBS_NTSC) {
			display_set_play_lmod(AVIN_CVBS_NTSC);
			show_ui_enable();		// show UI when L mode, maybe from full mode
		}
		play_standard = AVIN_CVBS_NTSC;
	} else if(std == AVIN_CVBS_PAL) {
		dbg(2, "play file of standard is PAL\n");
		if (play_standard != AVIN_CVBS_PAL) {
			display_set_play_lmod(AVIN_CVBS_PAL);
			show_ui_enable();		// show UI when L mode, maybe from full mode
		}
		play_standard = AVIN_CVBS_PAL;
	} else {
		ERROR("unknown standard or signal loss!\n");
		play_standard = 0;
		return -1;
	}

	return rc;
}

/**
 * @func    set_play_config
 * @brief   setting playfile service of option.
 * @param   play_opt	option structure
 * @return  none
 */
static void set_play_config (struct play_opt_t *play_opt)
{
	/* playfile service of option */
	play_opt->sync = 1;		// 0: not sync, 1: sync with fno
	play_opt->stop = 0;		// 0: quit, 1: stay first frame, 2: stay last frame
}

/**
 * @func    play_cmd_callback
 * @brief   command callback function for playfile service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int play_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check playback of status */
	rc = play_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF)
			cmd = CMD_NEXT;
    	else if(status.state == STOP_SOF)
			cmd = CMD_PREVIOUS;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_PLAYFILE);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_PLAYFILE);
    	//return cmd;
    }

    /* process event */
    switch(cmd) {
    case CMD_NEXT:
    	cmd = CMD_NEXT;
    	break;
    case CMD_PREVIOUS:
		cmd = CMD_PREVIOUS;
    	break;
    case CMD_DOORBELL:
    	cmd = CMD_DOORBELL;
    	break;
    }

    if(status.state == PLAY)
    	play_header_update();

	return cmd;
}

/*
 * playback task for playfile service
 */
void play_srv (struct playsrv_opt_t *playsrv_opt)
{
	HLS *hls = NULL;
    int fattrib;
	int rc = 0;
    int cmd = 0;
    struct play_opt_t play_opt;
    char *play_fname = NULL;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    dbg(0, "Start of playback task\n");
    /* Enable eyecatch effect. */
    display_set_blackscreen_effect();
    
    memset(&play_opt, 0, sizeof(struct play_opt_t));

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

    /* open video list in the FAT file system */
    hls = openls(VIDEO_LS, FATFS);
    if (hls == NULL) {
    	/* show UI for file issue */
    	show_file_issue(SHOW_THIS_OSD);
    	/* show error UI and waiting CMD_QUIT commend or
		 * timeout will auto quit this task */
    	err_bk = 1;
    	ui_err_timeout_cnt();
    	goto WAIT_TASK_CMD;
    }

    /* get video file and unread of total number */
	rc = get_total_video(hls, &video_nls, &video_nread);
	if (rc < 0) {
		if(rc == -2)
			show_no_video_ui(SHOW_THIS_OSD);
		else
			show_file_issue(SHOW_THIS_OSD);
		goto WAIT_TASK_CMD;
	}

	/* play newest file */
	cmd = CMD_PLAY;
	play_standard = 0;	// clear standard value

    /* task of main loop */
	while (1) {
		switch (cmd) {
		case CMD_PLAY:
			rc = get_curr_video(hls, &play_fname, &fattrib);
			if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
			break;
		case CMD_NEXT:
			rc = get_next_video(hls, &play_fname, &fattrib);
			if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
			break;
		case CMD_PREVIOUS:
			rc = get_prev_video(hls, &play_fname, &fattrib);
			if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
			break;
		default:
			dbg(0, "unknow command to playback %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
        }

        dbg(0, "play file name %s\n", play_fname);

        /* check and show new video info */
        if(check_or_fattrib(fattrib, FATTR_READ_BIT))
        	show_new_file(CLEAR_THIS_OSD);
        else
        	show_new_file(SHOW_THIS_OSD);

        /* setting playback mode with standard and display */
        rc = display_playfile(play_fname);
        if (rc < 0) {
            show_standard_issue(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

        /* setting playfile configure */
		set_play_config(&play_opt);

        /* reset UI of frame time */
        show_frm_update(NULL);
        /* show file header info */
        show_play_header(video_fno, video_nls);

        /* display UI for PLAY */
        show_play_icon(SHOW_THIS_OSD);

        /* call playfile service */
        cmd = playfile2(play_fname, &play_opt, &play_cmd_callback);

        /* clear file header */
        show_play_header(0, 0);

        //playfile_get_status();

        /* mark a read flag in file attribute */
        if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
        	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
        		mark_fattrib(hls, play_fname, fattrib , FATTR_READ_BIT);
        		video_nread--;
        	}
        }

        switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:          /* blank screen and wait event */
        	if(get_play_srv_state(&rc) >= 0) {
        		if(rc == STOP_EOF) {
        			cmd = CMD_NEXT;
        			continue;
        		}
        	}
        	goto WAIT_TASK_CMD;
        case CMD_STOP_ERR:		/* if any error occurred */
        	show_message_ui(SHOW_THIS_OSD);
        	rc = get_err(msgbuf);
			if (rc) {
				ERROR("error %d, message : %s\n", rc, msgbuf);
			}
			err_bk = 2;
        	goto WAIT_TASK_CMD;
        case CMD_DOORBELL: 		/* quit task if door bell in this sample */
        	goto EXIT_TASK;
        case CMD_NEXT:          /* get next file and continue playback */
	        cmd = CMD_NEXT;
	        continue;
        case CMD_PREVIOUS:		/* get previous file and continue playback */
	        cmd = CMD_PREVIOUS;
	        continue;
        case CMD_DELETE:
        	show_deleting_ui(SHOW_THIS_OSD);
        	rc = delete_curr_video(hls);
        	show_deleting_ui(CLEAR_THIS_OSD);
        	if(rc < 0) {        /* unknown error */
        		ERROR ("deletels(), rc=%d\n", rc);
                goto WAIT_TASK_CMD;
        	} else {
        		cmd = CMD_PLAY;
        		continue;
        	}
        	break;
        default:
            dbg(0, "wrong cmd=%d\n", cmd);
            //show_message_ui(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

WAIT_TASK_CMD:
        cmd = 0;
        play_standard = 0;	// clear standard value
	    /* waiting command loop */
        while(cmd == 0) {
	    	/* get sys command to know next step */
	        rc = get_sys_cmd(ST_PLAYFILE);
	    	if(rc <= 0)
	    		continue;
	    	/* waiting quit command or timeout */
	    	if(err_bk == 1) {
	    		if(rc != CMD_QUIT) continue;
	    		else ui_err_clear_cnt();
	    	} else if(err_bk == 2) {
	    		if(rc == CMD_NEXT || rc == CMD_PREVIOUS) {
	    			err_bk = 0;
	    			clear_massage_ui();
	    		}
	    	}

	        switch (rc) {
	        case CMD_QUIT:			/* quit task */
	        	goto EXIT_TASK;
			case CMD_PLAY:			/* play again */
				cmd = CMD_PLAY;
				break;
			case CMD_NEXT:     		/* continue next/previous file */
			case CMD_PREVIOUS:
				cmd = rc;
				break;
	        case CMD_DOORBELL:
	        	goto EXIT_TASK;
			case CMD_USER_COPY:
				break;
	        case CMD_DELETE:
	        	show_deleting_ui(SHOW_THIS_OSD);
	        	rc = delete_curr_video(hls);
	        	show_deleting_ui(CLEAR_THIS_OSD);
	        	if(rc < 0) {        /* unknown error */
	        		ERROR ("deletels(), rc=%d\n", rc);
	        	} else {
	        		cmd = CMD_PLAY;
	        	}
	        	break;
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
    dbg(0, "End of playback task\n");
}
