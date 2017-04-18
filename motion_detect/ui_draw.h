/**
 *  @file   ui_draw.h
 *  @brief
 *  $Id: ui_draw.h,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/24  Ken 	New file.
 *
 */


#ifndef UI_DRAW_H_
#define UI_DRAW_H_

#define _RTC_WDAY_X 7
#define _RTC_WDAY_Y 18

#define _RTC_HOUR_X (_RTC_WDAY_X+(8*2)+3+5)
#define _RTC_HOUR_Y 18

#define _SET_YEAR_X 4
#define _SET_MON_X (_SET_YEAR_X+9)
#define _SET_DAY_X (_SET_MON_X+5)
#define _SET_YEAR_Y 8


#define _SET_HOUR_X (_SET_YEAR_X+(8*2)+3)
#define _SET_MIN_X  (_SET_HOUR_X+5)
#define _SET_SEC_X  (_SET_MIN_X+5)
#define _SET_HOUR_Y 8


#define _SETCLK_ARR_YEAR_X   (_SET_YEAR_X+3)
#define _SETCLK_ARR_MON_X    (_SETCLK_ARR_YEAR_X+7)
#define _SETCLK_ARR_DAY_X    (_SETCLK_ARR_MON_X+5)
#define _SETCLK_ARR_HOUR_X   (_SETCLK_ARR_DAY_X+5)
#define _SETCLK_ARR_MIN_X    (_SETCLK_ARR_HOUR_X+5)
#define _SETCLK_ARR_SEC_X    (_SETCLK_ARR_MIN_X+5)

#define _TOP_INFO_X 2
#define _TOP_INFO_Y 2
#define _TOP_INFO_LEN 37

enum MENU_TAG_IDX {
	MAIN_MENU_TAG_1 = 0,
	MAIN_MENU_TAG_2,
	MAIN_MENU_TAG_3,

	PICVIEW_MENU_TAG,
	PLAYBACK_MENU_TAG,
	LISTEN_MENU_TAG,
	SET_CLK_MENU_TAG,
	IMG_ADJ_MENU_TAG,

	SETTING_MENU_TAG,

	CHECK_MENU_TAG,

	MD_MENU_TAG1,
	MD_MENU_TAG2,

	TAG_TOTAL_NUM
};

enum UI_BK_TYPE {
	BK_VIDEO_TYPE = 0,
	BK_MENU_TYPE,
	BK_IMG_TYPE,
	BK_TOTAL_TYPE
};
#define BACKGROUND_TAOTAL_NUM	4

enum UI_LED_MODE {
	LED_OFF = 0,
	LED_ALL_OFF,
	YELLOW_LED,
	BLUE_LED,
	RED_LED
};

enum FILE_HEADER_TYPE {
	FILE_HEADER_CLEAR = 0,
	FILE_HEADER_NO,
	FILE_HEADER_ALL,
	FILE_HEADER_MIN,
	FILE_HEADER_SEC
};

extern const int set_clk_arr_x[];


extern int ui_get_disp_bk_idx (void);
extern void draw_top_info(char *str, int type);
extern void ui_disp_background (int type);
extern void main_tag_led_effect (unsigned char btn, unsigned char on);
extern void btn_down_effect (int key, int en);
extern void menu_tag2_state (int clear);
extern void fresh_wday (void);
extern void draw_str(char *str, int x, int y, int index, unsigned char color);
extern void draw_osd_num(struct tw_icon* picon,int num, int dig, int x, int y);
extern void draw_set_arr(int x);
extern void draw_set_clk(int clr);
extern void clear_set_arr(int x);
extern void draw_rtc(int flush_date);
extern void ui_draw_menu_tag (int tag);
extern void draw_new_video (int total);
extern void draw_new_photo (int total);
extern void draw_new_audmsg (int total);
extern void ui_call_answer (void);
extern void ui_normal_answer (void);


#endif /* UI_DRAW_H_ */
