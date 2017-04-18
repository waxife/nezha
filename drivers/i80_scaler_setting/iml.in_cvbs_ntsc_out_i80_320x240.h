/**
 *  @file   iml.in_cvbs_ntsc_out_i80_320x240.h
 *  @brief  scaler register setting for cvbs ntsc input, output i80 320x240
 *  $Id: iml.in_cvbs_ntsc_out_i80_320x240.h,v 1.2 2014/01/08 03:26:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/01/10  Jonathan  New file.
 *
 */
#ifndef __IML_IN_CVBS_NTSC_OUT_I80_320X240_H
#define __IML_IN_CVBS_NTSC_OUT_I80_320X240_H

#include "config.h"
#include "iml.h"

/* crystal 27MHz */
/* system clock: 108M Hz */
static const struct reg reg_in_cvbs_ntsc_out_i80_320x240_page0[] = {
    /* ADC */
    {0x0A, 0x30},
    /* source select */
    /* CVBS input */
    {0x0E, 0x03},
    {0x0F, 0x02},
    /* Scaler */
    {0x70, 0x10},
    {0x72, 0x00},
    {0x73, 0x48},
    {0x85, 0x10},
    /* Output timing */
    {0xB0, 0x08},
    {0xB1, 0x00},
    {0xB2, 0x10},
    {0xB3, 0x00},
    {0xB4, 0x40},
    {0xB5, 0x01},
    {0xB6, 0xF0},
    {0xB7, 0x00},
    {0xB8, 0xA9},
    {0xB9, 0x01},
    {0xBA, 0x00},
    {0xBB, 0x01},
    {0xBC, 0x03},
    {0xBD, 0x00},
    {0xBE, 0x03},
    {0xBF, 0x00},
    /* freerun off */
    {0xC2, 0x00},
    /* dpll divider */
    {0xCB, 0x1A},
    /* main fetch size */
    {0xDC, 0x40},
    {0xDD, 0x01},
    {0xDE, 0xF0},
    {0xDF, 0x00},
    /* ttl output off */
    {0xFE, 0x00},
};

#define NUM_REG_IN_CVBS_NTSC_OUT_I80_320X240_PAGE0 (sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page0) / sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page0[0]))

#if 0
static const struct reg reg_in_cvbs_ntsc_out_i80_320x240_page1[] = {
    /* PWM23 */
    {0x62, 0x80},
    {0x63, 0x12},
    {0x64, 0x00},
    {0x65, 0x80},
    {0x66, 0x08},
    {0x67, 0x00},
    {0x68, 0x16},
    {0x69, 0x0d},
    {0x6a, 0x30},
    {0x6b, 0x05},
    {0x6c, 0x60},
};

#define NUM_REG_IN_CVBS_NTSC_OUT_I80_320X240_PAGE1 (sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page1) / sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page1[0]))

#endif

static const struct reg reg_in_cvbs_ntsc_out_i80_320x240_page2[] = {
    /* CVD on */
    {0x3F, 0x01},
    {0x3F, 0x00}
};

#define NUM_REG_IN_CVBS_NTSC_OUT_I80_320X240_PAGE2 (sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page2) / sizeof(reg_in_cvbs_ntsc_out_i80_320x240_page2[0]))

#endif /* __IML_IN_CVBS_NTSC_OUT_I80_320X240_H */
