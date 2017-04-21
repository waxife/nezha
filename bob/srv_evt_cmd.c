/**
 *  @file   srv_evt_cmd.c
 *  @brief
 *  $Id: srv_evt_cmd.c,v 1.4 2014/05/22 05:56:17 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.4 $
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

//#define SRV_DOORBELL_PROCESS
#ifdef	SRV_DOORBELL_PROCESS
#include "door_detect.h"
#else
#define DOOR_DETECT_GPIO	7
#define stop_rang_bell(x)
#define put_doorbell_event(x)
#endif

//#define TASK_USE_UI
#ifdef TASK_USE_UI
#include "show_task_ui.h"
#include "ui_draw.h"
#else
#define SHOW_THIS_OSD		0
#define CLEAR_THIS_OSD		1
#define show_ui_enable(x)
#define show_ui_disable(x)
#define show_pause_icon(x)
#define show_play_icon(x)
#define show_play_delete_ui(x)
#define show_play_copy_ui(x)
#define ui_normal_answer(x)
#define show_pview_copy_ui(x)
#define show_pview_delete_ui(x)
#define show_auplay_copy_ui(x)
#define show_auplay_delete_ui(x)
#define ui_srv_clock(x)
#define ui_srv_state(x)
#define srv_led_effect(x)
#define srv_led_off_effect(x)
#define ui_retrun_menu(x)
#define enable_sys_ui_update(x)
#define ui_md_pic_mask(x, y, w, h)
#endif

#ifdef TASK_USE_UI
extern int get_video_nread (void);
extern int get_photo_nread (void);
extern int get_aumsg_nread (void);
extern void set_video_filse (int no);
extern void set_photo_filse (int no);
extern void set_audio_filse (int no);
extern void ui_sys_clock (void);
extern void ui_sys_state (void);
extern unsigned char chk_pos2key (TW_EVENT_TOUCH* event);
extern void main_tag_led_effect (unsigned char btn, unsigned char mode);
extern void enable_sys_ui_update (int en);
extern void ui_retrun_menu (void);
extern void ui_md_pic_mask (int x, int y, int w, int h);
static void ui_srv_clock (void)
{
	ui_sys_clock();
}

static void ui_srv_state (int srv_id)
{
	switch(srv_id) {
	case ST_PLAYFILE:
		set_video_filse(get_video_nread());
		break;
	case ST_RECFILE:
		break;
	case ST_DECPIC:
		set_photo_filse(get_photo_nread());
		break;
	case ST_ENCPIC:
		break;
	case ST_DECAU:
		set_audio_filse(get_aumsg_nread());
		break;
	case ST_ENCAU:
		break;
//	case ST_SLIDE:
//		break;
	case ST_MDVID:
		break;
	}

	ui_sys_state();
}

static void srv_led_effect (unsigned char key)
{
	main_tag_led_effect(key, YELLOW_LED);	// UI LED ON
}

static void srv_led_off_effect (unsigned char key)
{
	main_tag_led_effect(key, LED_OFF);	// UI LED OFF
}
static unsigned char srv_chk_pos2key (TW_EVENT_TOUCH* event)
{
	return chk_pos2key(event);
}
#else
static unsigned char srv_chk_pos2key (TW_EVENT_TOUCH* event)
{
	return 0;
}
#endif

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

static void disp_play_normal (int std)
{
    ect_black_screen(FD_FAST,FD_NONE);
	display_set_play_lmod(std);
	ect_play_screen();
	show_ui_enable();
}

extern void get_pic_md_block (int *x, int *y, int *w, int *h);
static void disp_play_normal2 (int std)
{
	int x = 0, y = 0, w = 0, h = 0;

	ect_black_screen(FD_FAST,FD_NONE);
	display_set_play_lmod(std);
	ect_play_screen();

	sosd_spr_enable(LOAD_LUT);
	sosd_enable(DONT_LOAD);

	get_pic_md_block(&x, &y, &w, &h);
	if(w != 0 && h != 0) {
		ui_retrun_menu();
	} else {
		osd2_enable();
	}
	enable_sys_ui_update(1);
}
static void disp_play_full (int std)
{
	ect_black_screen(FD_FAST,FD_NONE);
	show_ui_disable();
	display_set_play_full(std);
	ect_play_screen();
}

static void disp_play_full2 (int std)
{
	int x = 0, y = 0, w = 0, h = 0;

	disp_play_full(std);

	get_pic_md_block(&x, &y, &w, &h);
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
		ui_md_pic_mask(x, y, w, h);
	}

	enable_sys_ui_update(0);
}

#define PLAY_NORMAL			0
#define PLAY_CHK_BACKUP		1
#define PLAY_CHK_DELETE		2
#define PLAY_DISP_FULL		3
static int play_task_state = PLAY_NORMAL;
static int play_srv_normal_key (unsigned char key)
{
	int cmd = 0;
	int state = 0;
	int std = 0;

	switch(key) {
	case KEY_CODE_1:
		get_play_srv_state(&state);
    	if(state == PLAY) {
    		cmd = CMD_PAUSE;
    		show_pause_icon(SHOW_THIS_OSD);
    	} else if(state == PAUSE) {
    	    cmd = CMD_PLAY;
    	    show_play_icon(SHOW_THIS_OSD);
    	}
		break;
	case KEY_CODE_2:
		cmd = CMD_PREVIOUS;
		break;
	case KEY_CODE_3:
		cmd = CMD_NEXT;
		break;
	case KEY_CODE_4:
//		/* must first stop play if backup (copy file) */
//		cmd = CMD_STOP;
//		play_task_state = PLAY_CHK_BACKUP;
//		show_play_copy_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_5:
		/* pause playback and change delete state for check deleting */
		cmd = CMD_PAUSE;
		play_task_state = PLAY_CHK_DELETE;
		show_play_delete_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_6:
		cmd = CMD_QUIT;
		break;
	case KEY_CODE_7:
		std = get_play_srv_std();
		if ((std == AVIN_CVBS_NTSC) || (std == AVIN_CVBS_PAL)) {
			play_task_state = PLAY_DISP_FULL;
			disp_play_full(std);	// display full mode
		}
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
//	case EVENT_CVD_NTSC:
//		cmd = CMD_CVD_NTSC;
//		break;
//	case EVENT_CVD_PAL:
//		cmd = CMD_CVD_PAL;
//		break;
//	case EVENT_CVD_LOST:
//		cmd = CMD_CVD_LOST;
//		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}
static int play_srv_chk_backup_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_USER_COPY;
		show_play_copy_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		cmd = CMD_PLAY;
		show_play_copy_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_play_copy_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	}

	return cmd;
}
static int play_srv_chk_delete_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_DELETE;
		show_play_delete_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		cmd = CMD_PLAY;
		show_play_delete_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_play_delete_ui(CLEAR_THIS_OSD);
		play_task_state = PLAY_NORMAL;
		break;
	}

	return cmd;
}
static int play_srv_disp_full_key (unsigned char key)
{
	int cmd = 0;
	int std = 0;

	switch(key) {
	case KEY_CODE_1:
	case KEY_CODE_2:
	case KEY_CODE_3:
	case KEY_CODE_4:
	case KEY_CODE_5:
	case KEY_CODE_6:
	case KEY_CODE_7:
		std = get_play_srv_std();
		if ((std == AVIN_CVBS_NTSC) || (std == AVIN_CVBS_PAL)) {
		
		    disp_play_normal(std);	// display L mode
		}
		play_task_state = PLAY_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		play_task_state = PLAY_NORMAL;
		break;
	}

	return cmd;
}
static int play_srv_process (unsigned char key)
{
	switch (play_task_state) {
	case PLAY_NORMAL:
		return play_srv_normal_key(key);
		break;
	case PLAY_CHK_BACKUP:
		return play_srv_chk_backup_key(key);
		break;
	case PLAY_CHK_DELETE:
		return play_srv_chk_delete_key(key);
		break;
	case PLAY_DISP_FULL:
		return play_srv_disp_full_key(key);
		break;
	default:
		ERROR("play task is unknown of state!\n");
		break;
	}

	return 0;	// no event
}

#define REC_NORMAL			0
#define REC_DOORBELL		1
static int rec_task_state = REC_NORMAL;
static int rec_srv_normal_key (unsigned char key)
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
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
	case EVENT_CVD_NTSC:
		cmd = CMD_CVD_NTSC;
		break;
	case EVENT_CVD_PAL:
		cmd = CMD_CVD_PAL;
		break;
	case EVENT_CVD_LOST:
		cmd = CMD_CVD_LOST;
		break;
	case EVENT_BELL_RUN:
		cmd = CMD_DOORBELL;
		rec_task_state = REC_DOORBELL;
		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}
static int rec_srv_door_key (unsigned char key)
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
		rec_task_state = REC_NORMAL;
		ui_normal_answer();
		stop_rang_bell();
		tw_set_chk_rbell(0);	// stop check ringbell of status
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
	case EVENT_CVD_NTSC:
		cmd = CMD_CVD_NTSC;
		break;
	case EVENT_CVD_PAL:
		cmd = CMD_CVD_PAL;
		break;
	case EVENT_CVD_LOST:
		cmd = CMD_CVD_LOST;
		break;
	case EVENT_BELL_STOP:
		rec_task_state = REC_NORMAL;
		ui_normal_answer();
		tw_set_chk_rbell(0);	// stop check ringbell of status
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}
static int rec_srv_process (unsigned char key)
{
	switch (rec_task_state) {
	case REC_NORMAL:
		return rec_srv_normal_key(key);
		break;
	case REC_DOORBELL:
		return rec_srv_door_key(key);
		break;
	default:
		ERROR("rec task is unknown of state!\n");
		break;
	}

	return 0;	// no event
}

#define PVIEW_NORMAL			0
#define PVIEW_CHK_BACKUP		1
#define PVIEW_CHK_DELETE		2
#define PVIEW_DISP_FULL			3
static int pview_task_state = PVIEW_NORMAL;
static int pview_srv_normal_key (unsigned char key)
{
	int cmd = 0;
	int std = 0;

	switch(key) {
	case KEY_CODE_1:
		break;
	case KEY_CODE_2:
		cmd = CMD_PREVIOUS;
		break;
	case KEY_CODE_3:
		cmd = CMD_NEXT;
		break;
	case KEY_CODE_4:
		/* must first stop view if backup (copy file) */
		//cmd = CMD_STOP;
		pview_task_state = PVIEW_CHK_BACKUP;
		show_pview_copy_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_5:
		/* pause picture view and change delete state for check deleting */
		//cmd = CMD_STOP;
		pview_task_state = PVIEW_CHK_DELETE;
		show_pview_delete_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_6:
		cmd = CMD_QUIT;
		break;
	case KEY_CODE_7:
		std = get_pview_srv_std();
		if ((std == AVIN_CVBS_NTSC) || (std == AVIN_CVBS_PAL)) {
			pview_task_state = PLAY_DISP_FULL;			
			disp_play_full2(std);	// display full mode
		}
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
//	case EVENT_CVD_NTSC:
//		cmd = CMD_CVD_NTSC;
//		break;
//	case EVENT_CVD_PAL:
//		cmd = CMD_CVD_PAL;
//		break;
//	case EVENT_CVD_LOST:
//		cmd = CMD_CVD_LOST;
//		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}
static int pview_srv_chk_backup_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_USER_COPY;
		show_pview_copy_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		//cmd = CMD_PLAY;
		show_pview_copy_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_pview_copy_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	}

	return cmd;
}
static int pview_srv_chk_delete_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_DELETE;
		show_pview_delete_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		//cmd = CMD_PLAY;
		show_pview_delete_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_pview_delete_ui(CLEAR_THIS_OSD);
		pview_task_state = PVIEW_NORMAL;
		break;
	}

	return cmd;
}
static int pview_srv_disp_full_key (unsigned char key)
{
	int cmd = 0;
	int std = 0;

	switch(key) {
	case KEY_CODE_1:
	case KEY_CODE_2:
	case KEY_CODE_3:
	case KEY_CODE_4:
	case KEY_CODE_5:
	case KEY_CODE_6:
	case KEY_CODE_7:
		std = get_pview_srv_std();
		if ((std == AVIN_CVBS_NTSC) || (std == AVIN_CVBS_PAL)) {
		
			disp_play_normal2(std);	// display L mode
		}
		pview_task_state = PVIEW_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		pview_task_state = PVIEW_NORMAL;
		break;
	}

	return cmd;
}
static int pview_srv_process (unsigned char key)
{
	switch (pview_task_state) {
	case PVIEW_NORMAL:
		return pview_srv_normal_key(key);
		break;
	case PVIEW_CHK_BACKUP:
		return pview_srv_chk_backup_key(key);
		break;
	case PVIEW_CHK_DELETE:
		return pview_srv_chk_delete_key(key);
		break;
	case PVIEW_DISP_FULL:
		return pview_srv_disp_full_key(key);
		break;
	default:
		ERROR("picture view task is unknown of state!\n");
		break;
	}

	return 0;	// no event
}

static int snap_srv_process (unsigned char key)
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
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
	case EVENT_CVD_NTSC:
		cmd = CMD_CVD_NTSC;
		break;
	case EVENT_CVD_PAL:
		cmd = CMD_CVD_PAL;
		break;
	case EVENT_CVD_LOST:
		cmd = CMD_CVD_LOST;
		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
}

#define AUPLAY_NORMAL			0
#define AUPLAY_CHK_BACKUP		1
#define AUPLAY_CHK_DELETE		2
static int auplay_task_state = PLAY_NORMAL;
static int auplay_srv_normal_key (unsigned char key)
{
//	int state = 0;
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		// no pause function in audio play
//		get_aumsg_srv_state(&state);
//    	if(state == PLAY) {
//    		cmd = CMD_PAUSE;
//    		show_audpause_icon(SHOW_THIS_OSD);
//    	} else {//if(state == PAUSE) {
//    	    cmd = CMD_PLAY;
//    	    show_audplay_icon(SHOW_THIS_OSD);
//    	}
    	break;
	case KEY_CODE_2:
		cmd = CMD_PREVIOUS;
		break;
	case KEY_CODE_3:
		cmd = CMD_NEXT;
		break;
	case KEY_CODE_4:
		/* must first stop play if backup (copy file) */
		cmd = CMD_STOP;
		auplay_task_state = AUPLAY_CHK_BACKUP;
		show_auplay_copy_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_5:
		/* pause audio play and change delete state for check deleting */
		cmd = CMD_PAUSE;
		auplay_task_state = AUPLAY_CHK_DELETE;
		show_auplay_delete_ui(SHOW_THIS_OSD);
		break;
	case KEY_CODE_6:
		cmd = CMD_QUIT;
		break;
	case KEY_CODE_7:
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
static int auplay_srv_chk_backup_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_USER_COPY;
		show_auplay_copy_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		cmd = CMD_PLAY;
		show_auplay_copy_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_auplay_copy_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	}

	return cmd;
}
static int auplay_srv_chk_delete_key (unsigned char key)
{
	int cmd = 0;

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_DELETE;
		show_auplay_delete_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	case KEY_CODE_2:
	case KEY_CODE_6:
		cmd = CMD_PLAY;
		show_auplay_delete_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		show_auplay_delete_ui(CLEAR_THIS_OSD);
		auplay_task_state = AUPLAY_NORMAL;
		break;
	}

	return cmd;
}
static int auplay_srv_process (unsigned char key)
{
	switch (auplay_task_state) {
	case AUPLAY_NORMAL:
		return auplay_srv_normal_key(key);
		break;
	case AUPLAY_CHK_BACKUP:
		return auplay_srv_chk_backup_key(key);
		break;
	case AUPLAY_CHK_DELETE:
		return auplay_srv_chk_delete_key(key);
		break;
	default:
		ERROR("audio play task is unknown of state!\n");
		break;
	}

	return 0;	// no event
}

static int aumsg_srv_process(unsigned char key)
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
	case EVENT_DOOR_BELL:
		cmd = CMD_QUIT;
		break;
	case EVENT_CVD_NTSC:
		cmd = CMD_CVD_NTSC;
		break;
	case EVENT_CVD_PAL:
		cmd = CMD_CVD_PAL;
		break;
	case EVENT_CVD_LOST:
		cmd = CMD_CVD_LOST;
		break;
	case EVENT_ERR_TIMEOUT:
		cmd = CMD_QUIT;
		break;
	}

	return cmd;
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

static int lsf_disp_mode = 0;
static int lsf_srv_process(unsigned char key)
{
	int cmd = 0;

	if(lsf_disp_mode) {
		lsf_disp_mode = 0;
		if(key >= 1 && key <= 7) {
			cmd = CMD_STOP;
		} else if (key == EVENT_DOOR_BELL) {
			cmd = CMD_QUIT;
		} else {
			lsf_disp_mode = 1;
			return cmd;
		}
	}

	switch(key) {
	case KEY_CODE_1:
		cmd = CMD_PLAY;
		lsf_disp_mode = 1;
		break;
	case KEY_CODE_2:
		cmd = CMD_PREVIOUS;
		break;
	case KEY_CODE_3:
		cmd = CMD_NEXT;
		break;
	case KEY_CODE_4:
		cmd = CMD_USER_COPY;
		break;
	case KEY_CODE_5:
		cmd = CMD_DELETE;
		break;
	case KEY_CODE_6:
	case KEY_CODE_7:
		cmd = CMD_QUIT;
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
		return play_srv_process(key);
		break;
	case ST_RECFILE:
		return rec_srv_process(key);
		break;
	case ST_DECPIC:
		return pview_srv_process(key);
		break;
	case ST_ENCPIC:
		return snap_srv_process(key);
		break;
	case ST_DECAU:
		return auplay_srv_process(key);
		break;
	case ST_ENCAU:
		return aumsg_srv_process(key);
		break;
//	case ST_SLIDE:
//		return slide_srv_process(key);
//		break;
	case ST_MDVID:
		return md_srv_process(key);
		break;
	case ST_LSF:
		return lsf_srv_process(key);
		break;
	}

	return 0;
}

/**
 * @func    srv_key_process
 * @brief   parse to task command when any event occurred
 * @param   key_code	key code
 * 			srv_id		current active of service ID
 * @return  parsed of command
 */
static int srv_key_process(unsigned char key_code, int srv_id)
{
	int rc = 0;
	if((key_code >= KEY_CODE_1) && (key_code <= KEY_CODE_7)) {
		// dbg(2, ">>> KEY %d\n\r", key_code);

		srv_led_effect(key_code);	// UI LED ON
		rc = srv_event_parse(key_code, srv_id);
		srv_led_off_effect(key_code);		// UI LED OFF
		switch(key_code)
		{
			case 3:
			rc = CMD_NEXT;
			break;
			case 2:
			rc = CMD_PREVIOUS;
			break;
			case 5:
			rc = CMD_QUIT;
			break;
			case 1:
			rc = CMD_DELETE;
			default:
			break;
		}
	}

	return rc;
}


static int tp_prev_key2 = 0;
/**
 * @func    srv_touch_process
 * @brief   parse to key event when any touch event occurred
 * @param   event	touch event of sturcture
 * 			srv_id	current active of service ID
 * @return  parsed of event
 */
static int srv_touch_process (TW_EVENT* event, int srv_id)
{
	int key = 0;
	int rc = 0;

	key = srv_chk_pos2key((TW_EVENT_TOUCH *)event);

    if(event->type == TW_EVENT_TYPE_TOUCH_DOWN) {
    	tp_prev_key2 = key;
    	srv_led_effect(key);
    } else if (event->type == TW_EVENT_TYPE_TOUCH_RELEASE
    	|| event->type == TW_EVENT_TYPE_TOUCH_REPEAT) {
    	if(key == tp_prev_key2) {
    		/* touch down of key is same with touch release */
    		rc = srv_key_process(key, srv_id);
    	} else {
    		/* touch down of key is different with touch release, and clear UI LED */
    		srv_led_off_effect(key);
    	}
    	if(event->type == TW_EVENT_TYPE_TOUCH_RELEASE)
    		tp_prev_key2 = 0;
    }

    return rc;
}

/**
 * @func    srv_ir_process
 * @brief   parse to key event when any IR event occurred
 * @param   ircode	IR code
 * 			srv_id	current active of service ID
 * @return  parsed of event
 */
static int srv_ir_process (unsigned char ircode, int srv_id)
{
	switch(ircode) {
	case IR_SLIDE_CODE:		// Key 1
		return srv_key_process(KEY_CODE_1, srv_id);
		break;
	case IR_PLAY_CODE:		// Key 2
		return srv_key_process(KEY_CODE_2, srv_id);
		break;
	case IR_BACK_CODE:		// Key 3
		return srv_key_process(KEY_CODE_3, srv_id);
		break;
	case IR_ZOOM_CODE:		// Key 4
		return srv_key_process(KEY_CODE_4, srv_id);
		break;
	case IR_SELECT_CODE:	// Key 5
		return srv_key_process(KEY_CODE_5, srv_id);
		break;
	case IR_ROTATE_CODE:	// Key 6
		return srv_key_process(KEY_CODE_6, srv_id);
		break;
	case IR_SUB_CODE:		// Key 7
		return srv_key_process(KEY_CODE_7, srv_id);
		break;
	case IR_MUTE_CODE:		// Key 8
//		return srv_key_process(EVENT_DOOR_BELL, srv_id);
    	/* return main loop to process door bell of event */
    	clear_event();		// for only process door bell of event
    	put_doorbell_event();
    	return srv_event_parse(EVENT_DOOR_BELL, srv_id);
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
#if 0
	case TW_EVENT_TYPE_TOUCH_DOWN:
	case TW_EVENT_TYPE_TOUCH_MOTION:
	case TW_EVENT_TYPE_TOUCH_RELEASE:
	case TW_EVENT_TYPE_TOUCH_REPEAT:
		dbg(2, ">>>>>> Touch Event, status = %d\n\r", event->type);
		dbg(2, "x = %d, y = %d\n\r", event->touch_panel.x, event->touch_panel.y);

		return srv_touch_process(event, srv_id);
		break;
	case TW_EVENT_TYPE_IR_DOWN:
		dbg(2, ">>>>>> IR Event\n\r");
		dbg(2, "IR Code = 0x%02X\n\r", event->ircode.ch);

		return srv_ir_process(event->ircode.ch, srv_id);
		break;
#endif
	case TW_EVENT_TYPE_KEY_DOWN:
		return srv_key_process(event->keystroke.ch, srv_id);
		break;
#if 0
	case TW_EVENT_TYPE_SIGNAL:
		dbg(2, ">>>>>> Signal Event\n\r");

		return srv_signal_process(event->signal.std, srv_id);
		break;
	case TW_EVENT_TYPE_GPIO:
        dbg(2, ">>>>>> GPIO Event\n\r");
        if(event->gpios.gid == DOOR_DETECT_GPIO) {
        	/* return main loop to process door bell of event */
        	clear_event();		// for only process door bell of event
        	put_doorbell_event();
        	return srv_event_parse(EVENT_DOOR_BELL, srv_id);
		}
        break;
	case TW_EVENT_TYPE_BELL:
        dbg(2, ">>>>>> Ring Bell Event\n\r");
        dbg(2, "ring bell of status = 0x%02X\n\r", event->rbell.status);
        if(event->rbell.status == ringbell_idle ) {
        	if(srv_id == ST_RECFILE)
        		return srv_event_parse(EVENT_BELL_STOP, srv_id);
        	else
        		tw_set_chk_rbell(0);
		} else if (event->rbell.status == ringbell_play) {
        	if(srv_id == ST_RECFILE)
        		return srv_event_parse(EVENT_BELL_RUN, srv_id);
        	else
        		tw_set_chk_rbell(0);
		}
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
#endif
	default:	/* no command */
		/* show system of clock and date */
		ui_srv_clock();
		ui_srv_state(srv_id);
		break;
	}

	return 0;
}
