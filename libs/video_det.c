/**
 *  @file   video_det.c
 *  @brief  Detect and return video standard cmd.
 *  $Id: video_det.c,v 1.8 2014/06/10 03:08:09 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  New file.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>
#include <debug.h>
#include "video_det.h"
#ifdef DBG_LEVEL
#undef DBG_LEVEL
#define DBG_LEVEL   0
#endif

struct cvbs_state cvbs_st;
static int cvd_state_pre = AVIN_CVBS_CLOSE;
void cvbs_reset_st(int cvd_state)
{
    cvd_state_pre = cvd_state;
    cvbs_st.std_mode = cvd_state;
}
/**
 * @func       cvbs_detect_change
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
int cvbs_detect_change(void)
{
    int cvd_state_now;
	
    if(readb(0xb04000e0)&0x01)
        cvd_state_now = cvbs2_detect(&cvbs_st);
    else
        cvd_state_now = cvbs2_detect656(&cvbs_st);
        
	if (cvd_state_now != cvd_state_pre)
	{
	    dbg(2,"cvd_state_now = %d, cvd_state_pre = %d\n", cvd_state_now, cvd_state_pre);
		if (cvd_state_now == AVIN_CVBS_CLOSE)
		{
		    cvd_state_pre = cvd_state_now;
            if((readb(0xb04000e0)&0x20)==0){
                cvbs2_bluescreen_on(1, 0x00,0x80, 0x80);
            }
			dbg(2, ">>>>> No Signal\n\r");
			
			return 1;
        
		}
		else if ((cvd_state_now == AVIN_CVBS_NTSC) || (cvd_state_now == AVIN_CVBS_PAL))
		{		
            
		    if((readb(0xb04000e0)&0x20)==0){
                cvbs2_bluescreen_on(1, 0x00,0x80, 0x80);
            }
		    cvd_state_pre = cvd_state_now;		    
			dbg(2,"cvd_state = %d\n", cvd_state_now);
			return 1;
			
		}
		else 
		{
		    dbg(2,"unknow cvd_state = %d\n", cvd_state_now);
		    cvd_state_pre = cvd_state_now;
		    return 1;
		}
	    return 0;
	}
	return 0;
}


/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
int cvbs_get_standard(void){
    if(readb(0xb04000e0)&0x01)
        return cvbs2_detect(&cvbs_st);
    else
        return cvbs2_detect656(&cvbs_st);
}

int cvbs_get_stable_standard(void){
    if(readb(0xb04000e0)&0x01)
        cvd_state_pre = cvbs2_detect_wait(&cvbs_st);    
    else
        cvd_state_pre = cvbs2_detect656_wait(&cvbs_st);   
    return cvd_state_pre;
}


