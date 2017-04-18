/**
 *  @file   eyecatch.h
 *  @brief  eye-catch effect. fade in fade out.
 *  $Id: eyecatch.h,v 1.3 2014/01/16 03:43:06 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  New file.
 *
 */
 
#ifndef _EYECATCH_H_
#define _EYECATCH_H_

/**
 * Fade in mode.
 */
typedef enum {
	FD_NONE,
	FD_SLOW,
	FD_FAST
} FD_MODE_T;

extern void ect_reset(void);

/**
 * @func       Black Screen Effect
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
extern int ect_black_screen(int fade_in_mode, int fade_out_mode);
extern int ect_black_screen_noblock(void);
extern int ect_black_screen_noblock_ncq(void);

/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
extern int ect_play_screen(void);
extern int ect_play_screen_noblock(void);
extern int ect_play_screen_noblock_ncq(void);
#endif //_EYECATCH_H_



