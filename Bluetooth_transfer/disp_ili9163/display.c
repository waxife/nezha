/**
 *  @file   display_doorphone.c
 *  @brief	innolux 7' for door phone demo
 *  $Id: display.c,v 1.1 2016/07/22 10:17:22 joec Exp $
 *  $Author: joec $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/18  Ken 	New file.
 *
 */

#include <stdio.h>
#include <display.h>
#include <sys.h>
#include <io.h>
#include "gpio.h"
#include "cvbs.h"
#include "eyecatch.h"
#include "./drivers/cq/cq.h"
#include "./drivers/i80/i80.h"
#include "drivers/ttl/ttl_driver.h"
#include "iml_cvbs.h"
#include <codec_eng/jreg.h>
#include "ili9163_init.c"


/*Define i80 panel type.*/
#define I80_DATABUS_PINS   8
#define I80_RES_WIDTH      400
#define I80_RES_HIGHT      240
int display_init(void)
{

    i80_init(ILI9327, I80_DATABUS_PINS, I80_RES_WIDTH, I80_RES_HIGHT);
 
    ili9163_init();
    
    
	ttl_init_reg_tables( panel_init_p0, NUM_REG_PANEL_INITAL_P0,  panel_init_p1, NUM_REG_PANEL_INITAL_P1, 
	                     panel_init_p2, NUM_REG_PANEL_INITAL_P2, NULL, 0);
	cvbs2_fpll_init();
	i80_set_input (I80_INPUT_SCALER);
	/* backlight control by GPIO10 */
	//gpio_out(10);

	return 0;
}

int display_release(void)
{
    return 0;
}

int display_bklight_on(void)
{
	gpio_set(10);

    return 0;
}

int display_bklight_off(void)
{
    gpio_clear(10);
    return 0;
}

int display_set_cvbs_lmod(int format, int channel)
{
    switch (format) {
    case VIDEO_STD_NTSC:
    	ttl_set_reg_tables(reg_cvbs_ntsc_lmod_page0, NUM_REG_CVBS_NTSC_LMOD_PAGE0, NULL, 0, 
    	                    reg_cvbs_ntsc_lmod_page2, NUM_REG_CVBS_NTSC_LMOD_PAGE2, NULL, 0);
        break;
    case VIDEO_STD_PAL:
    	ttl_set_reg_tables(reg_cvbs_pal_lmod_page0, NUM_REG_CVBS_PAL_LMODE_PAGE0, NULL, 0, 
    	                    reg_cvbs_pal_lmod_page2, NUM_REG_CVBS_PAL_LMODE_PAGE2 ,NULL, 0);
        break;
    default:
        break;
    }
    
    cq_write_byte_issue (CQ_P0, 0x01, ((readb(0xb0400010)&0xFC)|(channel&0x03)), CQ_TRIGGER_VDE);
    cq_write_byte_issue (CQ_P0, 0x0E, (readb(0xb04000e0)&0xDF), CQ_TRIGGER_VDE);
    return 0;
}

int display_set_cvbs_full(int format, int channel)
{
    switch (format) {
    case VIDEO_STD_NTSC:
    	ttl_set_reg_tables(reg_cvbs_ntsc_full_page0, NUM_REG_CVBS_NTSC_FULL_PAGE0, NULL, 0, 
    	                    reg_cvbs_ntsc_full_page2, NUM_REG_CVBS_NTSC_FULL_PAGE2, NULL, 0);
        break;
    case VIDEO_STD_PAL:
    	ttl_set_reg_tables(reg_cvbs_pal_full_page0, NUM_REG_CVBS_PAL_FULL_PAGE0, NULL, 0, 
    	                    reg_cvbs_pal_full_page2, NUM_REG_CVBS_PAL_FULL_PAGE2, NULL, 0);
        break;
    default:
        break;
    }
    cq_write_byte_issue (CQ_P0, 0x01, ((readb(0xb0400010)&0xFC)|(channel&0x03)), CQ_TRIGGER_VDE);
    cq_write_byte_issue (CQ_P0, 0x0E, (readb(0xb04000e0)&0xDF), CQ_TRIGGER_VDE);
    return 0;
}


int display_set_play_lmod(int format)
{
    switch (format) {
    case VIDEO_STD_NTSC:
    	ttl_set_reg_tables(reg_play_ntsc_lmod_page0, NUM_REG_PLAY_NTSC_LMOD_PAGE0, NULL, 0, 
    	                    reg_play_ntsc_lmod_page2, NUM_REG_PLAY_NTSC_LMOD_PAGE2, NULL, 0);
    	/* set pre-play reg */
        writel(0x00D60DEA, PPLAY_LB_CONF_REG);
        writel(0x00101030, PPLAY_TIMING_REG);
        writel(0x11, 0xB0400E20);
        writel(0x00, 0xB0400E20);
        break;
    case VIDEO_STD_PAL:
    	ttl_set_reg_tables(reg_play_pal_lmod_page0, NUM_REG_PLAY_PAL_LMODE_PAGE0, NULL, 0, 
    	                    reg_play_pal_lmod_page2, NUM_REG_PLAY_PAL_LMODE_PAGE2 ,NULL, 0);
    	/* set pre-play reg */
        writel(0x05FC0D5A, PPLAY_LB_CONF_REG);
        writel(0x00100E30, PPLAY_TIMING_REG);
        writel(0x11, 0xB0400E20);
        writel(0x00, 0xB0400E20);
        break;
    default:
        break;
    }
    
   
	cq_write_byte_issue (CQ_P0, 0x0E, (readb(0xb04000e0)|0x20), CQ_TRIGGER_VDE);
	cq_write_byte_issue (CQ_P1, 0x3A, 0xE4, CQ_TRIGGER_SW); // fpll enable, don't have input signal so disable Force_blue_wshd  
	
	
    
    return 0;
}
int display_set_play_full(int format)
{
   switch (format) {
    case VIDEO_STD_NTSC:
    	ttl_set_reg_tables(reg_play_ntsc_full_page0, NUM_REG_PLAY_NTSC_FULL_PAGE0, NULL, 0, 
    	                    reg_play_ntsc_full_page2, NUM_REG_PLAY_NTSC_FULL_PAGE2, NULL, 0);
    	/* set pre-play reg */
        writel(0x0BFC0300, PPLAY_LB_CONF_REG);
        writel(0x00100E30, PPLAY_TIMING_REG);
        writel(0x11, 0xB0400E20);
        writel(0x00, 0xB0400E20);
        break;
    case VIDEO_STD_PAL:
    	ttl_set_reg_tables(reg_play_pal_full_page0, NUM_REG_PLAY_PAL_FULL_PAGE0, NULL, 0, 
    	                    reg_play_pal_full_page2, NUM_REG_PLAY_PAL_FULL_PAGE2, NULL, 0);
    	/* set pre-play reg */
        writel(0x05FC0D5A, PPLAY_LB_CONF_REG);
        writel(0x00100E30, PPLAY_TIMING_REG);
        writel(0x11, 0xB0400E20);
        writel(0x00, 0xB0400E20);
        break;
    default:
        break;
    }
   
	cq_write_byte_issue (CQ_P0, 0x0E, (readb(0xb04000e0)|0x20), CQ_TRIGGER_VDE);
	cq_write_byte_issue (CQ_P1, 0x3A, 0xE4, CQ_TRIGGER_SW); // fpll enable, don't have input signal so disable Force_blue_wshd  
	
	
    return 0;
}

int display_set_blackscreen(unsigned long color)
{

    cvbs2_bluescreen_on(1, (color&0xFF0000)>>16, (color&0x00FF00)>>8, (color&0x0000FF));
    return 0;
}

int display_set_blackscreen_effect(void)
{
    return ect_black_screen(FD_FAST,FD_NONE);
}

int display_set_liveview_screen(void)
{
    cvbs2_bluescreen_on(0, 0, 0, 0);
    return 0;
}




