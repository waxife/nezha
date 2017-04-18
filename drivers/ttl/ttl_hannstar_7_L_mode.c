/**
 *  @file   ttl_hannstar_7_L_mode.c
 *  @brief	L mode for door phone
 *  $Id: ttl_hannstar_7_L_mode.c,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/18  Ken 	New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <sys.h>
#include <debug.h>
#include <iml.h>
#include "gpio.h"

#include "../../codec_eng/jreg.h"
#include "iml.hannstar_7_L_mode.h"
#include "ttl_hannstar_7_L_mode.h"
#include "./drivers/cq/cq.h"

#define WRITEL(VAL, ADDR)   writel (VAL, ADDR)
#define READL(ADDR)         readl (ADDR)

void ttl_hannstar_7_L_init(void)
{
	int i;
#if 0	
	for (i = 0; i < NUM_REG_PANEL_INITAL_P0; i++) {
        WRITEL (VALUE (panel_init_p0[i]), PAGE0 (panel_init_p0[i]));
        READL (PAGE0 (panel_init_p0[i]));
        usleep (1000);
    }
	for (i = 0; i < NUM_REG_PANEL_INITAL_P1; i++) {
        WRITEL (VALUE (panel_init_p1[i]), PAGE1 (panel_init_p1[i]));
        READL (PAGE1(panel_init_p1[i]));
        usleep (1000);
	}
    for (i = 0; i < NUM_REG_PANEL_INITAL_P2; i++) {
        WRITEL (VALUE (panel_init_p2[i]), PAGE2 (panel_init_p2[i]));
        READL (PAGE2 (panel_init_p2[i]));
        usleep (1000);
    }
#else
    cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	for (i = 0; i < NUM_REG_PANEL_INITAL_P0; i++) {
		cq_write_byte(CQ_P0, panel_init_p0[i].addr, panel_init_p0[i].value);
	}
	for (i = 0; i < NUM_REG_PANEL_INITAL_P1; i++) {
		cq_write_byte(CQ_P1, panel_init_p1[i].addr, panel_init_p1[i].value);
	}
    for (i = 0; i < NUM_REG_PANEL_INITAL_P2; i++) {
    	cq_write_byte(CQ_P2, panel_init_p2[i].addr, panel_init_p2[i].value);
    }
    if (cq_flush_now () < 0)
		ERROR ("cq_flush_now ()\n");
#endif
}


void ttl_hannstar_7_L_backlight_on(void)
{

}

void ttl_hannstar_7_L_backlight_off(void)
{

}

int ttl_hannstar_7_L_set_cvbs(int format)
{
    int i;

#if 0
    if (format == VIDEO_STD_NTSC) {
  	    for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAGE0; i++) {
            WRITEL (VALUE (reg_hannstar_7_cvbs_page0[i]), PAGE0 (reg_hannstar_7_cvbs_page0[i]));
            READL (PAGE0 (reg_hannstar_7_cvbs_page0[i]));
            usleep (1000);
        }

        for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAGE2; i++) {
            WRITEL (VALUE (reg_hannstar_7_cvbs_page2[i]), PAGE2 (reg_hannstar_7_cvbs_page2[i]));
            READL (PAGE2 (reg_hannstar_7_cvbs_page2[i]));
            usleep (1000);
        }
    } else if (format == VIDEO_FMT_PAL) {
  	    for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE0; i++) {
            WRITEL (VALUE (reg_hannstar_7_cvbs_pal_page0[i]), PAGE0 (reg_hannstar_7_cvbs_pal_page0[i]));
            READL (PAGE0 (reg_hannstar_7_cvbs_pal_page0[i]));
            usleep (1000);
        }

        for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE2; i++) {
            WRITEL (VALUE (reg_hannstar_7_cvbs_pal_page2[i]), PAGE2 (reg_hannstar_7_cvbs_pal_page2[i]));
            READL (PAGE2 (reg_hannstar_7_cvbs_pal_page2[i]));
            usleep (1000);
        }
    }
#else
    cq_trigger_source(CQ_TRIGGER_VSYNC);	// select trigger source
    if (format == VIDEO_STD_NTSC) {
		for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAGE0; i++) {
			cq_write_byte(CQ_P0, reg_hannstar_7_cvbs_page0[i].addr, reg_hannstar_7_cvbs_page0[i].value);
		}
		for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAGE2; i++) {
			cq_write_byte(CQ_P2, reg_hannstar_7_cvbs_page2[i].addr, reg_hannstar_7_cvbs_page2[i].value);
		}
    } else if (format == VIDEO_STD_PAL) {
		for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE0; i++) {
			cq_write_byte(CQ_P0, reg_hannstar_7_cvbs_pal_page0[i].addr, reg_hannstar_7_cvbs_pal_page0[i].value);
		}
		for (i = 0; i < NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE2; i++) {
			cq_write_byte(CQ_P2, reg_hannstar_7_cvbs_pal_page2[i].addr, reg_hannstar_7_cvbs_pal_page2[i].value);
		}
    }
    if (cq_flush_vsync () < 0)
		ERROR ("cq_flush_vsync ()\n");
#endif
    /* reset cvd ADC */
    writel(0x01, 0xb04023f0);
    usleep(10000);
    writel(0x00, 0xb04023f0);

    return 0;
}

/* pre-play (reg LB_CONF): dto      [15: 0]
 *                         pre-fill [31:16]
 *          (reg TIMING) : H back-porch [ 7:0]
 *                         V vack-porch [13:8] */
#define PPLAY_LB_CONF_CVBS_TTL_HANNSTAR_7   ((0x2500 << 16) | (0x0c9a & 0xffff))
#define PPLAY_TIMING_CVBS_TTL_HANNSTAR_7    0x0406

void ttl_hannstar_7_L_set_liveview(void)
{
    writel ((readl(0xb04000e0)&(~0x20)), 0xb04000e0);	// switch to input source
#ifndef fpll_en
    writel (0x02, 0xb0400c20);
#endif
}

void ttl_hannstar_7_L_set_playback(void)
{
//	writel (readl(0xb04000e0)|0x20, 0xb04000e0);	// switch to jpeg decode
//#ifndef fpll_en
//    writel (0x12, 0xb0400c20);
//#endif
    /* set pre-play reg */
    writel(PPLAY_LB_CONF_CVBS_TTL_HANNSTAR_7, PPLAY_LB_CONF_REG);
    writel(PPLAY_TIMING_CVBS_TTL_HANNSTAR_7, PPLAY_TIMING_REG);
}

