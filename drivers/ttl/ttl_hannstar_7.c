/**
 *  @file   ttl_hannstar_7.c
 *  @brief  Hannstar TTL panel driver for T582
 *  $Id: ttl_hannstar_7.c,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
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

#include "../../codec_eng/jreg.h"
#include "ttl_hannstar_7.h"
#include "iml.hannstar_7_cvbs.h"
//#include "iml.hannstar_7_oneshot.h"
#if 0
#include "iml.hannstar_7_cmos.h"
#endif

#define WRITEL(VAL, ADDR)   writel (VAL, ADDR)
#define READL(ADDR)         readl (ADDR)

void ttl_hannstar_7_init(void)
{
	int i;
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
}


void ttl_hannstar_7_backlight_on(void)
{
    writel(0x80, 0xB0401620);
    writel(0x80, 0xB0401650);
	
    return;
}

void ttl_hannstar_7_backlight_off(void)
{
    writel(0x00, 0xB0401620);
    writel(0x00, 0xB0401650);
	
    return;
}

int ttl_hannstar_7_set_cvbs(int format)
{
    int i;
		
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
    } else if (format == VIDEO_STD_PAL) {
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

    /* reset cvd ADC */
    writel(0x01, 0xb04023f0);
    usleep(10000);
    writel(0x00, 0xb04023f0);

    return 0;
}

int ttl_hannstar_7_set_cmos(void)
{
    return 0;
}


/* pre-play (reg LB_CONF): dto      [15: 0]
 *                         pre-fill [31:16]
 *          (reg TIMING) : H back-porch [ 7:0]
 *                         V vack-porch [13:8] */
#define PPLAY_LB_CONF_CVBS_TTL_HANNSTAR_7   ((0x2500 << 16) | (0x0c9a & 0xffff))
#define PPLAY_TIMING_CVBS_TTL_HANNSTAR_7    0x0406

int ttl_hannstar_7_set_cvbs_oneshot(int format)
{
    int i;

/*
    writel(0x20 | readl(0xB04000E0), 0xB04000E0); // switch to jpeg decode
*/
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
    } else if (format == VIDEO_STD_PAL) {
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
		//writel (readl(0xb04000e0)|0x23, 0xb04000e0);
		writel (0x45, 0xb0400740);
		writel (0x15, 0xb0400750);
		writel (0x28, 0xb0400DA0);
		writel (0xC8, 0xb0400B80);
    }
	
    writel (readl(0xb04000e0)|0x23, 0xb04000e0);
#ifndef fpll_en 
    writel (0x12, 0xb0400c20);
#endif
    /* set pre-play reg */
    writel(PPLAY_LB_CONF_CVBS_TTL_HANNSTAR_7, PPLAY_LB_CONF_REG);
    writel(PPLAY_TIMING_CVBS_TTL_HANNSTAR_7, PPLAY_TIMING_REG);

    return 0;
}

