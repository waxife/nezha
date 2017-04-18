/**
 *  @file   page_video_view.c
 *  @brief	T582 demo for door phone
 *  $Id: page_video_view.c,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2013/11/15  Ken 	New file.
 *
 */

#include <unistd.h>
#include <io.h>
#include <display.h>
#include <video_det.h>
#include <eyecatch.h>
#include <tw_widget_sys.h>
#include <callback.h>

#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"
#include "./drivers/cq/cq.h"

#ifdef DBG_LEVEL
#undef DBG_LEVEL
#endif
#define DBG_LEVEL   0
#include <debug.h>

#include "OSD/res.h"
#include "OSD/page_video_view.h"

#include "task/req_service.h"
#include "task/md_service.h"
#include "ui_draw.h"



extern int tag_idx;
static int cur_std = AVIN_CVBS_NTSC;	// default standard

void door_bell_event (void);


void change_cvd_std (int std)
{
    //display_set_cvbs_lmod(std, CH0);
    display_set_cvbs_full(std, CH0);
    //ui_disp_background(BK_VIDEO_TYPE);
    /* open panel */
    display_set_liveview_screen();
}



unsigned char page_video_view_process (TW_EVENT* event)
{
	switch(event->type)
	{
		case TW_EVENT_TYPE_ENTER_SYSTEM:
			dbg(2, ">>>>> Enter Video View Page\n\r");
			/* todo: Write Your Code Here */
           
			//display_set_cvbs_lmod(cur_std, CH0);
            display_set_cvbs_full(cur_std, CH0);           
        

            /* open panel */
            display_set_liveview_screen();
  

            break;

       
        case TW_EVENT_TYPE_KEY_DOWN:
            dbg(2, ">>>>>> Key Down Event\n\r");
			dbg(2, "keystroke = 0x%02X\n\r", event->keystroke.ch);

			//page_view_key_process(event->keystroke.ch);
        break;

       

        case TW_EVENT_TYPE_SIGNAL:
			dbg(2, ">>>>> Signal Event\n\r");
			dbg(2, "curren CVD state = 0x%02X\n\r", event->signal.std);

			cur_std = event->signal.std;

			if(event->signal.std == AVIN_CVBS_NTSC) {
                dbg(2, ">>>>> Set NTSC\n\r");
                change_cvd_std(AVIN_CVBS_NTSC);
                req_md_srv(0, 0, 720, 480, 0);
            }
            else if(event->signal.std == AVIN_CVBS_PAL) {
                dbg(2, ">>>>> Set PAL\n\r");
                change_cvd_std(AVIN_CVBS_PAL);
                req_md_srv(0, 0, 720, 576, 0);
            }
            else if(event->signal.std == AVIN_CVBS_CLOSE) {
                dbg(2, ">>>>> No Signal Close Panel\n\r");
               // ui_disp_background(BK_MENU_TYPE);
            }
            else {
                dbg(2, ">>>>> Signal Type Not Support\n\r");
               // ui_disp_background(BK_MENU_TYPE);
            }

		break;

        case TW_EVENT_TYPE_OTHER:
        	dbg(2, ">>>>>> Other Event\n\r");

        break;

		case TW_EVENT_TYPE_QUIT_SYSTEM:
			dbg(2, ">>>>> QUIT Video View Page\n\r");
			/* todo: Write Your Code Here */
			return TW_EVENT_TYPE_QUIT_TASK;
		break;

		default:	/* put to nodify if no process this event */

			return TW_RETURN_NO_PROCESS;
		break;
	}

	return TW_RETURN_NONE;
}
