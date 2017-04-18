/**
 *  @file   ili9327.c
 *  @brief  ili9327 interface
 *  $Id: ili9327.c,v 1.1.1.1 2013/12/18 03:43:53 jedy Exp $
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

#define XMAX    400
#define YMAX    240

static int
ili9327_set_backlight (struct i80c *pi80, int on)
{
    /* TODO */
    return 0;
}

static int
ili9327_get_backlight (struct i80c *pi80)
{
    /* TODO */
    return 0;
}

/*
 * R36(W), R0B(R) - Address Mode
 *  [7] Page Address Order
 *  [6] Column Address Order
 *  [5] Page/Column Selection
 *  [4] Vertical Order
 *  [3] RGB/BGR Order
 *  [2] Display data latch order
 *  [1] Horizontal Flip
 *  [0] Vertical Flip
 */
#define AM_NORMAL       (1 << 5)
#define AM_HFLIP        (1 << 1)
#define AM_VFLIP        (1 << 0)

static int
ili9327_set_vflip (struct i80c *pi80, int on)
{
    unsigned char val[1];

    /* get address mode */
    i80_read_regv (pi80, 0x0B, val, 1);

    /* set address mode */
    val[0] = (val[0] & ~AM_VFLIP) | (on ? AM_VFLIP : 0);
    i80_write_regv (pi80, 0x36, val, 1);

    pi80->vflip = (on ? VFLIP_ON : VFLIP_OFF);
    return 0;
}

static int
ili9327_get_vflip (struct i80c *pi80)
{
    return ((pi80->vflip & VFLIP_ON) != 0);
}

static int
ili9327_set_hflip (struct i80c *pi80, int on)
{
    unsigned char val[1];

    /* get address mode */
    i80_read_regv (pi80, 0x0B, val, 1);

    /* set address mode */
    val[0] = (val[0] & ~AM_HFLIP) | (on ? AM_HFLIP : 0);
    i80_write_regv (pi80, 0x36, val, 1);

    pi80->hflip = (on ? HFLIP_ON : HFLIP_OFF);
    return 0;
}

static int
ili9327_get_hflip (struct i80c *pi80)
{
    return ((pi80->hflip & HFLIP_ON) != 0);
}

static int
ili9327_window (struct i80c *pi80, int x, int y, int w, int h)
{
    unsigned char val[4];
    int xs = x;
    int ys = y;
    int xe = x + w - 1;
    int ye = y + h - 1;

    /* setup column adderss */
    val[0] = (xs >> 8) & 0xff;
    val[1] = (xs >> 0) & 0xff;
    val[2] = (xe >> 8) & 0xff;
    val[3] = (xe >> 0) & 0xff;
    i80_write_regv (pi80, 0x2A, val, 4);

    /* setup page valess */
    val[0] = (ys >> 8) & 0xff;
    val[1] = (ys >> 0) & 0xff;
    val[2] = (ye >> 8) & 0xff;
    val[3] = (ye >> 0) & 0xff;
    i80_write_regv (pi80, 0x2B, val, 4);

    return 0;
}
static int
ili9327_locate (struct i80c *pi80, int x, int y)
{
    ili9327_window (pi80, x, y, 1, 1);
    return 0;
}


static int
ili9327_init_lcm (struct i80c *pi80)
{
    DBG_PRINT ("init ILI9327\n");

    /* exit sleep */
    unsigned char data_e9[] = {0x20};
    i80_write_regv (pi80, 0x00e9, data_e9, 1);

    unsigned char data_11[] = {};
    i80_write_regv (pi80, 0x0011, data_11, 0);

    usleep (100);

    /* init lcm */
    unsigned char data_d1[] = {0x00, 0x5b, 0x15};
    i80_write_regv (pi80, 0x00d1, data_d1, 3);

    unsigned char data_d0[] = {0x07, 0x02, 0x88};
    i80_write_regv (pi80, 0x00d0, data_d0, 3);

    unsigned char data_36[] = {0x08};
    i80_write_regv (pi80, 0x0036, data_36, 1);

    unsigned char data_c1[] = {0x10, 0x10, 0x02, 0x02};
    i80_write_regv (pi80, 0x00c1, data_c1, 4);

    unsigned char data_c0[] = {0x00, 0x35, 0x00, 0x00, 0x01, 0x02};
    i80_write_regv (pi80, 0x00c0, data_c0, 6);

    unsigned char data_c5[] = {0x02};
    i80_write_regv (pi80, 0x00c5, data_c5, 1);

    unsigned char data_d2[] = {0x01, 0x44};
    i80_write_regv (pi80, 0x00d2, data_d2, 2);

    unsigned char data_c8[] = {0x00, 0x56, 0x45, 0x04, 0x03, 0x01, 0x23, 0x12,
                               0x77, 0x40, 0x09, 0x06, 0x88, 0x88, 0x88};
    i80_write_regv (pi80, 0x00c8, data_c8, 15);

    unsigned char data_ea[] = {0x80};
    i80_write_regv (pi80, 0x00ea, data_ea, 1);

    unsigned char data_29[] = {};
    i80_write_regv (pi80, 0x0029, data_29, 0);

    data_36[0] = 0x28;
    i80_write_regv (pi80, 0x0036, data_36, 1);

    unsigned char data_3a[] = {0x05};
    i80_write_regv (pi80, 0x003a, data_3a, 1);

    unsigned char data_2c[] = {0x00, 0x00};
    i80_write_regv (pi80, 0x002c, data_2c, 2);

    /* init backlight */

    return 0;
}

struct i80_lcm ili9327 = {
    .xmax =             XMAX,
    .ymax =             YMAX,
    .wmi =              0x002C,
    .rmi =              0x002E,
    .hac =              1,
    .hrd =              1,
    .hmd =              1,
    .fac =              1,
    .frd =              1,
    .fmd =              1,
    .r00 =              0x002C0403, // WR_IDX=0x002C, INDEX_SWAP=1, BUS_8BIT
    .r01 =              0x002A0000, // HGRAM_AD_INDEX=0x002a
    .r02 =              0x002B0000, // VGRAM_AD_INDEX=0X002b
    .r03 =              0x030D030D,
    .r05 =              0x00000000,
    .r17 =              0x00000004, // END_ADDRESS_EN=1
    .r18 =              0x018F00EF, // HGRAM_AD_END=0x018F, VGRAM_AD_END=0x00EF
    .set_backlight =    ili9327_set_backlight,
    .get_backlight =    ili9327_get_backlight,
    .set_vflip =        ili9327_set_vflip,
    .get_vflip =        ili9327_get_vflip,
    .set_hflip =        ili9327_set_hflip,
    .get_hflip =        ili9327_get_hflip,
    .window =           ili9327_window,
    .locate =           ili9327_locate,
    .init_lcm =         ili9327_init_lcm,
};
