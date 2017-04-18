/**
 *  @file   touch.h
 *  @brief  touch interrupt header 
 *  $Id: touch.h,v 1.11 2015/01/19 02:33:25 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.11 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _TOUCH_H_
#define _TOUCH_H_

#define TOUCH_PEN_DOWN 		0x00
#define TOUCH_PEN_MOVE 		0x01
#define TOUCH_PEN_REL 		0x02
#define TOUCH_NOACK   		0x03

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */

typedef struct __attribute__((packed)) touchpanel
{
	unsigned char status;
	unsigned short int x;
	unsigned short int y;
} touchd;

typedef struct __attribute__((packed)) touchtemp
{
	unsigned short x;
	unsigned short y;
} tcoord;

#pragma pack(pop)

extern unsigned short td_rep_thres;

extern int Calibration_MAX;

extern void touch_close(void);

extern void touch_init(void);

extern touchd touch_read(void);

extern void touch_isr(void);

extern void touch_clear(void);

extern touchd check_repeat(unsigned short threshold);

extern void touch_targe_test(void);

extern void draw_target_test(void);

extern int touch_set_resolution(int value);

#endif
