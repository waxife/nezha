/**
 *  @file   auplay_service.c
 *  @brief
 *  $Id: auplay_service.c,v 1.2 2014/05/29 05:25:08 ken Exp $
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
#include <codec_eng/aumsg.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "auplay_service.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * static variable of audio play task for store file numbers
 */
static int aumsg_fno = 0;
static int aumsg_nread = 0;
static int aumsg_nls = 0;

/**
 * static variable of audio play task for store statistic bar
 */
static int aumsg_dura = 0;
static int auplay_bar_cnt = 0;
static int auplay_bar_time = 0;

/**
 * @func    auplay_bar_update
 * @brief   draw statistic bar OSD when time update
 * @param   clear	1 to clear this OSD
 * 			times	millisecond
 * @return  none
 */
#define AUPLAY_BAR_STEP	32	// set one step is 0.5s
static void auplay_bar_update (int clear, int times)
{
	if(!clear) {
		// show audio statistic bar
		if(auplay_bar_time == 0) {
			auplay_bar_time = 1;
			auplay_bar_cnt += ((AUPLAY_BAR_STEP*1024)/aumsg_dura);
            if(auplay_bar_cnt > AUPLAY_BAR_STEP || auplay_bar_cnt < 0)
                auplay_bar_cnt = AUPLAY_BAR_STEP;
			show_audmsg_rec_time(auplay_bar_cnt);
		} else
		if((times - auplay_bar_time) >= 1000) {
			if(auplay_bar_cnt < AUPLAY_BAR_STEP) {
				auplay_bar_cnt += ((AUPLAY_BAR_STEP*1024)/aumsg_dura);
				if(auplay_bar_cnt > AUPLAY_BAR_STEP || auplay_bar_cnt < 0)
					auplay_bar_cnt = AUPLAY_BAR_STEP;
				show_audmsg_rec_time(auplay_bar_cnt);
			}
			auplay_bar_time = times;
		}
	} else {
		// clear audio statistic bar
		auplay_bar_cnt = 0;
		auplay_bar_time = 0;
		show_audmsg_rec_time(0);
	}
}

/**
 * @func    aumsg_header_update
 * @brief   draw header(time) OSD when time update
 * @param   fs			file system
 * 			fname		filename
 * @return  none
 */
#define AU_HEADER_LEN		20
static void aumsg_header_update (int fs, char* fname)
{
	int rc = 0;
	char prop_str[AU_HEADER_LEN + 1] = { '\0' };
	struct aumsg_property prop;

	aumsg_dura = 0;

	memset(&prop, 0, sizeof(struct aumsg_property));
	rc = aumsg_get_info(fs, fname, &prop);
	if (rc < 0) {
		ERROR("aumsg_get_info(), rc = %d\n", rc);
		return;
	}

//	printf ("duration: %d ms, length: %d byte\n",
//			prop.duration, prop.length);
	aumsg_dura = prop.duration;

	snprintf(prop_str, AU_HEADER_LEN, "%04d/%02d/%02d-%02d:%02d:%02d ",
		prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec);

	show_aumsg_time(prop_str);
}

/**
 * @func    show_audio_header
 * @brief   draw header(file number) OSD
 * @param   no			number of file
 * 			no_tal		total number of file
 * @return  none
 */
#define AUDPLAY_HEADER_LEN		9
static void show_audio_header (int no, int no_tal)
{
	char header_str[AUDPLAY_HEADER_LEN + 1] = { '\0' };

	if(no_tal != 0) {
		// show audio file of header
		snprintf(header_str, AUDPLAY_HEADER_LEN, "%04d/%04d", no, no_tal);
		show_file_header(header_str);
	} else {
		// clear audio file of header
		show_file_header(NULL);
	}
}

/**
 * @func    get_total_audio
 * @brief   scan and get a number of audio file in file list
 * @param   hls			file list of handle
 * 			nls			total number of file
 * 			unread		number of unread file
 * @return  0 for successful, -1 if any error occurred, -2 if no audio file
 */
static int get_total_audio (void* hls, int* nls, int* nread)
{
	int rc = 0;

	aumsg_fno = 0;

	rc = get_total_file(hls, nls, nread);
	if (rc < 0) {
		dbg(0, "file issue!\n");
		return -1;
	}
	if (*nls <= 0) {
		dbg(0, "no aumsg file!\n");
		return -2;
	} else {
		INFO("total aumsg: %d\n", *nls);
		INFO("total new aumsg: %d\n", *nread);
	}

	aumsg_fno = *nls;

	return rc;
}

/**
 * @func    get_curr_audio
 * @brief   get filename and attribute at the current position of audio file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_curr_audio (void* hls, char** pos, int* fattrib)
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
 * @func    get_next_audio
 * @brief   get filename and attribute at the next position of audio file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_next_audio (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_next_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		aumsg_fno = 0;
	} else if (rc == 1) {	/* in head now */
		aumsg_fno = 1;
		rc = 0;
	} else {
		aumsg_fno = (aumsg_fno < aumsg_nls)? (aumsg_fno+1) : aumsg_nls;
	}

	return rc;
}

/**
 * @func    get_prev_audio
 * @brief   get filename and attribute at the previous position of audio file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_prev_audio (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_prev_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		aumsg_fno = 0;
	} else if (rc == 1) {	/* in tail now */
		aumsg_fno = aumsg_nls;
		rc = 0;
	} else {
		aumsg_fno = (aumsg_fno > 1)? (aumsg_fno-1) : 1;
	}

	return rc;
}

/**
 * @func    decau_get_status
 * @brief   check decau service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
static int decau_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = decau_status(&status);
    if (rc < 0) {
        printf("Failed to get decode audio file status. (%d)\n", rc);
        goto EXIT;
    }

    printf("state = %d\n", status.state);
    printf("fno = %d\n", status.fno);
    printf("size = %d\n", status.size);
    printf("quality = %d\n", status.quality);

EXIT:
    return rc;
}

/**
 * @func    get_aumsg_total
 * @brief   get total number of audio file from task.
 * @param   none
 * @return  total number
 */
int get_aumsg_total (void)
{
	return aumsg_nls;
}

/**
 * @func    get_aumsg_nread
 * @brief   get unread number of audio file from task.
 * @param   none
 * @return  unread number
 */
int get_aumsg_nread (void)
{
	return aumsg_nread;
}

/**
 * @func    get_aumsg_srv_state
 * @brief   get decau service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int get_aumsg_srv_state (int *state)
{
	int rc = 0;
	struct sysc_status_t status;
	*state = 0;

    rc = decau_status(&status);
    if (rc < 0) {
        goto EXIT;
    }
    *state = status.state;

EXIT:
	return rc;
}

/**
 * @func    delete_curr_aud
 * @brief   delete current of audio file
 * @param   hls			file list of handle
 * @return  0 for successful, minus if any error occurred
 */
int delete_curr_aud (void* hls)
{
	int rc = 0;

	rc = delete_curr_file(hls);

	if(rc >= 0)
		if(aumsg_nls > 0)
			aumsg_nls--;

	if(aumsg_nls <= 0)
		show_no_audio_ui(SHOW_THIS_OSD);

	return rc;
}

/**
 * @func    set_aumsg_config
 * @brief   setting decau service of option.
 * @param   dau_opt_t	option structure
 * @return  none
 */
static void set_aumsg_config (struct dau_opt_t *dau_opt, FS dev, MEDIA media)
{
	/* audio play service of option */
	dau_opt->dev = dev;
	dau_opt->media = media;	// can select MEDIA_USER0 or MEDIA_USER1 if created
}

/**
 * @func    auplay_cmd_callback
 * @brief   command callback function for decau service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int auplay_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check decode audio of status */
	rc = decau_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF)
			cmd = CMD_STOP;	// no repeat // cmd = CMD_NEXT;
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_DECAU);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_DECAU);
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

    auplay_bar_update(SHOW_THIS_OSD, status.time); 
    
	return cmd;
}

/*
 * audio play task for decau service
 */
void auplay_srv (struct auplaysrv_opt_t *auplaysrv_opt)
{
	HLS *hls = NULL;
	int fattrib;
	int rc = 0;
    int cmd = 0;
    struct dau_opt_t dau_opt;
    char* aumsg_fname = NULL;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    dbg(0, "Start of audio play task\n");

    memset(&dau_opt, 0, sizeof(struct dau_opt_t));
    /* setting decode audio of file system */
	set_aumsg_config(&dau_opt, auplaysrv_opt->dev, MEDIA_AUMSG);

    if(dau_opt.dev == FATFS) {
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

    /* open audio list of NOR/FAT file system */
    hls = openls(dau_opt.media, dau_opt.dev);
    if (hls == NULL) {
    	/* show UI for NOR status */
    	if(dau_opt.dev == FATFS)
    		show_file_issue(SHOW_THIS_OSD);
    	else
    		show_nor_issue(SHOW_THIS_OSD);
    	/* show error UI and waiting CMD_QUIT commend or
    	 * timeout will auto quit this task */
    	err_bk = 1;
    	ui_err_timeout_cnt();
    	goto WAIT_TASK_CMD;
    }

    /* get audio file and unread of total number */
	rc = get_total_audio(hls, &aumsg_nls, &aumsg_nread);
	if (rc < 0) {
		if(rc == -2)
			show_no_audio_ui(SHOW_THIS_OSD);
		else
			show_file_issue(SHOW_THIS_OSD);
		goto WAIT_TASK_CMD;
	}
	/* play newest file */
	cmd = CMD_PLAY;

	/* task of main loop */
	while (1) {
		switch (cmd) {
        case CMD_PLAY:
        	rc = get_curr_audio(hls, &aumsg_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        case CMD_NEXT:
        	rc = get_next_audio(hls, &aumsg_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        case CMD_PREVIOUS:
        	rc = get_prev_audio(hls, &aumsg_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        default:
			dbg(0, "unknow command to audio play %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
        }

        dbg(0, "audio play file name %s\n", aumsg_fname);

        /* check and show new audio info */
        if(check_or_fattrib(fattrib, FATTR_READ_BIT))
        	show_new_file(CLEAR_THIS_OSD);
        else
        	show_new_file(SHOW_THIS_OSD);

        /* show file header info */
        show_audio_header(aumsg_fno, aumsg_nls);

        /* display UI for PLAY */
		show_audplay_icon(SHOW_THIS_OSD);
		auplay_bar_update(CLEAR_THIS_OSD, 0);

		aumsg_header_update(dau_opt.dev, aumsg_fname);

        /* call decau service */
        cmd = dec_aumsg(aumsg_fname, &dau_opt, &auplay_cmd_callback);

        /* clear file header */
        show_audio_header(0, 0);
        auplay_bar_update(CLEAR_THIS_OSD, 0);

        decau_get_status();

        /* mark read in the file attribute */
        if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
        	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
				mark_fattrib(hls, aumsg_fname, fattrib , FATTR_READ_BIT);
				aumsg_nread--;
			}
        }

        /* process a return command from service */
        switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
        case CMD_STOP:          /*  */
        	goto WAIT_TASK_CMD;
        case CMD_STOP_ERR:		/* if any error occurred */
        	show_message_ui(SHOW_THIS_OSD);
        	rc = get_err(msgbuf);
			if (rc) {
				ERROR("error %d, message : %s\n", rc, msgbuf);
			}
			err_bk = 2;
        	goto WAIT_TASK_CMD;
        case CMD_NEXT:          /* get next file and continue audio play */
	        cmd = CMD_NEXT;
	        continue;
        case CMD_PREVIOUS:		/* get previous file and continue audio play */
	        cmd = CMD_PREVIOUS;
	        continue;
        case CMD_DELETE:
        	show_deleting_ui(SHOW_THIS_OSD);
        	rc = delete_curr_aud(hls);
        	show_deleting_ui(CLEAR_THIS_OSD);
        	if(rc < 0) { /* unknown error */
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
        /* waiting command loop */
	    while(cmd == 0) {
	    	/* get sys command to know next step */
	        rc = get_sys_cmd(ST_DECAU);
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
	        case CMD_QUIT:
	        	goto EXIT_TASK;
			case CMD_PLAY:
				cmd = CMD_PLAY;
				break;
			case CMD_NEXT:     	/* continue next/previous file */
			case CMD_PREVIOUS:
				cmd = rc;
				break;
	        case CMD_DOORBELL:
	        	goto EXIT_TASK;
			case CMD_USER_COPY:	/* copy from NOR to SD card */
				if(dau_opt.dev == NORFS) {
					show_copying_ui(SHOW_THIS_OSD);
					rc = aumsg_backup2sd(AUMSG_LS, aumsg_fname);
					show_copying_ui(CLEAR_THIS_OSD);
					if(rc < 0)
						show_file_issue(SHOW_THIS_OSD);
				}
				cmd = CMD_PLAY;
				break;
	        case CMD_DELETE:
	        	show_deleting_ui(SHOW_THIS_OSD);
	        	rc = delete_curr_aud(hls);
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
    dbg(0, "End of audio play task\n");
}
