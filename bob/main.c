/**
 *  @file   main.c
 *  @brief
 *  $Id: main.c,v 1.2 2014/08/11 05:00:28 ken Exp $
 *  $Author: ken $
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
#include "./drivers/cq/cq.h"

#include "./drivers/spirw/spirw.h"
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"

#include "file_manip.h"
#include "sd_upgrade.h"
#include "userdata.h"

/* sarkey configure for demo board */
#define SARKEY_DEMOBOARD_NUM	7
const static unsigned char sarkey_demo_table[7][2] = {
 {0xe7,0x01},{0xc4,0x02},{0x98,0x03},{0x80,0x04},
 {0x66,0x05},{0x3f,0x06},{0x20,0x07}
};

extern unsigned char br_val, co_val;

static void disp_osd_init (void)
{
	/* enable NOR flash of Quad mode for SPIOSD */
	enable_quad_io();
	spiosd_init(0x116, 0x0A, 0x96, 0x0D);	// initialize and setting SPIOSD position of offset

	/* initialize for OSD of resource */
	resource_init();

	/* initialize for OSD2 */
	osd2_init();
}

/* media configure */
static void main_mconfig (void)
{
    int rc;

    media_config(FATFS, MEDIA_PIC, "picture", 1023, 64*1024);
    media_config(FATFS, MEDIA_AUMSG, "aumsg", 64, 64*1024);
    media_config(FATFS, MEDIA_VIDEO, "video", -1, 32*1024*1024);
    //media_config(NORFS, MEDIA_AUMSG, "aumsg", 10, 64*1024);
    //media_config(NORFS, MEDIA_PIC, "picture", -1, 64*1024);
    rc = media_config_done();
    if (rc < 0) {
        dbg(2, "Failed to config media space");
    }

    media_config_show();

    chk_sd_status();
}

static void shell_init(void)
{
    extern unsigned int _ftboot, _etext, _fdata, _etboot, _stack, __STACK_BARRIER;
    extern unsigned int _fbss, _ebss;
    unsigned int addr, addr2;
    unsigned int len;

    addr = (unsigned int)&_ftboot;
    addr2 = (unsigned int)&_etext;
    printf("program %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_fdata;
    addr2 = (unsigned int)&_etboot;
    printf("data    %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_fbss;
    addr2 = (unsigned int)&_ebss;
    printf("bss     %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_stack;
    len = addr - (unsigned int) &__STACK_BARRIER;
    printf("stack   %08x-%p (size=%d)\n", addr, &__STACK_BARRIER, len);
}


int main_drive(void *sys_ctx)
{
	/* initialize for display and video input */
	char time_str[32];
	memset(time_str, 0, sizeof(time_str));
	
	display_init();
    display_bklight_on();
    //Enable Color Bar
    //cq_write_byte_issue(CQ_P0, 0x91, 0xB8, CQ_TRIGGER_SW);

	display_set_cvbs_full(2, CH0);
    cvbs2_bluescreen_on(0,0,0,0);

	dbg(1, "**********************\r\n");
	dbg(1, "*******bob's code*****\r\n");
	dbg(1, "**********************\r\n");

	disp_osd_init();

	main_mconfig();

	userdata_init();
	dbg(2, "user data init\r\n");
	
	dbg(2, "br = %d; co = %d\r\n", user_datas[BRIGHTNESS], user_datas[CONTRAST]);
	
	br_val = user_datas[BRIGHTNESS];
	co_val = user_datas[CONTRAST];

	/*brightness and contrast range [0:100]*/
	if(br_val>100)
	{
		br_val = 100;
	}
	if(co_val>100)
	{
		co_val = 100;
	}
	
	/* this configure for demo board, default configure support EVB */
	//sarkey_init_table(sarkey_demo_table, SARKEY_DEMOBOARD_NUM);

	/* auto upgrade if has DFU files in SD card */
	sd_auto_upgrade(time_str);

	dbg(2, "time = %s\r\n", time_str);

	tw_task_handler(1, tw_page_handler);

	return 0;
}

