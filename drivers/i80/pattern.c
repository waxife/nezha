/**
 *  @file   pattern.c
 *  @brief  test pattern for i80 lcm
 *  $Id: pattern.c,v 1.1 2014/03/14 09:25:21 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
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
#include <debug.h>

#include "i80.h"

const static unsigned int
RGB666_PATTERN[8] = {
    RGB666_WHITE,   RGB666_YELLOW,  RGB666_CYAN,    RGB666_GREEN,
    RGB666_MAGENTA, RGB666_RED,     RGB666_BLUE,    RGB666_BLACK,
};

void
i80_colorbar (struct i80c *pi80, int invert)
{
    struct i80_lcm *lcm = pi80->lcm;
    int xslice = lcm->xmax / 8;
    int yslice = lcm->ymax / 8, yslice7 = yslice * 7;
    int x, y;
    unsigned int rgb;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            rgb = RGB666_PATTERN[x / xslice];

            if (y > yslice7)
                rgb = ~rgb;

            if (invert)
                rgb = ~rgb;

            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

#define W0	(lcm->xmax * 0 / 6)
#define W1	(lcm->xmax * 1 / 6)
#define W2	(lcm->xmax * 2 / 6)
#define W3	(lcm->xmax * 3 / 6)
#define W4	(lcm->xmax * 4 / 6)
#define W5	(lcm->xmax * 5 / 6)
#define W6	(lcm->xmax * 6 / 6)
void
i80_pcolorbar (struct i80c *pi80, int invert)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int r, g, b, rgb, scale;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        scale = 255 * (lcm->ymax - y) / lcm->ymax;
        for (x = 0; x < lcm->xmax; x++) {
            if (x >= W0 && x < W1) {
                r = scale;
                g = (x - W0) * scale * 6 / lcm->xmax;
                b = 0;
            } else if (x >= W1 && x < W2) {
                r = (W2 - x - 1) * scale * 6 / lcm->xmax;
                g = scale;
                b = 0;
            } else if (x >= W2 && x < W3) {
                r = 0;
                g = scale;
                b = (x - W2) * scale * 6 / lcm->xmax;
            } else if (x >= W3 && x < W4) {
                r = 0;
                g = (W4 - x - 1) * scale * 6 / lcm->xmax;
                b = scale;
            } else if (x >= W4 && x < W5) {
                r = (x - W4) * scale * 6 / lcm->xmax;
                g = 0;
                b = scale;
            } else if (x >= W5 && x < W6) {
                r = scale;
                g = 0;
                b = (W6 - x - 1) * scale * 6 / lcm->xmax;
            } else
                continue;

            /* rgb888 >> rgb666 */
            r >>= 2;
            g >>= 2;
            b >>= 2;
            rgb = (r << 12) | (g << 6) | (b << 0);

            if (invert)
                rgb = ~rgb;

            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
i80_white_ramp (struct i80c *pi80, int down)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int val, rgb;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            val = x & 0x3f;
            if (down)
                val = ~val & 0x3f;
            rgb = (val << 12) | (val << 6) | (val << 0);
            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
i80_red_ramp (struct i80c *pi80, int down)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int val, rgb;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            val = x & 0x3f;
            if (down)
                val = ~val & 0x3f;
            rgb = (0x3f << 12) | (val << 6) | (val << 0);
            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
i80_green_ramp (struct i80c *pi80, int down)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int val, rgb;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            val = x & 0x3f;
            if (down)
                val = ~val & 0x3f;
            rgb = (val << 12) | (0x3f << 6) | (val << 0);
            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
i80_blue_ramp (struct i80c *pi80, int down)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int val, rgb;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            val = x & 0x3f;
            if (down)
                val = ~val & 0x3f;
            rgb = (val << 12) | (val << 6) | (0x3f << 0);
            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
i80_chess_board (struct i80c *pi80, int size)
{
    struct i80_lcm *lcm = pi80->lcm;
    int x, y;
    unsigned int rgb;

    if (size < 1)
        size = 1;

    i80_window (pi80, 0, 0, lcm->xmax, lcm->ymax);
    i80_burst_setup (pi80, lcm->xmax * lcm->ymax);

    for (y = 0; y < lcm->ymax; y++) {
        for (x = 0; x < lcm->xmax; x++) {
            if (((x / size) % 2) ^ ((y / size) % 2))
                rgb = 0x3ffff;
            else
                rgb = 0x00000;
            i80_burst_write (pi80, rgb);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}
