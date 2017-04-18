/**
 *  @file   main.c
 *  @brief
 *  $Id: main.c,v 1.2 2015/08/06 05:31:25 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/17  Ken 	New file.
 *
 */

#include <config.h>
#include <debug.h>
#include <io.h>
#include "mconfig.h"
#include "ls.h"
#include "display.h"
#include "tw_widget_sys.h"
#include "rc.h"
#include <sys.h>
#include <time.h>

#include "./drivers/spirw/spirw.h"
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"


/* sarkey configure for demo board */
#define SARKEY_DEMOBOARD_NUM	7
const static unsigned char sarkey_demo_table[7][2] = {
 {0xe7,0x01},{0xc4,0x02},{0x98,0x03},{0x80,0x04},
 {0x66,0x05},{0x3f,0x06},{0x20,0x07}
};

static void disp_osd_init (void)
{
	/* enable NOR flash of Quad mode for SPIOSD */
	//enable_quad_io();
	//spiosd_init(0x116, 0x0A, 0x96, 0x0D);	// initialize and setting SPIOSD position of offset

	/* initialize for OSD of resource */
	//resource_init();

	/* initialize for OSD2 */
	osd2_init();
}

/* media configure */
void doorphone_mconfig (void)
{
    int rc;

    media_config(FATFS, MEDIA_PIC, "picture", 1023, 128*1024);
    media_config(FATFS, MEDIA_AUMSG, "aumsg", 64, 64*1024);
    media_config(FATFS, MEDIA_VIDEO, "video", -1, 32*1024*1024);
    media_config(NORFS, MEDIA_AUMSG, "aumsg", 10, 64*1024);
    media_config(NORFS, MEDIA_PIC, "picture", -1, 64*1024);
    rc = media_config_done();
    if (rc < 0) {
        dbg(2, "Failed to config media space");
    }

    media_config_show();
    //_format();
    //sdc_detect();

  
}

int main_drive(void *sys_ctx)
{
	/* initialize for display and video input */
	display_init();
	display_bklight_on();

	disp_osd_init();

    tw_task_handler(1, tw_page_handler);

	return 0;
}

