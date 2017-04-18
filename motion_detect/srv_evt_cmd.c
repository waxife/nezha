/**
 *  @file   srv_evt_cmd.c
 *  @brief
 *  $Id: srv_evt_cmd.c,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2013/11/22  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys.h>
#include <ctype.h>
#include <codec.h>
#include <serial.h>
#include <command.h>
#include <codec_eng/ringbell.h>
#include <codec_eng/encpic.h>
#include <video_det.h>

#include <sys/include/display.h>
#include <./drivers/touch/touch.h>
#include <./drivers/sarkey/sarkey.h>
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"

#define DBG_LEVEL   0
#include <debug.h>
#include "tw_widget_sys.h"
#include "task/req_service.h"
#include "keymap.h"
#include "eyecatch.h"
#include "srv_evt_cmd.h"






#define SHOW_OSD_TIMER_ID	0
#define	SHOW_OSD_TIMER_SEC	5000 // msec
void ui_err_timeout_cnt (void)
{
	tw_set_timer(SHOW_OSD_TIMER_ID, SHOW_OSD_TIMER_SEC);
}

void ui_err_clear_cnt (void)
{
	tw_clear_timer(SHOW_OSD_TIMER_ID);
}




static int __attribute__((unused))
slide_srv_process(unsigned char key)
{
	int cmd = 0;

	switch(key) {

	}

	return cmd;
}

static int md_srv_process(unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
	case KEY_CODE_2:
	case KEY_CODE_3:
	case KEY_CODE_4:
	case KEY_CODE_5:
	case KEY_CODE_6:
	case KEY_CODE_7:
		cmd = CMD_QUIT;
		break;
	case EVENT_CVD_LOST:
		cmd = CMD_CVD_LOST;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}

/**
 * @func    srv_event_parse
 * @brief   according to service ID and parse to task command from key
 * @param   key			key code
 * 			srv_id		current active of service ID
 * @return  parsed of command
 */
static int srv_event_parse (unsigned char key, int srv_id)
{
	switch(srv_id) {
	case ST_PLAYFILE:
	
		break;
	case ST_RECFILE:
		break;
	case ST_DECPIC:
		break;
	case ST_ENCPIC:
		break;
	case ST_DECAU:
		break;
	case ST_ENCAU:
		break;
//	case ST_SLIDE:
//		return slide_srv_process(key);
//		break;
	case ST_MDVID:
		return md_srv_process(key);
		break;
	case ST_LSF:
		break;

	}

	return 0;
}





/**
 * @func    srv_signal_process
 * @brief   parse to key event when any signal event occurred
 * @param   std		video of standard
 * 			srv_id	current active of service ID
 * @return  parsed of event
 */
int srv_signal_process (unsigned char std, int srv_id)
{
	unsigned char key = 0;

	switch (std) {
	case 1:
		key = EVENT_CVD_NTSC;
		break;
	case 2:
		key = EVENT_CVD_PAL;
		break;
	default:
		key = EVENT_CVD_LOST;
		break;
	}

	cvbs_reset_st(AVIN_CVBS_UNKNOW);

	return srv_event_parse(key, srv_id);
}

/**
 * @func    get_sys_cmd
 * @brief   check all event and parse to command for service
 * @param   srv_id		current active of service ID
 * @return  parsed of command
 * @note	ever task and service of callback function both
 * 			call this function and get a parsed of command
 */
int get_sys_cmd (int srv_id)
{
	TW_EVENT* event;

	/* get system of event */
	event = tw_get_sys_evt();

	/* event process & parse to command */
	switch(event->type) {
	case TW_EVENT_TYPE_TOUCH_DOWN:
//	case TW_EVENT_TYPE_TOUCH_MOTION:
	case TW_EVENT_TYPE_TOUCH_RELEASE:
	case TW_EVENT_TYPE_TOUCH_REPEAT:
		
		break;
	case TW_EVENT_TYPE_IR_DOWN:
		
		break;
	case TW_EVENT_TYPE_KEY_DOWN:
		
		break;
	case TW_EVENT_TYPE_SIGNAL:
		dbg(2, ">>>>>> Signal Event\n\r");

		return srv_signal_process(event->signal.std, srv_id);
		break;
	case TW_EVENT_TYPE_GPIO:
        
	case TW_EVENT_TYPE_BELL:
        
        break;
	case TW_EVENT_TYPE_TIMEOUT:
        dbg(2, ">>>>>> Timeout Event\n\r");
        if(event->timer.id == SHOW_OSD_TIMER_ID) {
        	return srv_event_parse(EVENT_ERR_TIMEOUT, srv_id);
		}
        break;
    case TW_EVENT_TYPE_OTHER:
    	dbg(2, ">>>>>> Other Event\n\r");

    	break;
	default:	/* no command */

		break;
	}

	return 0;
}
