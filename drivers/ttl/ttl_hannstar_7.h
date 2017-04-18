/**
 *  @file   ttl_hannstar_7.h
 *  @brief  Hannstar TTL panel driver header for T582
 *  $Id: ttl_hannstar_7.h,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *
 */
#ifndef _TTL_HANNSTAR_7_H_
#define _TTL_HANNSTAR_7_H_

#define VIDEO_STD_NTSC  1
#define VIDEO_STD_PAL  2

void ttl_hannstar_7_init(void);
int ttl_hannstar_7_set_cvbs(int format);
int ttl_hannstar_7_set_cvbs_oneshot(int format);
int ttl_hannstar_7_set_cmos(void);
void ttl_hannstar_7_backlight_on(void);
void ttl_hannstar_7_backlight_off(void);

#endif /* _TTL_HANNSTAR_7_H_ */
