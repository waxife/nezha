/**
 *  @file   keymap.h
 *  @brief
 *  $Id: keymap.h,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/15  Ken 	New file.
 *
 */


#ifndef KEYMAP_H_
#define KEYMAP_H_

/* IR Key Map */
#define IR_POWER_CODE       0x45
#define IR_MENU_CODE        0x46
#define IR_HOME_CODE        0x47
#define IR_ENTER_CODE       0x15
#define IR_LEFT_CODE        0x07
#define IR_RIGHT_CODE       0x09
#define IR_UP_CODE          0x40
#define IR_DOWN_CODE        0x19
#define IR_SLIDE_CODE       0x0C
#define IR_PLAY_CODE        0x18
#define IR_BACK_CODE        0x5E
#define IR_ZOOM_CODE        0x08
#define IR_SELECT_CODE      0x1C
#define IR_ROTATE_CODE      0x5A
#define IR_SUB_CODE         0x42
#define IR_MUTE_CODE        0x52
#define IR_PLUS_CODE        0x4A

/* Keypad Map */
#define KEY_CODE_1			1
#define KEY_CODE_2			2
#define KEY_CODE_3			3
#define KEY_CODE_4			4
#define KEY_CODE_5			5
#define KEY_CODE_6			6
#define KEY_CODE_7			7

/* event */
#define EVENT_DOOR_BELL		8
#define EVENT_CVD_NTSC		9
#define EVENT_CVD_PAL		10
#define EVENT_CVD_LOST		11
#define EVENT_BELL_RUN		12
#define EVENT_BELL_STOP		13
#define EVENT_ERR_TIMEOUT	14

/* button of region */
#define _left_pnl_btn_num 5

#define _left_pnl_st_x  (644+35)
#define _left_pnl_st_y  10
#define _left_pnl_dy    85

#define _left_pnl_btn_w 72 + 52
#define _left_pnl_btn_h 72

#define _btn_2main_x (644+87)
#define _btn_2main_y 427
#define _btn_2main_w 53
#define _btn_2main_h 53

#define _btn_extend_x (644+16)
#define _btn_extend_y 427
#define _btn_extend_w 53
#define _btn_extend_h 53

#endif /* KEYMAP_H_ */
