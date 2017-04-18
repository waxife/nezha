/**
 *  @file   video_det.h
 *  @brief  Detect and return video standard cmd.
 *  $Id: video_det.h,v 1.3 2014/01/17 09:48:56 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  New file.
 *
 */
 
#ifndef _VIDEO_DET_H_
#define _VIDEO_DET_H_
#include "cvbs.h"
extern struct cvbs_state cvbs_st;

/**
 * @func       cvbs_detect_change
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
extern int cvbs_detect_change(void);

/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
extern int cvbs_get_standard(void);
extern int cvbs_get_stable_standard(void);

/*
 * @func       cvbs_reset_st
 * @brief      Reset cvd detect status. Be carefule of use it. 
 * @param     
 * @return     null
 */
extern void cvbs_reset_st(int cvd_state);

#endif //_VIDEO_DET_H_



