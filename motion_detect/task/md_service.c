/**
 *  @file   md_service.c
 *  @brief
 *  $Id: md_service.c,v 1.3 2016/07/27 10:00:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2014/3/10  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <codec.h>
#include <encpic.h>
#include <video_det.h>
#include <sd.h>
#include <ls.h>
#include <watchdog.h>
#define DBG_HELPER
#include <debug.h>

#include "srv_evt_cmd.h"


#include "mdvid.h"
#include "md_service.h"


#ifdef DBG_LEVEL
#undef DBG_LEVEL
#endif
#define DBG_LEVEL   0
extern void ui_md_pic_mask (int x, int y, int w, int h);

static int std = 0;

/**
 * @func    set_md_snap_config
 * @brief   setting encpic service of option.
 * @param   epic_opt	option structure
 * @return  none
 */
static int set_md_snap_config (struct epic_opt_t *epic_opt, struct md_opt_t *md_opt)
{
	int rc = 0;
//	int std = 0;

	/* get current of signal status */
	std = cvbs_get_stable_standard();

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
//    	else/* get system of event */
//    	    cmd = get_sys_cmd(ST_MDVID);
    } else { /* invalid, maybe codec taking */
    	cmd = get_sys_cmd(ST_MDVID);
    	//return cmd;
    }
    watchdog_keepalive(10000000);

    
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
	int rc = 0;
    int cmd = 0;
   
    struct epic_opt_t snap_opt;
    struct sysc_status_t status;

    char msgbuf[ERR_MSG_LEN];
   
    int x = 0, y = 0, w = 0, h = 0;

    //Disable FPLL for dbg.
    writel(0x00, 0xB04013A0);

    dbg(0, "Start of MD task\n");

    memset(&snap_opt, 0, sizeof(struct epic_opt_t));
    snap_opt.dev = md_opt->dev;



	/* image of motion detection */
    cmd = CMD_MD_VID;

    /* task of main loop */
    while (1) {
		switch (cmd) {
		case CMD_MD_VID:
	        /* setting MD/REC configure */
            
			rc = set_md_snap_config(&snap_opt, md_opt);
	        if (rc < 0) {
	            unsigned char ch = readl(0xb0400010)&0x03;            
                if(++ch>= md_opt->detect_channels)
                    ch = 0;
                writel(ch, 0xb0400010);
                printf("switch to ch =%d\n", ch);
                usleep(10000);
                continue;
	        }
           
	        /* clear REC icon */
	        //show_snap_icon(CLEAR_THIS_OSD);
	        cmd = mdvid(md_opt, &md_cmd_callback);
			if(cmd == CMD_STOP) {
				md_status(&status);
	        	if(status.state == STOP_MD) {
					dbg(2, "Motion detect!\n\r");				
					dbg(2, " md_opt->mt_x = %d \n", md_opt->mt_x*8);
					dbg(2, " md_opt->mt_y = %d \n", md_opt->mt_y*8);
					dbg(2, " md_opt->mt_w = %d \n", md_opt->mt_w*8);
					dbg(2, " md_opt->mt_h = %d \n", md_opt->mt_h*8);
                    x = md_opt->mt_x*8;
                    y = md_opt->mt_y*8;
                    w = md_opt->mt_w*8;
                    h = md_opt->mt_h*8;
                    if(w != 0 && h != 0) {
                        x = ((x * 800 * 100)/720)/100;
                        w = ((w * 800 * 100)/720)/100;
                        if(std == AVIN_CVBS_PAL) {
                            y = ((y * 480 * 100)/288)/100;
                            h = ((h * 480 * 100)/288)/100;
                        } else {
                            y = (y * (480/240));
                            h = (h * (480/240));
                        }
                        printf("Channel = %d\n", md_opt->md_channel);
                        printf("CVBS x=%d, y=%d, w=%d, h=%d\n", md_opt->mt_x*8,md_opt->mt_y*8,md_opt->mt_w*8,md_opt->mt_h*8);
                        printf("Panel  x=%d, y=%d, w=%d, h=%d\n", x,y,w,h);
                        ui_md_pic_mask(x, y, w, h);
                    }

                    
					cmd = CMD_MD_VID;//CMD_REC;
					continue;
				}
			}
			break;
		
		default:
			dbg(0, "unknow command to record %d\n", cmd);
		
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
       
            rc = get_err(msgbuf);
            if (rc) {
                ERROR("error %d, message : %s\n", rc, msgbuf);
            }
			
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
	    	
	        switch (rc) {
	        case CMD_STOP:		/* quit task */
	        case CMD_QUIT:
            case CMD_CVD_LOST:
	        	goto EXIT_TASK;
	       
			default:
				dbg(0, "unknown cmd=%d\n", rc);
				break;
            }
        } /* end of task wait command loop */
    } /* end of task main loop */

EXIT_TASK:


//EXIT:
	
    dbg(0, "End of MD task\n");
}
