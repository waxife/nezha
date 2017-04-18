/**
 *  @file   gui.c
 *  @brief  API of I80 system interface
 *  $Id: gui.c,v 1.1 2014/03/14 09:25:21 kevin Exp $
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

void
gui_line (struct i80c *pi80, int x0, int y0, int x1, int y1, int color)
{
    int xmax = pi80->lcm->xmax;
    int ymax = pi80->lcm->ymax;
    int x, y;
    int dx;          //=abs(x1-x0);
    int dy;          //=abs(y1-y0);

    x0 = MIN (xmax, MAX (x0, 0));
    x1 = MIN (xmax, MAX (x1, 0));
    y0 = MIN (ymax, MAX (y0, 0));
    y1 = MIN (ymax, MAX (y1, 0));

    if (y0 == y1) {
        if (x0 <= x1)
            x = x0;
        else {
            x = x1;
            x1 = x0;
        }
        while (x <= x1) {
            i80_set_point (x, y0, color);
            x++;
        }
        return;
    } else if (y0 > y1)
        dy = y0 - y1;
    else
        dy = y1 - y0;

    if (x0 == x1) {
        if (y0 <= y1)
            y = y0;
        else {
            y = y1;
            y1 = y0;
        }
        while (y <= y1) {
            i80_set_point (x0, y, color);
            y++;
        }
        return;
    } else if (x0 > x1) {
        dx = x0 - x1;
        x = x1;
        x1 = x0;
        y = y1;
        y1 = y0;
    } else {
        dx = x1 - x0;
        x = x0;
        y = y0;
    }

    if (dx == dy) {
        while (x <= x1) {
            x++;
            if (y > y1)
                y--;
            else
                y++;
            i80_set_point (x, y, color);
        }
    } else {
        i80_set_point (x, y, color);
        if (y < y1) {
            if (dx > dy) {
                short p = dy * 2 - dx;
                short twoDy = 2 * dy;
                short twoDyMinusDx = 2 * (dy - dx);
                while (x < x1) {
                    x++;
                    if (p < 0)
                        p += twoDy;
                    else {
                        y++;
                        p += twoDyMinusDx;
                    }
                    i80_set_point (x, y, color);
                }
            } else {
                short p = dx * 2 - dy;
                short twoDx = 2 * dx;
                short twoDxMinusDy = 2 * (dx - dy);
                while (y < y1) {
                    y--;
                    if (p < 0)
                        p += twoDx;
                    else {
                        x++;
                        p += twoDxMinusDy;
                    }
                    i80_set_point (x, y, color);
                }
            }
        }
    }
}

void
gui_circle (struct i80c *pi80, int c_x, int c_y, int r, int color, int fill)
{
    unsigned short x, y;
    short delta, tmp;
    x = 0;
    y = r;
    delta = 3 - (r << 1);

    while (y >= x) {
        if (fill) {
            gui_line (pi80, c_x + x, c_y + y, c_x - x, c_y + y, color);
            gui_line (pi80, c_x + x, c_y - y, c_x - x, c_y - y, color);
            gui_line (pi80, c_x + y, c_y + x, c_x - y, c_y + x, color);
            gui_line (pi80, c_x + y, c_y - x, c_x - y, c_y - x, color);
        } else {
            i80_set_point (c_x + x, c_y + y, color);
            i80_set_point (c_x - x, c_y + y, color);
            i80_set_point (c_x + x, c_y - y, color);
            i80_set_point (c_x - x, c_y - y, color);
            i80_set_point (c_x + y, c_y + x, color);
            i80_set_point (c_x - y, c_y + x, color);
            i80_set_point (c_x + y, c_y - x, color);
            i80_set_point (c_x - y, c_y - x, color);
        }
        x++;
        if (delta >= 0) {
            y--;
            tmp = (x << 2);
            tmp -= (y << 2);
            delta += (tmp + 10);
        } else
            delta += ((x << 2) + 6);
    }
}

void
gui_rectangle (struct i80c *pi80, int x, int y, int w, int h, int color, int fill)
{
    int x0, y0, x1, y1;

    x0 = x;
    y0 = y;
    x1 = x + w;
    y1 = y + h;

    if (fill) {
        int i;
        if (x0 > x1) {
            i = x1;
            x1 = x0;
        } else
            i = x0;
        for (; i <= x1; i++)
            gui_line (pi80, i, y0, i, y1, color);
        return;
    }
    gui_line (pi80, x0, y0, x0, y1, color);
    gui_line (pi80, x0, y1, x1, y1, color);
    gui_line (pi80, x1, y1, x1, y0, color);
    gui_line (pi80, x1, y0, x0, y0, color);
}

void
gui_square (struct i80c *pi80, int x, int y, int len, int color, int fill)
{
    gui_rectangle (pi80, x, y, len, len, color, fill);
}

#include "font.h"
static void
gui_put_char (struct i80c *pi80, int x_pos, int y_pos, char c, int fg_color, int bg_color)
{
    int i, j, x, y;
    unsigned char tmp;

    for (j = 0; j < 16; j++) {
        tmp = ascii_8x16[((c - 0x20) * 16) + j];
        for (i = 0; i < 8; i++) {
            x = x_pos + i;
            y = y_pos + j;
            if (tmp & (1 << (7 - i))) {
                if (fg_color >= 0)
                    i80_set_point ( x, y, fg_color);
            } else {
                if (bg_color >= 0)
                    i80_set_point ( x, y, bg_color);
            }
        }
    }
}

void
gui_text (struct i80c *pi80, int x, int y, char *str, int fg_color, int bg_color)
{
    while (*str != '\0') {
        gui_put_char (pi80, x, y, *str, fg_color, bg_color);
        x += 8;
        str++;
    }
}

/* YUV422 to RGB */
#define SCALEBITS	10
#define FIX(x)		((int)((x)*(1L << SCALEBITS) + 0.5))

static int inline
clip255 (int x)
{
    return (x > 255) ? 255 : ((x < 0) ? 0 : x);
}

static int
yuv422_to_rgb666 (struct i80c *pi80, void *yuvbuf, int x, int y, int width, int height)
{
    unsigned char *yuv = (unsigned char *) yuvbuf;
    int Y, U, V;
    unsigned char R, G, B;

    if (x & 1) {
        /* odd */
        yuv += ((x - 1) + y * width) * 2;
        Y = (int) yuv[3];
        V = (int) yuv[0];
        U = (int) yuv[2];
    } else {
        /* even */
        yuv += ((x) + y * width) * 2;
        Y = (int) yuv[1];
        V = (int) yuv[0];
        U = (int) yuv[2];
    }

    V -= 128;
    U -= 128;

    R = clip255 (Y + ((FIX (1.402) * V) >> SCALEBITS));
    G = clip255 (Y - ((FIX (0.34414) * U + FIX (0.71414) * V) >> SCALEBITS));
    B = clip255 (Y + ((FIX (1.772) * U) >> SCALEBITS));

    /* 18-bit RGB666 format */
    return ((R >> 2) << 12) | ((G >> 2) << 6) | ((B >> 2) << 0);
}

void
gui_draw_yuv (struct i80c *pi80, int x, int y, int w, int h, void *src)
{
    int i, j;
    int rgb666;

    i80_window (pi80, x, y, w, h);
    i80_burst_setup (pi80, w * h);

    for (j = y; j < (y + h); j++) {
        for (i = x; i < (x + w); i++) {
            rgb666 = yuv422_to_rgb666 (pi80, src, i, j, 320, 240);
            i80_burst_write (pi80, rgb666);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
gui_dump (struct i80c *pi80, int x, int y, int w, int h)
{
    int i, j;
    int rgb666, rgb565, rgb888;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            rgb666 = i80_get_point (i + x, j + y);
            rgb565 = RGB666_TO_RGB565 (rgb666);
            rgb888 = RGB666_TO_RGB888 (rgb666);

            printf ("%4u%4u  %04X  %05X  %06X\n", i, j, rgb565, rgb666, rgb888);
        }
    }
}

void
gui_read_rgb565 (struct i80c *pi80, int x, int y, int w, int h, void *dst)
{
    int i, j;
    int rgb666, rgb565;
    unsigned char *ptr;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            rgb666 = i80_get_point ( i + x, j + y);
            rgb565 = RGB666_TO_RGB565 (rgb666);

            /* little endian */
            ptr = dst + (i + j * w) * 2;
            ptr[0] = rgb565 >> 0;
            ptr[1] = rgb565 >> 8;
        }
    }
}

void
gui_write_rgb565 (struct i80c *pi80, int x, int y, int w, int h, void *src)
{
    int i, j;
    int rgb666, rgb565;
    unsigned char *ptr;


    i80_window (pi80, x, y, w, h);
    i80_burst_setup (pi80, w * h);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            /* little endian */
            ptr = src + (i + j * w) * 2;
            rgb565 = (ptr[0] << (8 * 0)) | (ptr[1] << (8 * 1));

            rgb666 = RGB565_TO_RGB666 (rgb565);
            i80_burst_write (pi80, rgb666);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}

void
gui_read_rgb888 (struct i80c *pi80, int x, int y, int w, int h, void *dst)
{
    int i, j;
    int rgb666, rgb888;
    unsigned char *ptr;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            rgb666 = i80_get_point ( i + x, j + y);
            rgb888 = RGB666_TO_RGB888 (rgb666);

            /* little endian */
            ptr = dst + (i + j * w) * 3;
            ptr[0] = rgb888 >> (8 * 0);
            ptr[1] = rgb888 >> (8 * 1);
            ptr[2] = rgb888 >> (8 * 2);
        }
    }
}

void
gui_write_rgb888 (struct i80c *pi80, int x, int y, int w, int h, void *src)
{
    int i, j;
    int rgb666, rgb888;
    unsigned char *ptr;

    i80_window (pi80, x, y, w, h);
    i80_burst_setup (pi80, w * h);

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            /* little endian */
            ptr = src + (i + j * w) * 3;
            rgb888 = (ptr[0] << (8 * 0)) | (ptr[1] << (8 * 1)) | (ptr[2] << (8 * 2));

            rgb666 = RGB888_TO_RGB666 (rgb888);
            i80_burst_write (pi80, rgb666);
        }
    }
    assert (i80_burst_write_is_done (pi80));
}
