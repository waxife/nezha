/**
 *  @file   out_i80_320x240.c
 *  @brief  scaler setting functions, input t515, output i80 320x240
 *  $Id: out_i80_320x240.c,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2009 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/02/24  Ken  New file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <debug.h>

#include "iml.h"
#include "../i80_kirin/i80.h"
#include "../../codec_eng/jreg.h"
#include "out_i80_320x240.h"
#include "iml.in_t515_ntsc_out_i80_320x240.h"
#include "iml.in_cvbs_ntsc_out_i80_320x240.h"

#define ENABLE_SHADOW
#define WAIT_FRAMES 2


/*
 * scaler setting
 */
/* input: CMOS CCIR601 640x480 */
static void
scaler_setting_in_cmos_out_i80_320x240 (struct i80c *pi80)
{
#if 0
    int i;

    for (i = 0; i < NUM_REG_IN_CMOS_OUT_I80_320X240_PAGE0; i++) {
        WRITEL (VALUE (reg_in_cmos_out_i80_320x240_page0[i]), PAGE0 (reg_in_cmos_out_i80_320x240_page0[i]));
        READL (PAGE0 (reg_in_cmos_out_i80_320x240_page0[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
    for (i = 0; i < NUM_REG_IN_CMOS_OUT_I80_320X240_PAGE1; i++) {
        WRITEL (VALUE (reg_in_cmos_out_i80_320x240_page1[i]), PAGE1 (reg_in_cmos_out_i80_320x240_page1[i]));
        READL (PAGE1 (reg_in_cmos_out_i80_320x240_page1[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
    for (i = 0; i < NUM_REG_IN_CMOS_OUT_I80_320X240_PAGE2; i++) {
        WRITEL (VALUE (reg_in_cmos_out_i80_320x240_page2[i]), PAGE2 (reg_in_cmos_out_i80_320x240_page2[i]));
        READL (PAGE2 (reg_in_cmos_out_i80_320x240_page2[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
#endif
}

/* input: CCIR656 NTSC */
static void
scaler_setting_in_t515_ntsc_out_i80_320x240 (struct i80c *pi80)
{
#if 1
    int i;

    for (i = 0; i < NUM_REG_IN_T515_NTSC_OUT_I80_320X240_PAGE0; i++) {
        WRITEL (VALUE (reg_in_t515_ntsc_out_i80_320x240_page0[i]), PAGE0 (reg_in_t515_ntsc_out_i80_320x240_page0[i]));
        READL (PAGE0 (reg_in_t515_ntsc_out_i80_320x240_page0[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    for (i = 0; i < NUM_REG_IN_T515_NTSC_OUT_I80_320X240_PAGE2; i++) {
        WRITEL (VALUE (reg_in_t515_ntsc_out_i80_320x240_page2[i]), PAGE2 (reg_in_t515_ntsc_out_i80_320x240_page2[i]));
        READL (PAGE2 (reg_in_t515_ntsc_out_i80_320x240_page2[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
#endif
}

static void
scaler_setting_in_cvbs_ntsc_out_i80_320x240 (struct i80c *pi80)
{
#if 1
    int i;

    for (i = 0; i < NUM_REG_IN_CVBS_NTSC_OUT_I80_320X240_PAGE0; i++) {
        WRITEL (VALUE (reg_in_cvbs_ntsc_out_i80_320x240_page0[i]), PAGE0 (reg_in_cvbs_ntsc_out_i80_320x240_page0[i]));
        READL (PAGE0 (reg_in_cvbs_ntsc_out_i80_320x240_page0[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    for (i = 0; i < NUM_REG_IN_CVBS_NTSC_OUT_I80_320X240_PAGE2; i++) {
        WRITEL (VALUE (reg_in_cvbs_ntsc_out_i80_320x240_page2[i]), PAGE2 (reg_in_cvbs_ntsc_out_i80_320x240_page2[i]));
        READL (PAGE2 (reg_in_cvbs_ntsc_out_i80_320x240_page2[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
#endif
}

static void
scaler_setting_in_cvbs_pal_out_i80_320x240 (struct i80c *pi80)
{
#if 0
    int i;

    for (i = 0; i < NUM_REG_IN_CVBS_PAL_OUT_I80_320X240_PAGE0; i++) {
        WRITEL (VALUE (reg_in_cvbs_pal_out_i80_320x240_page0[i]), PAGE0 (reg_in_cvbs_pal_out_i80_320x240_page0[i]));
        READL (PAGE0 (reg_in_cvbs_pal_out_i80_320x240_page0[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    for (i = 0; i < NUM_REG_IN_CVBS_PAL_OUT_I80_320X240_PAGE2; i++) {
        WRITEL (VALUE (reg_in_cvbs_pal_out_i80_320x240_page2[i]), PAGE2 (reg_in_cvbs_pal_out_i80_320x240_page2[i]));
        READL (PAGE2 (reg_in_cvbs_pal_out_i80_320x240_page2[i]));
        usleep (1000);
    }
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif
#endif
}

/*
 * i80 controller setting functions
 */

static void
vidsrc_blank (struct i80c *pi80)
{
    unsigned char mark;

    /* reset fifo */
    WRITEL (FIFO_RESET, I80_R13);
    while (READL (I80_R13) & FIFO_RESET);

    /* black screen */
    WRITEL (0xa7, 0xb0400910);

    WRITEL (0x10, 0xb04009d0);  // P1_B8[8]=1, P0_9D,9E,9F=10,80,80 >> BLACK
    WRITEL (0x80, 0xb04009e0);
    WRITEL (0x80, 0xb04009f0);

    /* enable scaler free run pattern - En_psyn_str_swt for shadow cmd */
    WRITEL (0x92, 0xb0400c20);

#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    /* wait some frames until mode is changed */
    mark = READL (0xb0400d00) + WAIT_FRAMES;
    while ((unsigned char) (READL (0xb0400d00) - mark) > 0x1f);

    /* reset fifo */
    WRITEL (FIFO_RESET, I80_R13);
    while (READL (I80_R13) & FIFO_RESET);

    /* change panel freq */
    if (i80_vidsrc_get(pi80) == I80_VIDSRC_CMOS)
        WRITEL (0x0e, 0xb0400c10);      // 30fps
    else
        WRITEL (0x0c, 0xb0400c10);      // 60fps

#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    /* wait some frames until mode is changed */
    mark = READL (0xb0400d00) + WAIT_FRAMES;
    while ((unsigned char) (READL (0xb0400d00) - mark) > 0x1f);

    /* reset fifo */
    WRITEL (FIFO_RESET, I80_R13);
    while (READL (I80_R13) & FIFO_RESET);
}


/*
 * input select functions
 */
int
scaler_set_in_cpu_out_i80_320x240 (struct i80c *pi80)
{
    unsigned char mark;

    i80_vidsrc_set (pi80, I80_VIDSRC_CPU);

    vidsrc_blank (pi80);

    /* wait some frames until mode is changed */
    mark = READL (0xb0400d00) + WAIT_FRAMES;
    while ((unsigned char) (READL (0xb0400d00) - mark) > 0x1f);

    i80_set_cpu (pi80);

    return 0;
}

int
scaler_set_in_cmos_out_i80_320x240 (struct i80c *pi80)
{
    unsigned char mark;

    i80_vidsrc_set (pi80, I80_VIDSRC_CMOS);

    vidsrc_blank (pi80);

    /* config scaler */
    scaler_setting_in_cmos_out_i80_320x240 (pi80);

    /* wait some frames until mode is changed */
    mark = READL (0xb0400d00) + WAIT_FRAMES;
    while ((unsigned char) (READL (0xb0400d00) - mark) > 0x1f);

    /* config i80 controller */
    i80_set_cmos (pi80);

    /* disable black screen */
    WRITEL (0x07, 0xb0400910);
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    return 0;
}

int
scaler_set_in_cvbs_out_i80_320x240 (struct i80c *pi80, int format)
{
    unsigned char mark;

    i80_vidsrc_set (pi80, I80_VIDSRC_CVBS);

    vidsrc_blank (pi80);

    /* config scaler */
    if (format == 1) {
        /* VIDEO_STD_NTSC */
        scaler_setting_in_cvbs_ntsc_out_i80_320x240 (pi80);
    } else if (format == 2) {
        /* VIDEO_FMT_PAL */
        scaler_setting_in_cvbs_pal_out_i80_320x240 (pi80);
    }

    /* wait some frames until mode is changed */
    mark = READL (0xb0400d00) + WAIT_FRAMES;
    while ((unsigned char) (READL (0xb0400d00) - mark) > 0x1f);

    /* config i80 controller */
    i80_set_cvbs (pi80, format);

    /* disable black screen */
    WRITEL (0x07, 0xb0400910);
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    return 0;
}

int
scaler_set_in_t515_ntsc_out_i80_320x240 (struct i80c *pi80)
{
    i80_vidsrc_set (pi80, I80_VIDSRC_T515);

    vidsrc_blank (pi80);

    scaler_setting_in_t515_ntsc_out_i80_320x240 (pi80);

    i80_set_t515 (pi80);

    /* disable black screen */
    WRITEL (0x07, 0xb0400910);
#ifdef ENABLE_SHADOW
    WRITEL (0x11, 0xb0400e20);  // enable shadow
#endif

    return 0;
}

/* pre-play (reg LB_CONF): dto      [15: 0]
 *                         pre-fill [31:16]
 *          (reg TIMING) : H back-porch [ 7:0]
 *                         V vack-porch [13:8] */
#define PPLAY_LB_CONF_IN_CMOS_OUT_I80_320X240   ((0x2000 << 16) | (0xae7 & 0xffff))
#define PPLAY_LB_CONF_IN_CVBS_OUT_I80_320X240   ((0x1800 << 16) | (0x1616 & 0xffff))
#define PPLAY_TIMING_IN_CMOS_OUT_I80_320X240    0x101030
#define PPLAY_TIMING_IN_CVBS_OUT_I80_320X240    0x0406

int
scaler_set_in_pb_cmos_out_i80_320x240 (struct i80c *pi80, int format)
{
#if 0
    int i;

    /* set scaler reg */
    for (i = 0; i < NUM_REG_IN_PB_CMOS_OUT_I80_320X240_PAGE0; i++) {
        writel (VALUE (reg_in_pb_cmos_out_i80_320x240_page0[i]), PAGE0 (reg_in_pb_cmos_out_i80_320x240_page0[i]));
        readl (PAGE0 (reg_in_pb_cmos_out_i80_320x240_page0[i]));
        usleep(10);
    }
    writel(0x11, 0xb0400e20);
    
    /* set pre-play reg */
    writel(PPLAY_LB_CONF_IN_CMOS_OUT_I80_320X240, PPLAY_LB_CONF_REG);
    writel(PPLAY_TIMING_IN_CMOS_OUT_I80_320X240, PPLAY_TIMING_REG);

    i80_set_picview(pi80, format);

#endif
    return 0;
}


int
scaler_set_in_pb_cvbs_out_i80_320x240 (struct i80c *pi80, int format)
{
    if (format == 1) {
        /* VIDEO_STD_NTSC */
        writel(0x20 | readl(0xb04000e0), 0xb04000e0); // switch to jpeg decode
        writel(0x12, 0xb0400c20); // freerun on
        writel(0x20, 0xb0400750); 
        writel(0x11, 0xb0400e20);

        /* set pre-play reg */
        writel(PPLAY_LB_CONF_IN_CVBS_OUT_I80_320X240, PPLAY_LB_CONF_REG);
        writel(PPLAY_TIMING_IN_CVBS_OUT_I80_320X240, PPLAY_TIMING_REG);
    } else if (format == 2) {
        /* VIDEO_FMT_PAL */
    }

    i80_set_picview(pi80, format);

    return 0;
}

