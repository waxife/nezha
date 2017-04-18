/**
 *  @file   sh_i80_kirin.c
 *  @brief  i80 shell commands (via Kirin I80 master)
 *  $Id: sh_i80_kirin.c,v 1.1.1.1 2013/12/18 03:43:34 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2012/12/19  Hugo  New file.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <debug.h>
#include <sys.h>
#include <unistd.h>
#include "drivers/i80_kirin/i80.h"

#define REG(name, idx) \
command_init (sh_r ##name, "r" #name, "r" #name "[<val>]"); \
static int sh_r ##name (int argc, char **argv, void **result, void *sys_ctx) { \
    unsigned int val; \
    if (argc > 1) { \
        val = strtol (argv[1], NULL, 16); \
        writel (val, 0xbb000000 | (idx << 4)); \
    } else {\
        val = readl (0xbb000000 | (idx << 4)); \
        printf ("r" #name ": 0x%08x\n", val); \
    } \
    return 0; \
}

REG(00, 0x00)
REG(01, 0x01)
REG(02, 0x02)
REG(03, 0x03)
REG(04, 0x04)
REG(05, 0x05)
REG(06, 0x06)
REG(10, 0x10)
REG(11, 0x11)
REG(12, 0x12)
REG(13, 0x13)
REG(14, 0x14)
REG(15, 0x15)
REG(16, 0x16)
REG(17, 0x17)
REG(18, 0x18)

/*****************************************************************************
 * I80 Controller
 *****************************************************************************/

command_init (sh_i80_data_width, "i80_data_width", "i80_data_width [16|18]");
command_init (sh_i80_bus_width, "i80_bus_width", "i80_bus_width [8|9|16|18]");
command_init (sh_i80_reg, "i80_reg", "i80_reg <idx> [<val>]");
command_init (sh_i80_regv, "i80_regv", "i80_regv <idx> <cnt> [<b1> <b2> ...]");
command_init (sh_i80_input, "i80_input", "i80_input [0:cpu|1:scaler]");
command_init (sh_i80_output, "i80_output", "i80_output [0:cmd|1:vsync|2:rgb]");
command_init (sh_i80_debug, "i80_debug", "i80_debug [0|1]");
command_init (sh_i80_reset, "i80_reset", "i80_reset");
command_init (sh_i80_init, "i80_init", "i80_init");

static int
sh_i80_data_width (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        if (val != 16 && val != 18)
            goto EXIT;
        i80_set_data_width (pi80, val);
    } else {
        val = i80_get_data_width (pi80);
        printf ("data_width: %d\n", val);
    }

    return 0;
EXIT:
    print_usage (sh_i80_data_width);
    return -1;
}

static int
sh_i80_bus_width (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        if (val != 8 && val != 9 && val != 16 && val != 18)
            goto EXIT;
        i80_set_bus_width (pi80, val);
    } else {
        val = i80_get_bus_width (pi80);
        printf ("bus_width: %d\n", val);
    }

    return 0;
EXIT:
    print_usage (sh_i80_bus_width);
    return -1;
}

static int
sh_i80_reg (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short idx;
    unsigned short val;
    int i;

    if (argc < 2)
        goto EXIT;

    idx = strtol (argv[1], NULL, 16);

    if (argc > 2) {
        val = strtol (argv[2], NULL, 16);
        i80_write_reg (pi80, idx, val);
    } else {
        val = i80_read_reg (pi80, idx);

        /* show register and value */
        printf ("R%04X = 0x%04X (", idx, val);
        for (i = 15; i >= 0; i--) {
            printf ("%d", (val & (1 << i)) != 0);
            if (i && i % 4 == 0)
                printf (" ");
        }
        printf (")\n");
    }

    return 0;
EXIT:
    print_usage (sh_i80_reg);
    return -1;
}

static int
sh_i80_regv (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short idx;
    unsigned char cnt;
    unsigned char val[16];
    int i;

    if (argc < 3)
        goto EXIT;

    idx = strtol (argv[1], NULL, 16);
    cnt = atoi (argv[2]);

    if (argc > 3) {
        if (argc < (cnt + 3))
            goto EXIT;

        for (i = 0; i < cnt; i++)
            val[i] = strtol (argv[3+i], NULL, 16);

        i80_write_regv (pi80, idx, val, cnt);
    } else {
        i80_read_regv (pi80, idx, val, cnt);

        /* show register and value */
        printf ("R%02X = ", idx);
        for (i = 0; i < cnt; i++)
            printf ("0x%02X ", val[i]);
        printf ("\n");
    }

    return 0;
EXIT:
    print_usage (sh_i80_regv);
    return -1;
}

static int
sh_i80_input (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_input (pi80, val);
    } else {
        val = i80_get_input (pi80);
        printf ("input: %d\n", val);
    }

    return 0;
}

static int
sh_i80_output (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_output (pi80, val);
    } else {
        val = i80_get_output (pi80);
        printf ("output: %d\n", val);
    }

    return 0;
}

static int
sh_i80_debug (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_debug (pi80, val);
    } else {
        val = i80_get_debug (pi80);
        printf ("debug: %d\n", val);
    }

    return 0;
}

static int
sh_i80_reset (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;

    i80_reset (pi80);
    return 0;
}

static int
sh_i80_init (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;

    i80_init (pi80);
    return 0;
}


/*****************************************************************************
 * I80 Panel
 *****************************************************************************/

command_init (sh_i80_init_lcm, "i80_init_lcm", "i80_init_lcm");
command_init (sh_i80_backlight, "i80_backlight", "i80_backlight [0|1]");
command_init (sh_i80_clear, "i80_clear", "i80_clear [<rgb888>]");
command_init (sh_i80_vflip, "i80_vflip", "i80_vflip [0|1]");
command_init (sh_i80_hflip, "i80_hflip", "i80_hflip [0|1]");
command_init (sh_i80_point, "i80_point", "i80_point <x> <y> <rgb888>");
command_init (sh_i80_frate, "i80_frate", "i80_frate");

static int
sh_i80_init_lcm (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;

    i80_init_lcm (pi80);
    return 0;
}

static int
sh_i80_backlight (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_backlight (pi80, val);
    } else {
        val = i80_get_backlight (pi80);
        printf ("backlight: %d\n", val);
    }

    return 0;
}

static int
sh_i80_clear (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned int rgb666, rgb888;

    rgb888 = argc > 1 ? strtol (argv[1], NULL, 16) : RGB888_BG;
    rgb666 = RGB888_TO_RGB666 (rgb888);
    i80_clear (pi80, rgb666);

    return 0;
}

static int
sh_i80_vflip (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_vflip (pi80, val);
    } else {
        val = i80_get_vflip (pi80);
        printf ("vflip: %d\n", val);
    }

    return 0;
}

static int
sh_i80_hflip (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    if (argc > 1) {
        val = atoi (argv[1]);
        i80_set_hflip (pi80, val);
    } else {
        val = i80_get_hflip (pi80);
        printf ("hflip: %d\n", val);
    }

    return 0;
}

static int
sh_i80_point (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y;
    unsigned int rgb565, rgb666, rgb888;

    if (argc < 3)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    if (argc > 3) {
        rgb888 = strtol (argv[3], NULL, 16) & RGB888_MASK;
        rgb666 = RGB888_TO_RGB666 (rgb888);
        i80_set_point (pi80, x, y, rgb666);
    } else {
        rgb666 = i80_get_point (pi80, x, y);
        rgb565 = RGB666_TO_RGB565 (rgb666);
        rgb888 = RGB666_TO_RGB888 (rgb666);
        printf ("RGB565  RGB666  RGB888\n");
        printf ("  %04X   %05X  %06X\n", rgb565, rgb666, rgb888);
    }

    return 0;
EXIT:
    print_usage (sh_i80_point);
    return -1;
}

static int
sh_i80_frate (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;

    printf ("frame rate: %d\n", i80_frate (pi80));
    return 0;
}


/*****************************************************************************
 * I80 GUI
 *****************************************************************************/

command_init (sh_gui_line, "gui_line", "gui_line <x0> <y0> <x1> <y1> [<color>]");
command_init (sh_gui_circle, "gui_circle", "gui_circle <x0> <y0> <r> [<color>] [fill:0|1]");
command_init (sh_gui_rectangle, "gui_rectangle", "gui_rectangle <x> <y> <w> <h> [<color>] [fill:0|1]");
command_init (sh_gui_square, "gui_square", "gui_square <x0> <y0> <len> [<color>] [fill:0|1]");
command_init (sh_gui_text, "gui_text", "gui_text <x> <y> <text> [<fg_color>] [<bg_color>]");

command_init (sh_i80_colorbar, "i80_colorbar", "i80_colorbar [invert:0|1]");
command_init (sh_i80_pcolorbar, "i80_pcolorbar", "i80_pcolorbar [invert:0|1]");
command_init (sh_i80_w_ramp, "i80_w_ramp", "i80_w_ramp [down:0|1]");
command_init (sh_i80_r_ramp, "i80_r_ramp", "i80_r_ramp [down:0|1]");
command_init (sh_i80_g_ramp, "i80_g_ramp", "i80_g_ramp [down:0|1]");
command_init (sh_i80_b_ramp, "i80_b_ramp", "i80_b_ramp [down:0|1]");
command_init (sh_i80_chess_board, "i80_chess_board", "i80_chess_board [<size>]");

command_init (sh_gui_draw_yuv, "gui_draw_yuv", "gui_draw_yuv <x> <y> <w> <h> <addr>");
command_init (sh_gui_read_rgb565, "gui_read_rgb565", "gui_read_rgb565 <x> <y> <w> <h> <dst>");
command_init (sh_gui_write_rgb565, "gui_write_rgb565", "gui_write_rgb565 <x> <y> <w> <h> <src>");
command_init (sh_gui_read_rgb888, "gui_read_rgb888", "gui_read_rgb888 <x> <y> <w> <h> <dst>");
command_init (sh_gui_write_rgb888, "gui_write_rgb888", "gui_write_rgb888 <x> <y> <w> <h> <src>");
command_init (sh_gui_dump, "gui_dump", "gui_dump <x> <y> <w> <h>");

static int
sh_gui_line (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int x0, y0, x1, y1, rgb888, rgb666;

    if (argc < 5)
        goto EXIT;

    x0 = atoi (argv[1]);
    y0 = atoi (argv[2]);
    x1 = atoi (argv[3]);
    y1 = atoi (argv[4]);
    rgb888 = argc > 5 ? strtol (argv[5], NULL, 16) & RGB888_MASK : RGB888_FG;
    rgb666 = RGB888_TO_RGB666 (rgb888);

    gui_line (pi80, x0, y0, x1, y1, rgb666);

    return 0;
EXIT:
    print_usage (sh_gui_line);
    return -1;
}

static int
sh_gui_circle (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int x0, y0, r, rgb888, rgb666, fill;

    if (argc < 4)
        goto EXIT;

    x0 = atoi (argv[1]);
    y0 = atoi (argv[2]);
    r = atoi (argv[3]);
    rgb888 = argc > 4 ? strtol (argv[4], NULL, 16) & RGB888_MASK : RGB888_FG;
    rgb666 = RGB888_TO_RGB666 (rgb888);
    fill = argc > 5 ? atoi (argv[5]) : 0;

    gui_circle (pi80, x0, y0, r, rgb666, fill);

    return 0;
EXIT:
    print_usage (sh_gui_circle);
    return -1;
}

static int
sh_gui_rectangle (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int x, y, w, h, rgb888, rgb666, fill;

    if (argc < 5)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    rgb888 = argc > 5 ? strtol (argv[5], NULL, 16) & RGB888_MASK : RGB888_FG;
    rgb666 = RGB888_TO_RGB666 (rgb888);
    fill = argc > 6 ? atoi (argv[6]) : 0;

    gui_rectangle (pi80, x, y, w, h, rgb666, fill);

    return 0;
EXIT:
    print_usage (sh_gui_rectangle);
    return -1;
}

static int
sh_gui_square (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int x0, y0, len, rgb888, rgb666, fill;

    if (argc < 4)
        goto EXIT;

    x0 = atoi (argv[1]);
    y0 = atoi (argv[2]);
    len = atoi (argv[3]);
    rgb888 = argc > 4 ? strtol (argv[4], NULL, 16) & RGB888_MASK : RGB888_FG;
    rgb666 = RGB888_TO_RGB666 (rgb888);
    fill = argc > 5 ? atoi (argv[5]) : 0;

    gui_square (pi80, x0, y0, len, rgb666, fill);

    return 0;
EXIT:
    print_usage (sh_gui_square);
    return -1;
}

static int
sh_gui_text (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y;
    unsigned char *text;
    int rgb888, fg_color, bg_color;

    if (argc < 4)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    text = (unsigned char *) argv[3];
    if (argc > 4) {
        rgb888 = strtol (argv[4], NULL, 16) & RGB888_MASK;
        fg_color = RGB888_TO_RGB666 (rgb888);
    } else
        fg_color = RGB888_FG;
    if (argc > 5) {
        rgb888 = strtol (argv[5], NULL, 16) & RGB888_MASK;
        bg_color = RGB888_TO_RGB666 (rgb888);
    } else
        bg_color = RGB888_BG;

    gui_text (pi80, x, y, text, fg_color, bg_color);

    return 0;
EXIT:
    print_usage (sh_gui_text);
    return -1;
}

static int
sh_i80_colorbar (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int invert;

    invert = argc > 1 ? atoi (argv[1]) : 0;
    i80_colorbar (pi80, invert);

    return 0;
}

static int
sh_i80_pcolorbar (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int invert;

    invert = argc > 1 ? atoi (argv[1]) : 0;
    i80_pcolorbar (pi80, invert);

    return 0;
}

static int
sh_i80_w_ramp (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int down;

    down = argc > 1 ? atoi (argv[1]) : 0;
    i80_white_ramp (pi80, down);

    return 0;
}

static int
sh_i80_r_ramp (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int down;

    down = argc > 1 ? atoi (argv[1]) : 0;
    i80_red_ramp (pi80, down);

    return 0;
}

static int
sh_i80_g_ramp (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int down;

    down = argc > 1 ? atoi (argv[1]) : 0;
    i80_green_ramp (pi80, down);

    return 0;
}

static int
sh_i80_b_ramp (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int down;

    down = argc > 1 ? atoi (argv[1]) : 0;
    i80_blue_ramp (pi80, down);

    return 0;
}

static int
sh_i80_chess_board (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    int val;

    val = argc > 1 ? atoi (argv[1]) : 1;
    i80_chess_board (pi80, val);

    return 0;
}

static int
sh_gui_draw_yuv (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;
    unsigned int addr;

    if (argc < 6)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    addr = strtol (argv[5], NULL, 16);
    gui_draw_yuv (pi80, x, y, w, h, (void *) addr);

    return 0;
EXIT:
    print_usage (sh_gui_draw_yuv);
    return -1;
}

static int
sh_gui_read_rgb565 (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;
    unsigned int dst;

    if (argc < 6)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    dst = strtol (argv[5], NULL, 16);
    gui_read_rgb565 (pi80, x, y, w, h, (void *) dst);

    return 0;
EXIT:
    print_usage (sh_gui_read_rgb565);
    return -1;
}

static int
sh_gui_write_rgb565 (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;
    unsigned int src;

    if (argc < 6)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    src = strtol (argv[5], NULL, 16);
    gui_write_rgb565 (pi80, x, y, w, h, (void *) src);

    return 0;
EXIT:
    print_usage (sh_gui_write_rgb565);
    return -1;
}

static int
sh_gui_read_rgb888 (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;
    unsigned int dst;

    if (argc < 6)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    dst = strtol (argv[5], NULL, 16);
    gui_read_rgb888 (pi80, x, y, w, h, (void *) dst);

    return 0;
EXIT:
    print_usage (sh_gui_read_rgb888);
    return -1;
}

static int
sh_gui_write_rgb888 (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;
    unsigned int src;

    if (argc < 6)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    src = strtol (argv[5], NULL, 16);
    gui_write_rgb888 (pi80, x, y, w, h, (void *) src);

    return 0;
EXIT:
    print_usage (sh_gui_write_rgb888);
    return -1;
}

static int
sh_gui_dump (int argc, char **argv, void **result, void *sys_ctx)
{
    struct i80c *pi80 = ((struct sys_ctx_t *) sys_ctx)->pi80;
    unsigned short x, y, w, h;

    if (argc < 5)
        goto EXIT;

    x = atoi (argv[1]);
    y = atoi (argv[2]);
    w = atoi (argv[3]);
    h = atoi (argv[4]);
    gui_dump (pi80, x, y, w, h);

    return 0;
EXIT:
    print_usage (sh_gui_dump);
    return -1;
}
