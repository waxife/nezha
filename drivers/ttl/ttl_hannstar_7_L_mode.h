/**
 *  @file   ttl_hannstar_7_L_mode.h
 *  @brief	L mode for door phone
 *  $Id: ttl_hannstar_7_L_mode.h,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/18  Ken 	New file.
 *
 */


#ifndef TTL_HANNSTAR_7_L_MODE_H_
#define TTL_HANNSTAR_7_L_MODE_H_

#define VIDEO_STD_NTSC  1
#define VIDEO_STD_PAL   2

void ttl_hannstar_7_L_init(void);
int ttl_hannstar_7_L_set_cvbs(int format);
void ttl_hannstar_7_L_backlight_on(void);
void ttl_hannstar_7_L_backlight_off(void);

void ttl_hannstar_7_L_set_liveview(void);
void ttl_hannstar_7_L_set_playback(void);

#endif /* TTL_HANNSTAR_7_L_MODE_H_ */
