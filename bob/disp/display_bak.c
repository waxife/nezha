/**
 *  @file   display_doorphone.c
 *  @brief	innolux 7' for door phone demo
 *  $Id: display.c,v 1.5 2014/07/18 08:17:24 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/18  Ken 	New file.
 *
 */

#include <stdio.h>
#include <display.h>
#include <sys.h>
#include <unistd.h>
#include <io.h>
#include "gpio.h"
#include "cvbs.h"
#include "eyecatch.h"
#include "./drivers/cq/cq.h"
#include "drivers/ttl/ttl_driver.h"
#include "iml_cvbs.h"
#include <codec_eng/jreg.h>

const unsigned char GammaData[]={
0x00,0x02,0x04,0x06,0x07,0x09,0x0B,0x0C,0x0E,0x0F,0x11,0x12,0x13,0x15,0x16,0x18,
0x19,0x1A,0x1B,0x1D,0x1E,0x1F,0x21,0x22,0x23,0x24,0x26,0x27,0x28,0x29,0x2A,0x2C,
0x2D,0x2E,0x2F,0x30,0x31,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3C,0x3D,0x3E,
0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5E,0x5F,0x60,
0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,
0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,
0x9E,0x9F,0xA0,0xA1,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,
0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,
0xBB,0xBC,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC4,0xC5,0xC6,0xC7,0xC8,
0xC9,0xCA,0xCB,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,
0xD7,0xD8,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,
0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xEF,0xF0,0xF1,
0xF2,0xF3,0xF4,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
};

void InitGamma(void)
{
    int i;
	writel(0x06, 0xB0400900);	
	writel(0x00, 0xB0400930);
	for(i=0;i<sizeof(GammaData);i++){
		writel(GammaData[i], 0xB0400940);
        usleep(20);//Don't remove! or Gamma load error.
    }
	


}


int display_init(void)
{

	ttl_init_reg_tables( panel_init_p0, NUM_REG_PANEL_INITAL_P0,  panel_init_p1, NUM_REG_PANEL_INITAL_P1, 
	                     panel_init_p2, NUM_REG_PANEL_INITAL_P2, NULL, 0);
    InitGamma();                
	cvbs2_fpll_init();
	usleep(100000);
	/* backlight control by GPIO10 */
	gpio_out(10);

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
    cvbs2_fine_tune_dto();
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
    cvbs2_fine_tune_dto();
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
        writel(0x0ED60DEA, PPLAY_LB_CONF_REG);
        writel(0x00101030, PPLAY_TIMING_REG);
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




