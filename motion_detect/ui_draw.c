/**
 *  @file   ui_draw.c
 *  @brief
 *  $Id: ui_draw.c,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/24  Ken 	New file.
 *
 */

#include <stdio.h>
#include <time.h>
#include <debug.h>
#include "tw_widget_sys.h"
#include "../drivers/osd2/osd2.h"
#include "./drivers/spiosd/spiosd.h"
#include "reg_tw.h"
#include "rc.h"

#include "OSD/page_video_view.h"

#include "keymap.h"

#include "ui_draw.h"

const struct tw_spr* const tag_spr[TAG_TOTAL_NUM] =
{
    &page_video_view_tag1,
    &page_video_view_tag2,
    &page_video_view_tag3,

    &page_video_view_photo,
    &page_video_view_video,
    &page_video_view_msg,
    &page_video_view_clk,
    &page_video_view_img_ctrl,

    &page_video_view_setting,

    &page_video_view_check,
};

void ui_draw_menu_tag (int tag)
{
	tw_spr_draw((struct tw_spr *)tag_spr[tag]);
	sosd_spr_enable(LOAD_LUT);
}

void ui_draw_md_ctrl (int idx)
{
	struct tw_spr spr;
	struct res_spr_t spr_s;

	if(idx == MD_MENU_TAG1)
		spr = page_video_view_mdctrl1;
	else if(idx == MD_MENU_TAG2)
		spr = page_video_view_mdctrl2;
	else
		return;

	if(get_res_spr(spr.sprID, &spr_s)) {
		ERROR("get sprite of structure is fail\n");
		return;
	}
	sosd_spr_disable();
	sosd_hvoffset_init (0x11E, 0x2B);
	sosd_sp_hvoffset_init(0x9B, 0x29);
	_sosd_pLUT_addr(spr_s.lut_addr);
	_sosd_img_addr(spr_s.index_addr);
	_sosd_img_ls(spr_s.line_jump);
	_sosd_img_window(spr.width, spr.height);
	_sosd_img_loca(spr.x, spr.y);
	sosd_enable(LOAD_LUT);
}

const struct tw_gicon* const wday[] =
{
    &page_video_view_week_6,    /* Sunday */
    &page_video_view_week_0,    /* Monday */
    &page_video_view_week_1,
    &page_video_view_week_2,
    &page_video_view_week_3,
    &page_video_view_week_4,
    &page_video_view_week_5,    /* Saturday */
};


/* OSD2 LED of Y position */
#define MAIN_TAG_LED_1_Y		1
#define MAIN_TAG_LED_2_Y		5
#define MAIN_TAG_LED_3_Y		8
#define MAIN_TAG_LED_4_Y		12
#define MAIN_TAG_LED_5_Y		15
const int led_y_positions[] = {
	MAIN_TAG_LED_1_Y, MAIN_TAG_LED_2_Y, MAIN_TAG_LED_3_Y,
	MAIN_TAG_LED_4_Y, MAIN_TAG_LED_5_Y
};
void main_tag_led_effect (unsigned char btn, unsigned char mode)
{
    struct tw_gicon gicon;
    struct tw_icon led_icon;
	int blanking = 0;

	switch (mode) {
		case LED_OFF:
			if(btn%2)
				gicon = page_video_view_led_yellow_0;
			else
				gicon = page_video_view_led_yellow_1;
			blanking = 0;
			break;
		case LED_ALL_OFF:
				gicon = page_video_view_led_yellow_0;
			blanking = 0;
			break;
		case YELLOW_LED:
			if(btn%2)
				gicon = page_video_view_led_yellow_0;
			else
				gicon = page_video_view_led_yellow_1;
			blanking = 0;
			break;
		case RED_LED:
			if(btn%2)
				gicon = page_video_view_led_red_0;
			else
				gicon = page_video_view_led_red_1;
			blanking = 1;
			break;
		case BLUE_LED:
			if(btn%2)
				led_icon = page_video_view_led_blue_0;
			else
				led_icon = page_video_view_led_blue_1;
			blanking = 1;
			break;
		default:
			return;
			break;
	}

	if((btn <= 0) || (btn > KEY_CODE_5))
		if(mode != LED_ALL_OFF)
			return;

	if (mode == LED_ALL_OFF) {
		int i = 0;
		for(i = 0; i < 5; i++)
			tw_osd2_clear_block(gicon.x, led_y_positions[i], gicon.width, gicon.height);
	} else if (mode == LED_OFF) {
	    tw_osd2_clear_block(gicon.x, led_y_positions[btn-1], gicon.width, gicon.height);
	} else if (mode == BLUE_LED) {
		led_icon.y = led_y_positions[btn-1];
		tw_icon_color_change(&led_icon, led_icon.color, blanking);
	} else {
		_n_tw_gicon_loc(&gicon, gicon.x, led_y_positions[btn-1], blanking);
		osd2_update();
	}
}

#define BTN_EFFECT_NUM	5
#define BTN_LOC_DIFF	86
void btn_down_effect (int key, int en)
{
    struct tw_spr spr;

    if(!en) {
    	// disable effect
    	sosd_spr_disable();
    	return;
    }

    key -= 1;
    if(key >= BTN_EFFECT_NUM)
    	return;

    spr = page_video_view_btn_effct;
    spr.y = key * BTN_LOC_DIFF;


	tw_spr_draw((struct tw_spr *)&spr);
	sosd_spr_enable(LOAD_LUT);
}


void draw_wday(struct tm* ptime, int x, int y)
{
//  struct tw_icon week = page_video_view_week;
//
//  week.x = x;
//  week.y = y;
//  week.height = 1;
//  week.index += (unsigned short)((ptime->wday)*5);
//  _n_tw_icon_draw(&week);
    _n_tw_gicon_draw ((struct tw_gicon*)wday[ptime->tm_wday]);
}

void draw_osd_num(struct tw_icon* picon,int num, int dig, int x, int y)
{
    int snum, i;
    unsigned short index;
    //unsigned char color= 0;

    if (dig >=0) {
        for (index=1,i=1;index<dig;index++) {
            i*=10;
        }
        dig = i;
    }else if( dig < 0 ){
        return;
    }

    for(i=dig;i>=1;i/=10){
        snum = num/i;
        index = (unsigned short)(picon->index+(snum*picon->width*picon->height));
        //color = check_color_config(picon->index, 0, picon->color);
        _icon_draw(index, picon->color, x, y, picon->width, picon->height, 0);
        num -= (snum*i);
        x+=picon->width;
    }
}

void draw_date(struct tm* ptime, struct tw_icon* pnum, int symbol_idx, int x, int y)
{
    draw_osd_num(pnum, ptime->tm_year + 1970, 4, x, y);
    x+=4*pnum->width;

    _icon_draw (pnum->index+symbol_idx, pnum->color, x++, y, 1, pnum->height, 0);

    draw_osd_num(pnum, ptime->tm_mon + 1, 2, x, y);
    x+=2*pnum->width;

    _icon_draw (pnum->index+symbol_idx, pnum->color, x++, y, 1, pnum->height, 0);

    draw_osd_num(pnum,ptime->tm_mday, 2, x, y);

    return;
}

void draw_time(struct tm* ptime, struct tw_icon* pnum, int symbol_idx, int x, int y, int draw_sec)
{
    draw_osd_num(pnum,ptime->tm_hour, 2, x, y);
    x+=2*pnum->width;

    _icon_draw (pnum->index+symbol_idx, pnum->color, x++, y, 1, pnum->height, 0);

    draw_osd_num(pnum,ptime->tm_min, 2, x, y);
    x+=2*pnum->width;

    if (draw_sec) {
        _icon_draw (pnum->index+symbol_idx, pnum->color, x++, y, 1, pnum->height, 0);

        draw_osd_num(pnum,ptime->tm_sec, 2, x, y);
        x+=2*pnum->width;
    }
}

int CaculateWeekDay(int year, int month, int day)
{
	int week;
#if 0
 	if (1 == month) {
		month = 13;
		year--;
	}
	if (2 == month) {
		month = 14;
		year--;
	}

	week=(day+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;
#else
    struct tm t, *tm;
    time_t l;
    t.tm_year = year - 1970;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = t.tm_min = t.tm_sec = 0;
    l = mktime(&t);
    tm = localtime(&l);
    week = tm->tm_wday;

#endif
	return week;
}

#define top_date_x (_TOP_INFO_X+6)
#define top_time_x (top_date_x+11)

void clr_top_info()
{
    tw_osd2_clear_block (_TOP_INFO_X, _TOP_INFO_Y, _TOP_INFO_LEN, 1);
    return;
}

#define _SPACE_ICON page_video_view_mask_font

void draw_top_char(struct tw_icon* picon, char data, int x, int y)
{
    int idx;

    if (data == '/') {
        idx = 12;
    }else if(data == '-'){
        idx = 11;
    }else if(data == ':'){
        idx = 10;
    }else if(data == ' '){
        _icon_draw (_SPACE_ICON.index, _SPACE_ICON.color, x, y, _SPACE_ICON.width, _SPACE_ICON.height, 0);
        return;
    }
    else{
        idx = data - '0';
    }

    _icon_draw (picon->index+idx, picon->color, x, y, picon->width, picon->height, 0);
}

void put_font_index(int pindex, unsigned char *str, unsigned char *font_table, int length)
{
    int idx = 0;
    int i = 0;

    for(i = 0; i < length; i++) {
        if (str[i] == '/') {
            idx = 12;
        } else if(str[i]  == '-') {
            idx = 11;
        } else if(str[i]  == ':') {
            idx = 10;
        } else if(str[i]  == ' ') {
//        	_icon_draw (_SPACE_ICON.index, _SPACE_ICON.color, x, y, _SPACE_ICON.width, _SPACE_ICON.height, 0);
//        	return;
        	font_table[i] = 0;
        	continue;
        }
        else{
            idx = str[i] - '0';
        }
    	font_table[i] = pindex + idx;
    }
}

void draw_str(char *str, int x, int y, int index, unsigned char color)
{
    unsigned char font_table[10] = { 0 };
    int length = 0;
    for (length=0;length<10;length++) {
        if(str[length] == 0)
            break;
    }

    put_font_index(index, str, font_table, length);
    _osd2_cq_burst2(y, x, length, 1, color<<1, font_table);
}

void draw_top_info(char *str, int type)
{
    struct tw_icon icon_num;
    int x = 0;
    int length = 0;
    unsigned char font_table[19] = { 0 };

    icon_num.index = page_video_view_num.index;
    icon_num.color = page_video_view_num.color;

    icon_num.width = 1;
    icon_num.height = 1;

    if (str == NULL) {
        clr_top_info();
        return;
    }

    switch(type) {
    case FILE_HEADER_CLEAR:
    	clr_top_info();
		return;
    	break;
    case FILE_HEADER_NO:
    	x = _TOP_INFO_X;
    	length = 9;
    	tw_osd2_menu_write_block (_TOP_INFO_X, _TOP_INFO_Y, _TOP_INFO_LEN, 1,
    		page_video_view_mask_font.index, page_video_view_mask_font.color<<1);
    	break;
    case FILE_HEADER_ALL:
    	x = _TOP_INFO_X + 10;
    	length = 19;
    	break;
    case FILE_HEADER_MIN:
    	x = _TOP_INFO_X + 10 + 14;
    	length = 5;
    	break;
    case FILE_HEADER_SEC:
    	x = _TOP_INFO_X + 10 + 17;
    	length = 2;
    	break;
    }

    put_font_index(icon_num.index, str, font_table, length);

    _osd2_cq_burst2(_TOP_INFO_Y, x, length, 1, icon_num.color<<1, font_table);
    osd2_update();
}

void draw_osd_num_line (struct tw_icon* pnum, int value, int dig, int x, int w, int line)
{
    int i = 0;
    int j = 0;
    int num = 0;
    int tmp = 1;
    int tmp2 = 0;
    unsigned short 	index = 0;
    unsigned char 	color = 0;
    unsigned short 	wdata = 0;

    for(i= 0; i < (dig-1); i++)
    	tmp *= 10;

    color = check_color_config(pnum->index, 0, pnum->color);
    color <<= 1;
    for(i = 0, j = 1; i < dig; i++, j*=10) {
    	if(j >= tmp)
    		num = (value%10);
    	else {
    		num = (value/(tmp/j));
    		tmp2 = num/10;
    		num -= (tmp2*10);
    	}
		index = (unsigned short)(pnum->index + (num * pnum->width * pnum->height)
			+ (line? pnum->width : 0));
		wdata = ((color << 8) | (index & 0x01FF));
		_osd2_cq_line_data(x, w, wdata);
		_osd2_cq_line_data((x+1), w, (wdata+1));
    }
}

void draw_osd_symbol_line(struct tw_icon* pnum, int idx, int x, int w, int line)
{
    unsigned short 	index = 0;
    unsigned char 	color  = 0;
    unsigned short 	wdata = 0;

    color = check_color_config(pnum->index, 0, pnum->color);
    color <<= 1;
    if(idx == 40) {
    	// blinking
    	color |= 0x20;
    }
	index = (unsigned short)(pnum->index + idx + (line? 1 : 0));
	wdata = ((color << 8) | (index & 0x01FF));
	_osd2_cq_line_data(x, w, wdata);
}

void draw_year_line (struct tm* ptime, struct tw_icon* pnum, int x, int y, int w, int line)
{
	// year
	draw_osd_num_line(pnum, ptime->tm_year + 1970, 4, x, w, line);
	// dash
	draw_osd_symbol_line(pnum, 42, x+8, w, line);
	// mon
	draw_osd_num_line(pnum, ptime->tm_mon + 1, 2, x+9, w, line);
	// dash
	draw_osd_symbol_line(pnum, 42, x+13, w, line);
	// date
	draw_osd_num_line(pnum, ptime->tm_mday, 2, x+14, w, line);
	// space
	_osd2_cq_line_data(x+18, w, 0);
}

void draw_hour_line (struct tm* ptime, struct tw_icon* pnum, int x, int y, int w, int line)
{
	// hour
	draw_osd_num_line(pnum, ptime->tm_hour, 2, x+19, w, line);
	// semi
	draw_osd_symbol_line(pnum, 40, x+23, w, 0);
}

void draw_min_line (struct tm* ptime, struct tw_icon* pnum, int x, int y, int w, int line)
{
	// min
	draw_osd_num_line(pnum, ptime->tm_min, 2, x+24, w, line);
}

static int pre_date = 0;
static int pre_hour = 0;
static int pre_min = 0;
static int pre_wday = 0;
void draw_rtc (int flush_date)
{
    struct tm time;
    //struct tw_icon icon_num = page_video_view_num;
    struct tw_icon icon_num = page_video_view_big_num_0;

    rtc_get_time(&time);

    if ((pre_date != time.tm_mday)|| flush_date) {
    	// time.wday = CaculateWeekDay(time.year, time.mon, time.date);
        pre_wday = time.tm_wday;
        //printf("wday = %d\n", pre_wday);
    	draw_wday(&time, _RTC_WDAY_X, _RTC_WDAY_Y + 1);

    	_osd2_cq_line_config(_RTC_WDAY_Y, _RTC_WDAY_X+5, 28);
    	draw_year_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 19, 0);
    	draw_hour_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 5, 0);
    	draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 4, 0);
    	_osd2_cq_line_config(_RTC_WDAY_Y+1, _RTC_WDAY_X+5, 28);
		draw_year_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 19, 1);
		draw_hour_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 5, 1);
		draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 4, 1);
		osd2_update();
    } else if ((pre_hour != time.tm_hour)|| flush_date) {
    	_osd2_cq_line_config(_RTC_WDAY_Y, _RTC_WDAY_X+5+19, 9);
    	draw_hour_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 5, 0);
    	draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 4, 0);
    	_osd2_cq_line_config(_RTC_WDAY_Y+1, _RTC_WDAY_X+5+19, 9);
		draw_hour_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 5, 1);
		draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 4, 1);
		osd2_update();
    } else if ((pre_min != time.tm_min)|| flush_date) {
    	_osd2_cq_line_config(_RTC_WDAY_Y, _RTC_WDAY_X+5+24, 4);
    	draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y, 4, 0);
    	_osd2_cq_line_config(_RTC_WDAY_Y+1, _RTC_WDAY_X+5+24, 4);
		draw_min_line(&time, &icon_num, _RTC_WDAY_X+5, _RTC_WDAY_Y+1, 4, 1);
		osd2_update();
    }

    pre_date = time.tm_mday;
    pre_hour = time.tm_hour;
    pre_min = time.tm_min;

//    time.wday = CaculateWeekDay(time.year, time.mon, time.date);
//
//    icon_num.width = 2;
//    //icon_num.height = 1;
//
//    if (flush_date) {
//        draw_wday(&time, _RTC_WDAY_X, _RTC_WDAY_Y + 1);
//        draw_date(&time, &icon_num, 42, _RTC_WDAY_X+5, _RTC_WDAY_Y);
//    }
//    draw_time(&time, &icon_num, 40, _RTC_HOUR_X, _RTC_HOUR_Y, 0);
}

void fresh_wday (void)
{
    _n_tw_gicon_load ((struct tw_gicon*)wday[pre_wday]);
}

const int set_clk_arr_x[] = {
    _SETCLK_ARR_YEAR_X, _SETCLK_ARR_MON_X, _SETCLK_ARR_DAY_X,
    _SETCLK_ARR_HOUR_X, _SETCLK_ARR_MIN_X, _SETCLK_ARR_SEC_X,
};

void clear_set_arr(int x)
{
    tw_osd2_clear_block (x, page_video_view_arr_down.y+2,
                         page_video_view_arr_down.width, 4);

    tw_osd2_clear_block (x, page_video_view_arr_up.y+2,
                         page_video_view_arr_up.width, page_video_view_arr_up.height);
}

void draw_set_arr(int x)
{
    _n_tw_icon_loc((struct tw_icon *)&page_video_view_arr_down, x, page_video_view_arr_down.y+2);
    _n_tw_icon_loc((struct tw_icon*)&page_video_view_arr_up, x, page_video_view_arr_up.y+2);

    osd2_update();
}

void draw_set_clk(int clr)
{
    struct tm time;
    struct tw_icon icon_num = page_video_view_big_num_0;


    if(clr){
        tw_osd2_clear_block (_SET_YEAR_X, 6, 33, 6);
        return;
    }
    rtc_get_time(&time);

    icon_num.color |= 0x20;

    icon_num.width = 2;
    icon_num.height = 2;

    draw_date(&time, &icon_num, 42, _SET_YEAR_X, _SET_YEAR_Y);
    draw_time(&time, &icon_num, 40, _SET_HOUR_X, _SET_HOUR_Y, 1);

    draw_set_arr(set_clk_arr_x[0]);
}

void draw_adj_clk(struct tm *ptime)
{
    struct tw_icon icon_num = page_video_view_big_num_0;

    icon_num.color |= 0x20;

    icon_num.width = 2;
    icon_num.height = 2;

    draw_date(ptime, &icon_num, 42, _SET_YEAR_X, _SET_YEAR_Y);
    draw_time(ptime, &icon_num, 40, _SET_HOUR_X, _SET_HOUR_Y, 1);

    //draw_set_arr(set_clk_arr_x[0]);
}

static void draw_file_num (int x, int num)
{
	int tmp = 0;

	_icon_draw (page_video_view_num.index+(num/1000), page_video_view_num.color,
		x, _TOP_INFO_Y-1, 1, 1, 1);
	tmp = (num/100) - ((num/1000)*10);
	_icon_draw (page_video_view_num.index+tmp, page_video_view_num.color,
		x+1, _TOP_INFO_Y-1, 1, 1, 1);
	tmp = (num/10) - (tmp*10) - ((num/1000)*100);
	_icon_draw (page_video_view_num.index+tmp, page_video_view_num.color,
		x+2, _TOP_INFO_Y-1, 1, 1, 1);
	_icon_draw (page_video_view_num.index+(num%10), page_video_view_num.color,
		x+3, _TOP_INFO_Y-1, 1, 1, 1);
}

void draw_new_video (int total)
{
	if(total <= 0) {
		// clear
		tw_osd2_clear_block (21, _TOP_INFO_Y-1, 6, 1);
	} else if (total >= 0) {
		if(total > 9999)
			total = 9999;
		_icon_draw (page_video_view_video1.index, page_video_view_video1.color,
			21, _TOP_INFO_Y-1, page_video_view_video1.width, page_video_view_video1.height, 0);
		draw_file_num(23, total);
		osd2_update();
	}
}

void draw_new_photo (int total)
{
	if(total <= 0) {
		// clear
		tw_osd2_clear_block (27, _TOP_INFO_Y-1, 6, 1);
	} else {
		if(total > 9999)
			total = 9999;
		_icon_draw (page_video_view_photo1.index, page_video_view_photo1.color,
			27, _TOP_INFO_Y-1, page_video_view_photo1.width, page_video_view_photo1.height, 0);
		draw_file_num(29, total);
		osd2_update();
	}
}

void draw_new_audmsg (int total)
{
	if(total <= 0) {
		// clear
		tw_osd2_clear_block (33, _TOP_INFO_Y-1, 6, 1);
	} else {
		if(total > 9999)
			total = 9999;
		_icon_draw (page_video_view_mag.index, page_video_view_mag.color,
			33, _TOP_INFO_Y-1, page_video_view_mag.width, page_video_view_mag.height, 0);
		draw_file_num(35, total);
		osd2_update();
	}
}

void ui_call_answer (void)
{
	tw_spr_draw((struct tw_spr *)&page_video_view_tag1_phone);
	sosd_spr_enable(LOAD_LUT);
}

void ui_normal_answer (void)
{
	tw_spr_draw((struct tw_spr *)&page_video_view_tag1);
	sosd_spr_enable(LOAD_LUT);
}

#define MD_FONT_WIDTH	16
#define MD_FONT_HEIGHT	8
#define MD_DISP_WIDTH	720
#define MD_DISP_HEIGHT	480
#define MD_MENU_WIDTH	(MD_DISP_WIDTH/MD_FONT_WIDTH)
#define MD_MENU_HEIGHT	(MD_DISP_HEIGHT/MD_FONT_HEIGHT)
#define MD_1BP_FSIZE	(MD_FONT_WIDTH*MD_FONT_HEIGHT)/(16) //WORD
#define MD_MENU_ADDR	(4*MD_1BP_FSIZE)
#define MD_MENU_SIZE	((MD_MENU_WIDTH + 4)*MD_MENU_HEIGHT)
#define MD_RATTH		0x65
#define MD_RATTV		0x01
#if 0
static void
fill_font (unsigned short dat)
{
	int i = 0;

	/* write data to OSDRAM through data port and auto inc.  */
	for(i = 0; i < MD_1BP_FSIZE; i++) {
		/* word access, LSB first, then MSB byte */
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, (unsigned char)(dat & 0xFF));
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, (unsigned char)(dat >> 8));
	}
}
#endif
static void
load_md_fonts (void)
{
	unsigned short addr = 0;

	osd2_oram_fill(addr, MD_1BP_FSIZE, 0x0000);
	addr += MD_1BP_FSIZE;
	osd2_oram_fill(addr, MD_1BP_FSIZE, 0xFFFF);
	addr += MD_1BP_FSIZE;
	osd2_oram_fill(addr, MD_1BP_FSIZE, 0xFF00);
}

#define	MD_MENU_SIZE_C(W, H)	((W + 4)*H)
static void
creat_blank_md_menu (int w, int h)
{
	unsigned short end_addr = 0;
	osd2_oram_fill(MD_MENU_ADDR, MD_MENU_SIZE_C(w, h), 0x0000);
	osd2_menu_location(MD_RATTH, MD_RATTV,
		MD_MENU_ADDR, w, h);

	end_addr = MD_MENU_ADDR + MD_MENU_SIZE_C(w, h);
	osd2_cfg_wr(0x13, (unsigned char) (end_addr & 0xFF));
	osd2_cfg_wr(0x14, (unsigned char) ((end_addr>>8) & 0xFF));

}

const unsigned char MD_OSD2_LUT[] = {
//	A	  B		G	  R
	0xFE, 0x00, 0x00, 0x00,
	0x80, 0x00, 0xFF, 0x00,
};
const unsigned char MD_OSD2_ORG[] = {
  0x00, 0x00,
  0x10, 0x00,
  0x01, 0x68,
  0x02, 0x02,
  0x03, 0x02,
  0x04, 0xFF,
  0x05, 0xFF,
  0x06, 0xFF,
  0x07, 0xFF,
  0x0C, 0x00,
  0x0D, 0x02,
  0x11, 0x20,
  0x12, 0x00,
  0x13, 0x9B,
  0x14, 0x0B,
  0x16, 0x00,
  0x17, 0x00,
  0x0B, 0x40,
  0x0E, 0x04,
  0x50, 0xFF,
  0x51, 0x00,
  0x52, 0x00,
  0x19, 0xFE,
  0x1A, 0x1F,
  0x1B, 0xFF,
  0x1C, 0x1F,
  0x31, 0x01,
};
static int md_menu_w = 0;
static int md_menu_h = 0;
void ui_md_init (int w, int h)
{
	osd2_disable();
	/* load ORG */
	osd2_wr_regs_tbl((unsigned long)MD_OSD2_ORG,
		(sizeof(MD_OSD2_ORG)/sizeof(unsigned char)));
	/* load LUT */
	osd2_wr_lut_tbl((unsigned long)MD_OSD2_LUT,
		(sizeof(MD_OSD2_LUT)/sizeof(unsigned char)));
#if 0
	/* set OSDRAM of point address */
	osd2_set_ram_addr(0x0000);
	/* load blank font */
	fill_font(0);
	/* load full font */
	fill_font(0xFFFF);
	/* load half font */
	fill_font(0xFF00);
#else
	load_md_fonts();
#endif
	/* load MENU */
	_osd2_set_menu_addr(MD_MENU_ADDR);
	_osd2_set_menu_width(w);
	_osd2_set_menu_height(h);
	creat_blank_md_menu(w, h);

	md_menu_w = w;
	md_menu_h = h;
}

//int md_mask_x = 0;
//int md_mask_y = 0;
//int md_mask_w = 720;
//int md_mask_h = 480;

//#include "./drivers/cq/cq.h"
#define RATT_V	0x4000	// RATT_V of Attribute ID
#define RATT_H	0x6000	// RATT_H of Attribute ID
#define RATT_D	0xE000	// RATT_D of Attribute ID
#define RATT_J	0x8000	// RATT_J of Attribute ID
static void
md_menu_wr (int x, int y, int width, int height, int dat, int menu_a, int menu_w, int dir)
{
	int i;

	x &= 0x07FF;
	y &= 0x07FF;

	menu_a += 3;	// RATT_C
	for(i=0; i < height; i++) {
		OSD_SET_RAM_ADDR (menu_a);
		if((x + width + 1) == menu_w)
			OSD_SET_RAM_DATA (x + width + 1);
		else
			OSD_SET_RAM_DATA (x + width);
		if((x + width) == 1)
			OSD_SET_RAM_ADDR (menu_a + x + width);
		else {
			OSD_SET_RAM_ADDR (menu_a + x + width - (dir? 1 : 0));
			if(dir)
				OSD_SET_RAM_DATA (0x0201);
		}
		OSD_SET_RAM_DATA (dat);
		menu_a += (menu_w + 4);
		if((x + width + 1) < menu_w)
			OSD_SET_RAM_DATA ((RATT_J | (menu_a - 3)));
	}
}
static void
md_menu_wr2 (int x, int y, int width, int height, int dat, int menu_a, int menu_w, int dummy)
{
	int i = 0, j = 0;
	unsigned short addr = 0;

	x &= 0x07FF;
	y &= 0x07FF;

	menu_a += 4;	// character start
	for(i = y; i < (y + height); i++) {
		addr = menu_a + ((menu_w + 4) * i);
		OSD_SET_RAM_ADDR(addr - 1);
		OSD_SET_RAM_DATA((x + width + (dummy < 0? 0 : 1)));	// RATT_C
		for(j = 0; j < width; j++)
			OSD_SET_RAM_DATA(dat);
		if(dummy >= 0)
			OSD_SET_RAM_DATA(dummy);
		if((x + width) < menu_w)
			OSD_SET_RAM_DATA((RATT_J |(addr + menu_w)));
	}
	addr = menu_a + ((menu_w + 4) * i);
	addr -= 4;
	osd2_cfg_wr(0x13, (unsigned char) (addr & 0xFF));
	osd2_cfg_wr(0x14, (unsigned char) ((addr>>8) & 0xFF));
}

void ui_md_mask_loc (int x, int y)
{
	/* block aliment */
	x = (x/8) * 8;
	y = (y/8) * 8;

	osd2_menu_location((MD_RATTH + x), (MD_RATTV + y),
		MD_MENU_ADDR, md_menu_w, md_menu_h);

}

void ui_md_mask_clear (int w, int h)
{
	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w/16)
		osd2_menu_write_block(0, 0, (w/16), (h/8), 0x0201, MD_MENU_ADDR, md_menu_w);
	if(w%16)
		osd2_menu_write_block((w/16), 0, 1, (h/8), 0x0202, MD_MENU_ADDR, md_menu_w);

}

void ui_md_mask_idx (int w, int h)
{
	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w/16)
		osd2_menu_write_block(0, 0, (w/16), (h/8), 0x0201, MD_MENU_ADDR, md_menu_w);
	if(w%16)
		osd2_menu_write_block((w/16), 0, 1, (h/8), 0x0202, MD_MENU_ADDR, md_menu_w);
}

void ui_md_mask_h_inc (int w, int h)
{
	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w%16)
		md_menu_wr((w/16), 0, 1, (h/8), 0x0202, MD_MENU_ADDR, md_menu_w, 1);
	else
		md_menu_wr((w/16) - 1, 0, 1, (h/8), 0x0201, MD_MENU_ADDR, md_menu_w, 1);
}

void ui_md_mask_v_inc (int w, int h)
{
	int dummy = 0;

	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w%16) dummy = 0x0202;
	else dummy = -1;

	md_menu_wr2(0, (h/8) - 1, (w/16), 1, 0x0201, MD_MENU_ADDR, md_menu_w, dummy);
}

void ui_md_mask_h_dec (int w, int h)
{
	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w%16)
		md_menu_wr((w/16), 0, 1, (h/8), 0x0202, MD_MENU_ADDR, md_menu_w, 0);
	else
		md_menu_wr((w/16), 0, 1, (h/8), 0x0000, MD_MENU_ADDR, md_menu_w, 0);
}

void ui_md_mask_v_dec (int w, int h)
{
	int dummy = 0;

	/* block aliment */
	w = (w/8) * 8;
	h = (h/8) * 8;

	if(w%16) dummy = 0x0000;
	else dummy = -1;

	md_menu_wr2(0, (h/8), (w/16), 1, 0x0000, MD_MENU_ADDR, md_menu_w, dummy);
}

void ui_md_mask (int x, int y, int w, int h)
{
	ui_md_mask_loc(x, y);
	ui_md_mask_idx(w, h);
}



//void save_md_range (void)
//{
//	save_userdata(MD_RANGE_X);
//	save_userdata(MD_RANGE_Y);
//	save_userdata(MD_RANGE_W);
//	save_userdata(MD_RANGE_H);
//}

void ui_retrun_menu (void)
{
	osd2_disable();
	tw_load_oregs(page_video_view_main_menu.regsID);
	tw_load_luts(page_video_view_main_menu.lutsID);
	tw_load_tiles(page_video_view_main_menu.tilesID);
	_osd2_set_menu_addr(page_video_view_main_menu.addr);
	_osd2_set_menu_width(page_video_view_main_menu.width);
	_osd2_set_menu_height(page_video_view_main_menu.height);
	tw_osd2_menu_location(0x65, 1);
	_n_tw_gicon_load((struct tw_gicon*)&page_video_view_num);
	draw_set_clk(1);
	draw_rtc(1);
	osd2_enable();
	osd2_blink(2, 1);
}

void ui_md_pic_mask (int x, int y, int w, int h)
{
	int width = 0, height = 0;

	width = (w / MD_FONT_WIDTH) + (w % MD_FONT_WIDTH);
	height = (h / MD_FONT_HEIGHT) + (h % MD_FONT_HEIGHT);

	ui_md_init(width, height);
	ui_md_mask(x, y, w, h);
	osd2_enable();
}
