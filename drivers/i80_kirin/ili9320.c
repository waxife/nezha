/**
 *  @file   ili9320.c
 *  @brief  ili9320 interface
 *  $Id: ili9320.c,v 1.1.1.1 2013/12/18 03:43:53 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2012/12/19  Hugo  New file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <sys.h>
#include <debug.h>

#include "i80.h"

#define XMAX    320
#define YMAX    240

static int
ili9320_set_backlight (struct i80c *pi80, int on)
{
    if (on) {
        writel (0x81, 0xb0401550);
    } else {
        writel (0x01, 0xb0401550);
    }

    return 0;
}

static int
ili9320_get_backlight (struct i80c *pi80)
{
    return ((readl (0xb0401550) & 0x80) != 0);
}

static int
ili9320_set_vflip (struct i80c *pi80, int flip)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 4 : 5;
    val = (val & ~(1 << shift)) | ((flip != 0) << shift);
    i80_write_reg (pi80, 0x03, val);

    return 0;
}

static int
ili9320_get_vflip (struct i80c *pi80)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 4 : 5;
    return (val & (1 << shift));
}

static int
ili9320_set_hflip (struct i80c *pi80, int flip)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 5 : 4;
    val = (val & ~(1 << shift)) | ((flip != 0) << shift);
    i80_write_reg (pi80, 0x03, val);

    return 0;
}

static int
ili9320_get_hflip (struct i80c *pi80)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 5 : 4;
    return (val & (1 << shift));
}

static int
ili9320_window (struct i80c *pi80, int x, int y, int w, int h)
{
    i80_write_reg (pi80, 0x50, y);
    i80_write_reg (pi80, 0x52, x);
    i80_write_reg (pi80, 0x51, y + h - 1);
    i80_write_reg (pi80, 0x53, x + w - 1);

    i80_write_reg (pi80, 0x20, y);
    i80_write_reg (pi80, 0x21, x);

    return 0;
}

static int
ili9320_locate (struct i80c *pi80, int x, int y)
{
    i80_write_reg (pi80, 0x20, y);
    i80_write_reg (pi80, 0x21, x);

    return 0;
}

static void
ili9320_backlight_init (struct i80c *pi80)
{
    // drive PWM3 gradually
    writel (0x80, 0xb0400e00);
    writel (0x08, 0xb0401560);
    writel (0x10, 0xb0401560);
    writel (0x12, 0xb0401560);
    writel (0x14, 0xb0401560);
    writel (0x16, 0xb0401560);
    writel (0x18, 0xb0401560);
    writel (0x1C, 0xb0401560);
    writel (0x00, 0xb0400e20);
    writel (0x00, 0xb0401550);
}

static int
ili9320_init_lcm (struct i80c *pi80)
{
    DBG_PRINT ("init ILI9320\n");

    /* Start Oscillation */
    i80_write_reg (pi80, 0x00, 0x0001);
    latch_delay (10);

    /* Driver Control */
    i80_write_reg (pi80, 0x01, 0x0100);
    i80_write_reg (pi80, 0x02, 0x0700);
    i80_write_reg (pi80, 0x03, 0x1038);
    i80_write_reg (pi80, 0x04, 0x0000);

    /* Display Control */
    i80_write_reg (pi80, 0x08, 0x0808); // FP=8, BP=8
    i80_write_reg (pi80, 0x09, 0x0000);
    i80_write_reg (pi80, 0x0a, 0x0000);
    i80_write_reg (pi80, 0x0c, 0x0001);
    i80_write_reg (pi80, 0x0d, 0x0000);
    i80_write_reg (pi80, 0x0f, 0x0000);

    /* Power Control */
    i80_write_reg (pi80, 0x10, 0x0000);
    i80_write_reg (pi80, 0x11, 0x0007);
    i80_write_reg (pi80, 0x12, 0x0000);
    i80_write_reg (pi80, 0x13, 0x0000);
    latch_delay (2500);

    i80_write_reg (pi80, 0x10, 0x17b0);
    i80_write_reg (pi80, 0x11, 0x0002);
    latch_delay (550);
    i80_write_reg (pi80, 0x12, 0x0139);
    latch_delay (550);
    i80_write_reg (pi80, 0x13, 0x0900);
    i80_write_reg (pi80, 0x29, 0x0000);
    latch_delay (550);

    /* Frame Rate & Color Control */
    i80_write_reg (pi80, 0x2b, 0x000c); // FRS[3:0] = 1100 (80fps)
    latch_delay (550);

    /* GARM Address */
    i80_write_reg (pi80, 0x20, 0x0000);
    i80_write_reg (pi80, 0x21, 0x0000);

    /* Camma Control */
    i80_write_reg (pi80, 0x30, 0x0006);
    i80_write_reg (pi80, 0x31, 0x0101);
    i80_write_reg (pi80, 0x32, 0x0003);
    i80_write_reg (pi80, 0x35, 0x0106);
    i80_write_reg (pi80, 0x36, 0x0b02);
    i80_write_reg (pi80, 0x37, 0x0302);
    i80_write_reg (pi80, 0x38, 0x0707);
    i80_write_reg (pi80, 0x39, 0x0007);
    i80_write_reg (pi80, 0x3C, 0x0600);
    i80_write_reg (pi80, 0x3D, 0x020b);

    /* Window Address */
    i80_write_reg (pi80, 0x50, 0x0000);
    i80_write_reg (pi80, 0x51, 0x00ef);
    i80_write_reg (pi80, 0x52, 0x0000);
    i80_write_reg (pi80, 0x53, 0x013f);

    /* Gate Scan Control */
    i80_write_reg (pi80, 0x60, 0x2700);
    i80_write_reg (pi80, 0x61, 0x0001);
    i80_write_reg (pi80, 0x6a, 0x0000);

    /* Partial Image */
    i80_write_reg (pi80, 0x80, 0x0000);
    i80_write_reg (pi80, 0x81, 0x0000);
    i80_write_reg (pi80, 0x82, 0x0000);
    i80_write_reg (pi80, 0x83, 0x0000);
    i80_write_reg (pi80, 0x84, 0x0000);
    i80_write_reg (pi80, 0x85, 0x0000);

    /* Panel Interface Control */
    i80_write_reg (pi80, 0x90, 0x0010);
    i80_write_reg (pi80, 0x92, 0x0600);
    i80_write_reg (pi80, 0x93, 0x0003);
    i80_write_reg (pi80, 0x95, 0x0110);
    i80_write_reg (pi80, 0x97, 0x0000);
    i80_write_reg (pi80, 0x98, 0x0000);

    /* Display Control */
    i80_write_reg (pi80, 0x07, 0x0133);

    /* VSYNC for CVBS */
    writel (0x04, 0xb9000054);  // bit[2]   i80 master vsync output
    // bit[3]   i80 slave output
    // bit[7:4] display clock selection

    ili9320_backlight_init (pi80);
    ili9320_set_backlight (pi80, 1);

    return 0;
}

struct i80_lcm ili9320 = {
    .xmax =             XMAX,
    .ymax =             YMAX,
    .wmi =              0x0022,
    .rmi =              0x0022,
    .hac =              2,
    .hrd =              0,
    .hmd =              2,
    .fac =              1,
    .frd =              0,
    .fmd =              1,
    .r00 =              0x00220003, // WR_IDX=0x0022, INDEX_SWAP=0, BUS_8BIT
    .r01 =              0x00200000, // HGRAM_AD_INDEX=0x0020
    .r02 =              0x00210000, // VGRAM_AD_INDEX=0X0021
    .r03 =              0x030D030D,
    .r05 =              0x00000000,
    .r17 =              0x00000000, // END_ADDRESS_EN=0
    .r18 =              0x00000000,
    .set_backlight =    ili9320_set_backlight,
    .get_backlight =    ili9320_get_backlight,
    .set_vflip =        ili9320_set_vflip,
    .get_vflip =        ili9320_get_vflip,
    .set_hflip =        ili9320_set_hflip,
    .get_hflip =        ili9320_get_hflip,
    .window =           ili9320_window,
    .locate =           ili9320_locate,
    .init_lcm =         ili9320_init_lcm,
};
