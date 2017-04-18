/**
 *  @file   hx8347b.c
 *  @brief  hx8347b interface
 *  $Id: hx8347b.c,v 1.1.1.1 2013/12/18 03:43:53 jedy Exp $
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
hx8347b_set_backlight (struct i80c *pi80, int on)
{
    if (on) {
        WRITEL (0x81, 0xb0401550);
    } else {
        WRITEL (0x01, 0xb0401550);
    }

    return 0;
}

static int
hx8347b_get_backlight (struct i80c *pi80)
{
    return 0;
}

static int
hx8347b_set_vflip (struct i80c *pi80, int flip)
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
hx8347b_get_vflip (struct i80c *pi80)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 4 : 5;
    return ((val & (1 << shift)) != 0);
}

static int
hx8347b_set_hflip (struct i80c *pi80, int flip)
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
hx8347b_get_hflip (struct i80c *pi80)
{
    int val;
    int shift;

    val = i80_read_reg (pi80, 0x03);
    shift = val & (1 << 3) ? 5 : 4;
    return ((val & (1 << shift)) != 0);
}

static int
hx8347b_window (struct i80c *pi80, int x, int y, int w, int h)
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
hx8347b_locate (struct i80c *pi80, int x, int y)
{
    i80_write_reg (pi80, 0x20, y);
    i80_write_reg (pi80, 0x21, x);

    return 0;
}

static int
hx8347b_set_dispmode (struct i80c *pi80, int mode)
{
    switch (mode) {
        case I80_OUTPUT_VSYNC:
            i80_write_reg (pi80, 0x0c, 0x0020); // DM[1:0] = 10 (VSYNC mode)
            break;
        case I80_OUTPUT_RGB:
            break;
        case I80_OUTPUT_CMD:
        default:
            i80_write_reg (pi80, 0x0c, 0x0000); // DM[1:0] = 00 (command mode)
            break;
    }
    return 0;
}

static int
hx8347b_get_dispmode (struct i80c *pi80)
{
    int val;
    int mode;

    val = i80_read_reg (pi80, 0x0c);
    val = (val >> 4) & 0x3;
    switch (val) {
        case 0:
            mode = I80_OUTPUT_CMD;
            break;
        case 1:
            mode = I80_OUTPUT_RGB;
            break;
        case 2:
            mode = I80_OUTPUT_VSYNC;
            break;
        default:
            mode = val;
            break;
    }

    return mode;
}

static int
hx8347b_init_lcm (struct i80c *pi80)
{
    DBG_PRINT ("init HX8347B\n");

    //i80_write_reg (pi80, 0x0000, 0x0001);

    //------------ Start Initial Sequence -----------//
    i80_write_reg (pi80, 0x00e5, 0x78f0); // set SRAM internal timing
    //i80_write_reg (pi80, 0x00ef, 0x1221); 
    i80_write_reg (pi80, 0x0001, 0x0100); // set SS and SM bit
    i80_write_reg (pi80, 0x0002, 0x0700); // set 1 line inversion
    i80_write_reg (pi80, 0x0003, 0x1038); // set GRAM write direction and BGR=1
    i80_write_reg (pi80, 0x0004, 0x0000); // Resize register
    i80_write_reg (pi80, 0x0008, 0x0202); //set the back porch and front porch
    i80_write_reg (pi80, 0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    i80_write_reg (pi80, 0x000a, 0x0000); // FMARK function
    i80_write_reg (pi80, 0x000c, 0x0000); // RGB interface setting
    i80_write_reg (pi80, 0x000d, 0x0000); // Frame marker Position
    i80_write_reg (pi80, 0x000f, 0x0000); // RGB interface polarity

    //------------ Power On sequence ----------------//
    i80_write_reg (pi80, 0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    i80_write_reg (pi80, 0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    i80_write_reg (pi80, 0x0012, 0x0000); // VREG1OUT voltage
    i80_write_reg (pi80, 0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    i80_write_reg (pi80, 0x0007, 0x0001); // 
    usleep (50000);
    i80_write_reg (pi80, 0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
    i80_write_reg (pi80, 0x0011, 0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]
    usleep (50000);
    i80_write_reg (pi80, 0x0012, 0x009c); // External reference voltage= Vci
    usleep (50000);
    i80_write_reg (pi80, 0x0013, 0x1800); // VDV[4:0] for VCOM amplitude
    i80_write_reg (pi80, 0x0029, 0x002b); // VCM[5:0] for VCOMH
    i80_write_reg (pi80, 0x002b, 0x000d); // Set Frame Rate
    usleep (50000);
    i80_write_reg (pi80, 0x0020, 0x0000); // GRAM horizontal Address
    i80_write_reg (pi80, 0x0021, 0x0000); // GRAM Vertical Address

    //------------ Adjust the Gamma Curve ----------//
    i80_write_reg (pi80, 0x0030, 0x0000); 
    i80_write_reg (pi80, 0x0031, 0x0507);
    i80_write_reg (pi80, 0x0032, 0x0004);
    i80_write_reg (pi80, 0x0035, 0x0205);
    i80_write_reg (pi80, 0x0036, 0x0004);
    i80_write_reg (pi80, 0x0037, 0x0307);
    i80_write_reg (pi80, 0x0038, 0x0002);
    i80_write_reg (pi80, 0x0039, 0x0707);
    i80_write_reg (pi80, 0x003c, 0x0502);
    i80_write_reg (pi80, 0x003d, 0x0004);

    //------------- Set GRAM area --------------------//
    i80_write_reg (pi80, 0x0050, 0x0000); // Horizontal GRAM Start Address
    i80_write_reg (pi80, 0x0051, 0x00ef); // Horizontal GRAM End Address
    i80_write_reg (pi80, 0x0052, 0x0000); // Vertical GRAM Start Address
    i80_write_reg (pi80, 0x0053, 0x013f); // Vertical GRAM Start Address
    i80_write_reg (pi80, 0x0060, 0x2700); // Gate Scan Line
    i80_write_reg (pi80, 0x0061, 0x0001); // NDL,VLE, REV
    i80_write_reg (pi80, 0x006a, 0x0000); // set scrolling line

    //------------ Partial Display Control -----------//
    i80_write_reg (pi80, 0x0080, 0x0000);
    i80_write_reg (pi80, 0x0081, 0x0000);
    i80_write_reg (pi80, 0x0082, 0x0000);
    i80_write_reg (pi80, 0x0083, 0x0000);
    i80_write_reg (pi80, 0x0084, 0x0000);
    i80_write_reg (pi80, 0x0085, 0x0000);

    //------------ Panel Control ---------------------//
    i80_write_reg (pi80, 0x0090, 0x0010);
    i80_write_reg (pi80, 0x0092, 0x0600);
    i80_write_reg (pi80, 0x0007, 0x0133); // 262K color and display ON

    //i80_write_reg (pi80, 0x0022, 0x0000);

    /* init backlight */

    /* turn on backlight */

    return 0;
}

struct i80_lcm hx8347b = {
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
    .set_backlight =    hx8347b_set_backlight,
    .get_backlight =    hx8347b_get_backlight,
    .set_vflip =        hx8347b_set_vflip,
    .get_vflip =        hx8347b_get_vflip,
    .set_hflip =        hx8347b_set_hflip,
    .get_hflip =        hx8347b_get_hflip,
    .window =           hx8347b_window,
    .locate =           hx8347b_locate,
    .set_dispmode =     hx8347b_set_dispmode,
    .get_dispmode =     hx8347b_get_dispmode,
    .init_lcm =         hx8347b_init_lcm,
};
