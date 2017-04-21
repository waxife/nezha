/**
 *  @file   pview_service.c
 *  @brief
 *  $Id: pview_service.c,v 1.2 2014/03/27 10:44:59 ken Exp $
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
#include <codec_eng/decpic.h>
#include <display.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>

#define DBG_HELPER
#include <debug.h>

#include "file_manip.h"
#include "srv_evt_cmd.h"
#include "pview_service.h"
#include "eyecatch.h"
#include "md_bks.h"

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#else
#include "task_no_ui.h"
#endif

/**
 * static variable of picture view task for store file numbers
 */
static int photo_fno = 0;
static int photo_nread = 0;
static int photo_nls = 0;
static int pic_md_x = 0;
static int pic_md_y = 0;
static int pic_md_w = 0;
static int pic_md_h = 0;
static int copy_flag = 0;
/**
 * @func    photo_header_update
 * @brief   draw header(frame time) OSD when frame time update
 * @param   fs			file system
 * 			fname		filename
 * @return  none
 */
#define PROP_HEADER_LEN		20
static void photo_header_update (int fs, char* fname)
{
	int rc = 0;
	char prop_str[PROP_HEADER_LEN+1] = { '\0' };
	struct pic_property prop;

    memset(&prop, 0, sizeof(struct pic_property));
    rc = pic_get_info(fs, fname, &prop);
    if (rc < 0) {
        ERROR("pic_get_info(), rc = %d\n", rc);
        return;
    }

	snprintf(prop_str, PROP_HEADER_LEN, "%04d/%02d/%02d-%02d:%02d:%02d ",
		prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec);

	show_photo_time(prop_str);
}

/**
 * @func    show_photo_header
 * @brief   draw header(file number) OSD
 * @param   no			number of file
 * 			no_tal		total number of file
 * @return  none
 */
#define PVIEW_HEADER_LEN		9
static void show_photo_header (int no, int no_tal)
{
	char header_str[PVIEW_HEADER_LEN + 1] = { '\0' };

	if(no_tal != 0) {
		// show photo file of header
		snprintf(header_str, PVIEW_HEADER_LEN, "%04d/%04d", no, no_tal);
		show_file_header(header_str);
	} else {
		// clear photo file of header
		show_file_header(NULL);
	}
}

/**
 * @func    get_total_photo
 * @brief   scan and get a number of photo file in file list
 * @param   hls			file list of handle
 * 			nls			total number of file
 * 			unread		number of unread file
 * @return  0 for successful, -1 if any error occurred, -2 if no photo file
 */
static int get_total_photo (void* hls, int* nls, int* nread)
{
	int rc = 0;

	photo_fno = 0;

	rc = get_total_file(hls, nls, nread);
	if (rc < 0) {
		dbg(0, "file issue!\n");
		return -1;
	}
	if (*nls <= 0) {
		dbg(0, "no photo file!\n");
		return -2;
	} else {
		INFO("total photo: %d\n", *nls);
		INFO("total new photo: %d\n", *nread);
	}

	photo_fno = *nls;

	return rc;
}

/**
 * @func    get_curr_photo
 * @brief   get filename and attribute at the current position of photo file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_curr_photo (void* hls, char** pos, int* fattrib)
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
 * @func    get_next_photo
 * @brief   get filename and attribute at the next position of photo file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_next_photo (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_next_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		photo_fno = 0;
	} else if (rc == 1) {	/* in head now */
		photo_fno = 1;
		rc = 0;
	} else {
		photo_fno = (photo_fno < photo_nls)? (photo_fno+1) : photo_nls;
	}

	return rc;
}

/**
 * @func    get_prev_photo
 * @brief   get filename and attribute at the previous position of photo file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
static int get_prev_photo (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = get_prev_file(hls, pos, fattrib);
	if (rc < 0) {
		ERROR ("get file list is fail!\n");
		show_file_issue(SHOW_THIS_OSD);
		photo_fno = 0;
	} else if (rc == 1) {	/* in tail now */
		photo_fno = photo_nls;
		rc = 0;
	} else {
		photo_fno = (photo_fno > 1)? (photo_fno-1) : 1;
	}

	return rc;
}

/**
 * @func    playfile_get_status
 * @brief   check decpic service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
static int decpic_get_status (void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = decpic_status(&status);
    if (rc < 0) {
        printf("Failed to get decode picture file status. (%d)\n", rc);
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
 * @func    get_photo_total
 * @brief   get total number of photo file from task.
 * @param   none
 * @return  total number
 */
int get_photo_total (void)
{
	return photo_nls;
}

/**
 * @func    get_photo_nread
 * @brief   get unread number of photo file from task.
 * @param   none
 * @return  unread number
 */
int get_photo_nread (void)
{
	return photo_nread;
}

/**
 * @func    get_pview_srv_state
 * @brief   get decpic service of status.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int get_pview_srv_state (int *state)
{
	int rc = 0;
	struct sysc_status_t status;
	*state = 0;

    rc = decpic_status(&status);
    if (rc < 0) {
        goto EXIT;
    }
    *state = status.state;

EXIT:
	return rc;
}

/**
 * @func    get_pview_srv_std
 * @brief   get photo file of standard.
 * @param   none
 * @return  video standard
 */
static int pview_standard = 0;
int get_pview_srv_std (void)
{
	return pview_standard;
}

void get_pic_md_block (int *x, int *y, int *w, int *h)
{
	*x = pic_md_x;
	*y = pic_md_y;
	*w = pic_md_w;
	*h = pic_md_h;
}

/**
 * @func    delete_curr_pic
 * @brief   delete current of picture file
 * @param   hls			file list of handle
 * @return  0 for successful, minus if any error occurred
 */
int delete_curr_pic (void* hls)
{
	int rc = 0;

	rc = delete_curr_file(hls);

	if(rc >= 0)
		if(photo_nls > 0)
			photo_nls--;

	if(photo_nls <= 0)
		show_no_picture_ui(SHOW_THIS_OSD);

	return rc;
}

/**
 * @func    display_pview
 * @brief   check photo file of standard and configuring display.
 * @param   fname		filename
 * @return  0 for successful, minus if any error occurred
 */
static int display_pview (int fs, char* fname)
{
	int rc = 0;
	int std = 0;
	struct pic_property prop;

	/* get file of property */
	memset(&prop, 0, sizeof(struct pic_property));
    rc = pic_get_info(fs, fname, &prop);
    if (rc < 0) {
        ERROR("pic_get_info(), rc = %d\n", rc);
        return -1;
    }

    if(prop.user_info_len) {
    	pic_md_x = ((unsigned char)prop.user_info[0]) * MD_BLOCK_SIZE;
    	pic_md_y = ((unsigned char)prop.user_info[1]) * MD_BLOCK_SIZE;
    	pic_md_w = ((unsigned char)prop.user_info[2]) * MD_BLOCK_SIZE;
    	pic_md_h = ((unsigned char)prop.user_info[3]) * MD_BLOCK_SIZE;
    	dbg(3, "prop.user_info_len = %d \n", prop.user_info_len);
    	dbg(3, "pic_md_x = %d \n", pic_md_x);
    	dbg(3, "pic_md_y = %d \n", pic_md_y);
    	dbg(3, "pic_md_w = %d \n", pic_md_w);
    	dbg(3, "pic_md_h = %d \n", pic_md_h);
    } else {
    	pic_md_x = 0;
    	pic_md_y = 0;
    	pic_md_w = 0;
    	pic_md_h = 0;
    }

    /* check file of standard base on frame of height */
    if(prop.height == 240)
    	std = AVIN_CVBS_NTSC;
    else if(prop.height == 288)
    	std = AVIN_CVBS_PAL;

    /* compare with previous of state and setting display */
    if(std == AVIN_CVBS_NTSC) {
		dbg(2, "photo file of standard is NTSC\n");
		if (pview_standard != AVIN_CVBS_NTSC)
			display_set_play_lmod(AVIN_CVBS_NTSC);
		pview_standard = AVIN_CVBS_NTSC;

	} else if(std == AVIN_CVBS_PAL) {
		dbg(2, "photo file of standard is PAL\n");
		if (pview_standard != AVIN_CVBS_PAL)
		{
			display_set_play_full(AVIN_CVBS_PAL);
			//dbg(2, "1111111111111111\r\n");
		}/*display full view*/
		pview_standard = AVIN_CVBS_PAL;

	} else {
		ERROR("unknown standard or signal loss!\n");
		pview_standard = 0;
		return -1;
	}

	return rc;
}

/**
 * @func    set_pview_config
 * @brief   setting decpic service of option.
 * @param   dpic_opt	option structure
 * @return  none
 */
static void set_pview_config (struct dpic_opt_t *dpic_opt)
{
	/* picture view service of option */
	dpic_opt->duration = 0;
	dpic_opt->effect = 0;

}

/**
 * @func    pview_cmd_callback
 * @brief   command callback function for decpic service.
 * @param   arg		according to corresponding command
 * @return  command
 */
static int pview_cmd_callback (void **arg)
{
	struct sysc_status_t status;
	int rc = 0;
	int cmd = 0;

    /* check decode picture of status */
	rc = decpic_status(&status);
    if (rc == 0) {	/* success */
    	if(status.state == STOP_ERR)
    		cmd = CMD_STOP_ERR;
    	else if(status.state == STOP_EOF) {
			dbg(0, "decode finish\n");
    	}
    	else/* get system of event */
    	    cmd = get_sys_cmd(ST_DECPIC);
		//dbg(2,"111111111\r\n");
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_DECPIC);
		//dbg(2,"222222222\r\n");
    	//return cmd;
    }
	//dbg(0,"cmd=%d\r\n", cmd);
	return cmd;
    /* process event */
    switch(cmd) {
    case CMD_NEXT:
    	cmd = CMD_NEXT;
    	break;
    case CMD_PREVIOUS:
		cmd = CMD_PREVIOUS;
    	break;
    case CMD_USER_COPY:
    	cmd = CMD_STOP;
    	copy_flag = 1;
    	break;
    case CMD_DOORBELL:
    	//cmd = CMD_DOORBELL;
    	cmd = CMD_QUIT;		/* quit task if door bell in this sample */
    	break;
    }

//    if(status.state == STOP_EOF)
//    	photo_header_update();

	return cmd;
}

/*
 * picture view task for decpic service
 */
void pview_srv (struct pviewsrv_opt_t *pviewsrv_opt)
{
	HLS *hls = NULL;
    int fattrib;
	int rc = 0;
    int cmd = 0;
    struct dpic_opt_t dpic_opt;
    char* photo_fname = NULL;
    char msgbuf[ERR_MSG_LEN];
    int err_bk = 0;

    dbg(0, "Start of picture view task\n");
    
    /* Enable eyecatch effect. */
    //display_set_blackscreen_effect();

	//dbg(0, "Start of picture view task\n");
    //goto EXIT_TASK;
    
    memset(&dpic_opt, 0, sizeof(struct dpic_opt_t));

    dpic_opt.dev = pviewsrv_opt->dev;

    if(dpic_opt.dev == FATFS) {
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
    hls = openls(PIC_LS, dpic_opt.dev);
    if (hls == NULL) {
    	/* show UI for NOR status */
    	if(dpic_opt.dev == FATFS)
    		show_file_issue(SHOW_THIS_OSD);
    	else
    		show_nor_issue(SHOW_THIS_OSD);
    	/* show error UI and waiting CMD_QUIT commend or
    	 * timeout will auto quit this task */
    	err_bk = 1;
    	ui_err_timeout_cnt();
    	goto WAIT_TASK_CMD;
    }

    /* get photo file and unread of total number */
	rc = get_total_photo(hls, &photo_nls, &photo_nread);
	if (rc < 0) {
		if(rc == -2)
			show_no_picture_ui(SHOW_THIS_OSD);
		else
			show_file_issue(SHOW_THIS_OSD);
		goto WAIT_TASK_CMD;
	}

	/* view newest file */
	cmd = CMD_PLAY;
	pview_standard = 0;	// clear standard value
	copy_flag = 0;

	/* task of main loop */
	while (1) {
		switch (cmd) {
        case CMD_PLAY:
        	rc = get_curr_photo(hls, &photo_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        case CMD_NEXT:
        	rc = get_next_photo(hls, &photo_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        case CMD_PREVIOUS:
        	rc = get_prev_photo(hls, &photo_fname, &fattrib);
        	if (rc < 0) {
				goto WAIT_TASK_CMD;
			}
            break;
        default:
			dbg(0, "unknow command to picture view %d\n", cmd);
			show_message_ui(SHOW_THIS_OSD);
			goto WAIT_TASK_CMD;
        }

        dbg(0, "photo file name %s\n", photo_fname);

        /* setting picture view configure */
        set_pview_config(&dpic_opt);

        /* check and show new photo info */
        if(check_or_fattrib(fattrib, FATTR_READ_BIT))
        	show_new_file(CLEAR_THIS_OSD);
        else
        	show_new_file(SHOW_THIS_OSD);

        /* setting picture view mode with standard and display */
        rc = display_pview(dpic_opt.dev, photo_fname);
		dbg(2, "pview_standard = %d\r\n", pview_standard);
		
        if (rc < 0) {
            show_standard_issue(SHOW_THIS_OSD);
            goto WAIT_TASK_CMD;
        }

        /* show file header info */
        show_photo_header(photo_fno, photo_nls);
        photo_header_update(dpic_opt.dev, photo_fname);

        /* call decpic service */
        cmd = decpic(photo_fname, &dpic_opt, &pview_cmd_callback);

        /* clear file header */
        show_photo_header(0, 0);

        decpic_get_status();

        /* mark read in the file attribute */
        if ((cmd != CMD_STOP_ERR) && (cmd != CMD_DELETE)) {
        	if(!check_or_fattrib(fattrib, FATTR_READ_BIT)) {
				mark_fattrib(hls, photo_fname, fattrib , FATTR_READ_BIT);
				photo_nread--;
        	}
        }

        /* process a return command from service */
        switch(cmd) {
        case CMD_QUIT:  		/* quit task */
	        goto EXIT_TASK;
			#if 0
        case CMD_STOP:          /* blank screen and wait event */
			if(copy_flag) {
				copy_flag = 0;
        		if(dpic_opt.dev == NORFS) {
					show_copying_ui(SHOW_THIS_OSD);
					//rc = pic_backup2sd(PIC_LS, photo_fname);
					show_copying_ui(CLEAR_THIS_OSD);
					if(rc < 0) {
						show_file_issue(SHOW_THIS_OSD);
						goto WAIT_TASK_CMD;
					}
				}
				cmd = CMD_PLAY;
				continue;
        	} else
        	goto WAIT_TASK_CMD;
        case CMD_STOP_ERR:		/* if any error occurred */
        	show_message_ui(SHOW_THIS_OSD);
        	rc = get_err(msgbuf);
			if (rc) {
				ERROR("error %d, message : %s\n", rc, msgbuf);
			}
			err_bk = 2;
        	goto WAIT_TASK_CMD;
        case CMD_DOORBELL:		/* quit task if door bell in this sample */
        	goto EXIT_TASK;
			#endif
        case CMD_NEXT:          /* get next file and continue picture view */
	        cmd = CMD_NEXT;
	        continue;
        case CMD_PREVIOUS:	/* get previous file and continue picture view */
	        cmd = CMD_PREVIOUS;
	        continue;
        case CMD_DELETE:
			
        	//show_deleting_ui(SHOW_THIS_OSD);
        	rc = delete_curr_pic(hls);
        	//show_deleting_ui(CLEAR_THIS_OSD);
        	if(rc < 0) { 		/* unknown error */
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
        pview_standard = 0;	// clear standard value
        copy_flag = 0;
        /* waiting command loop */
	    while(cmd == 0) {
	    	/* get sys command to know next step */
	    	rc = get_sys_cmd(ST_DECPIC);
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
			case CMD_NEXT:     	/* continue next/previous file */
			case CMD_PREVIOUS:
				cmd = rc;
				break;
			case CMD_USER_COPY:	/* copy from NOR to SD card */
				if(dpic_opt.dev == NORFS) {
					//show_copying_ui(SHOW_THIS_OSD);
					//rc = pic_backup2sd(PIC_LS, photo_fname);
					//show_copying_ui(CLEAR_THIS_OSD);
					if(rc < 0)
						show_file_issue(SHOW_THIS_OSD);
				}
				cmd = CMD_PLAY;
				break;
	        case CMD_DELETE:
	        	//show_deleting_ui(SHOW_THIS_OSD);
	        	dbg(2, "ddfkjflsjaf\r\n");
	        	rc = delete_curr_pic(hls);
	        	//show_deleting_ui(CLEAR_THIS_OSD);
	        	if(rc < 0) { 		/* unknown error */
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
	//clear_massage_ui();
	printf("End of picture view task\n");
}
