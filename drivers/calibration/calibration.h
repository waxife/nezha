/**
 *  @file   calibration.h
 *  @brief  calibration driver of header file
 *  $Id: calibration.h,v 1.10 2015/01/19 02:34:00 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.10 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

/* calibration mode of definition */
#define	CHECK_IFNOT_CALIBRA		0
#define CHECK_IFNOT_DEFAULT		1
#define	FORCE_TP_CALIBRA		2
#define	FACTORY_CALIBRA_MODE	3

/* default setting of definition for resolution 800x480 */
#define DEFAULT_TP_LEFT			478
#define DEFAULT_TP_RIGHT		28
#define DEFAULT_TP_UP			434
#define DEFAULT_TP_DOWN			71
#define DEFAULT_TP_MODE			0
#define	DEFAULT_TP_WIDTH		800
#define	DEFAULT_TP_HIGHT		480

extern void do_calibration (unsigned char select);
extern unsigned short tp_left;
extern unsigned short tp_right;
extern unsigned short tp_up;
extern unsigned short tp_down;
extern unsigned short tp_mode;
extern unsigned short c_w;
extern unsigned short c_h;
extern unsigned char calibration_finish;
extern void calib_diff_threshold(int value);

#endif
