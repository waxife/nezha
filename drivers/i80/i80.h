/**
 *  @file   i80.h
 *  @brief  header of i80 system interface (Via Kirin I80 master)
 *  $Id: i80.h,v 1.2 2014/04/03 10:31:15 tony Exp $
 *  $Author: tony $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2009 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/12/10  Hugo  New file.
 *
 */
#ifndef __I80_H
#define __I80_H
enum I80_PANEL_LIST{
    ILI9320 = 0,
    ILI9327 = 1,
    HX8347  = 2,
    ILI9341 =3,
    END_LIST
};
#define I80_INPUT_CPU       0
#define I80_INPUT_SCALER    1

#define I80_BASE            0xbb000000
#define I80_R00             (I80_BASE + (0x00 << 4))
#define I80_R01             (I80_BASE + (0x01 << 4))
#define I80_R02             (I80_BASE + (0x02 << 4))
#define I80_R03             (I80_BASE + (0x03 << 4))
#define I80_R04             (I80_BASE + (0x04 << 4))
#define I80_R05             (I80_BASE + (0x05 << 4))
#define I80_R06             (I80_BASE + (0x06 << 4))
#define I80_R10             (I80_BASE + (0x10 << 4))
#define I80_R11             (I80_BASE + (0x11 << 4))
#define I80_R12             (I80_BASE + (0x12 << 4))
#define I80_R13             (I80_BASE + (0x13 << 4))
#define I80_R14             (I80_BASE + (0x14 << 4))
#define I80_R15             (I80_BASE + (0x15 << 4))
#define I80_R16             (I80_BASE + (0x16 << 4))
#define I80_R17             (I80_BASE + (0x17 << 4))
#define I80_R18             (I80_BASE + (0x18 << 4))

/* I80_R00 - System Register */
#define BURST_INDEX(V)        (((V) & 0xffff) << 16)
#define REVERSE_RGB_BIT     (1 << 15)
#define COLOR_ORDER_RGB     (0 << 12)
#define COLOR_ORDER_RBG     (1 << 12)
#define COLOR_ORDER_GBR     (2 << 12)
#define COLOR_ORDER_GRB     (3 << 12)
#define COLOR_ORDER_BRG     (4 << 12)
#define COLOR_ORDER_BGR     (5 << 12)
#define DATA_BE             (0 << 11)
#define DATA_LE             (1 << 11)
#define INDEX_BE            (0 << 10)
#define INDEX_LE            (1 << 10)
#define RESERVE_OUT_8BIT    (1 << 8)
#define RESERVE_OUT_9BIT    (1 << 8)
#define RESERVE_OUT_16BIT   (2 << 8)
#define RESERVE_OUT_18BIT   (2 << 8)
#define I80_VSYNC_POL       (1 << 7)
#define ONE_PREP_DONE       (1 << 6)
#define BUS_8BIT            (0 << 4)
#define BUS_9BIT            (1 << 4)
#define BUS_16BIT           (2 << 4)
#define BUS_18BIT           (3 << 4)
#define BUS_MASK            (3 << 4)
#define SYS_RST_            (1 << 1)
#define SYS_ENABLE          (1 << 0)

#define I80_R00_MASK        (0x3 << 4)

/* I80_R01 - HGRAM Config */
#define HGRAM_AD_INDEX(V)   (((V) & 0xffff) << 16)
#define HGRAM_AD_START(V)   (((V) & 0xffff) << 0)

/* I80_R02 - VGRAM Config */
#define VGRAM_AD_INDEX(V)   (((V) & 0xffff) << 16)
#define VGRAM_AD_START(V)   (((V) & 0xffff) << 0)

/* I80_R03 - W/R Level Pulse Width */
#define WR_STROBE_WIDTH(V)  (((V) & 0xff) << 24)
#define RD_STROBE_WIDTH(V)  (((V) & 0xff) << 16)
#define WR_RECOVERY_GAP(V)  (((V) & 0xff) << 8)
#define RD_RECOVERY_GAP(V)  (((V) & 0xff) << 0)

/* I80_R04 - VSYNC & Data Retention */
#define WA_BYTE             (1 << 31)
#define WA_WORD             (0 << 31)
#define WA_MASK             (1 << 31)
#define WD_BYTE             (1 << 30)
#define WD_WORD             (0 << 30)
#define WD_MASK             (1 << 30)
#define WA_CNT(V)           (((V) & 0x3) << 28)
#define WA_CNT_MASK         (0x3 << 28)
#define WD_CNT(V)           (((V) & 0xf) << 24)
#define WD_CNT_MASK         (0xf << 24)

#define I80_R04_MASK        (0xff << 24)

/* I80_R10 - Source Select Register */
#define MODE_SHIFT          4
#define MODE_MASK           (0x3 << MODE_SHIFT)
#define MODE_SEL(V)         (((V) & 0x3) << MODE_SHIFT)
#define MODE_CMD            (0 << 4)
#define MODE_VSYNC          (1 << 4)
#define MODE_RGB            (2 << 4)
#define SRC_SHIFT           0
#define SRC_MASK            (0x1 << SRC_SHIFT)
#define SRC_SEL(V)          (((V) & 0x1) << SRC_SHIFT)
#define SRC_CPU             (0 << 0)
#define SRC_SCALER          (1 << 0)


/* I80_R11 - Command Register */
#define CMD_INDEX(R)        (((R) & 0xff) << 24)    /* command page */
#define CMD_INDEX_MASK      (0xff << 24)
#define CMD_ISSUE           (1 << 23)
#define CMD_BUSY            (1 << 23)
#define WRITE_REG           (0 << 20)   /* [22:20]=000, Write a register */
#define WRITE_GRAM          (1 << 20)   /* [22:20]=001, Write a pixel */
#define READ_REG            (2 << 20)   /* [22:20]=010, Read a register */
#define READ_GRAM           (3 << 20)   /* [22:20]=011, Read a pixel */
#define READ_STATUS         (4 << 20)   /* [22:20]=1xx, Read a status */
#define DATA_16BIT          (0 << 19)
#define DATA_18BIT          (1 << 19)
#define DUMMY_EN            (1 << 18)
#define CMD_DATA(V)         (((V) & 0x3FFFF) << 0)
#define DATA_MASK           (0x3FFFF)

/* I80_R12 - Command Register II */
#define CMD_PAGE(R)         (((R) >> 8) << 24)      /* command index */
#define CMD_PAGE_MASK       (0xff << 24)
#define READ_DONE           (1 << 16)
#define RD_BYTE             (1 << 15)
#define RD_WORD             (0 << 15)
#define RD_MASK             (1 << 15)
#define DUMMY_CNT(V)        (((V) & 0x7) << 12)
#define DUMMY_CNT_MASK      (0x7 << 12)
#define RD_CNT(V)           (((V) & 0xf) << 8)
#define RD_CNT_MASK         (0xf << 8)
#define HDE_MSK_IDX(V)      (((V) & 0x3) << 1)
#define HDE_MSK_EN          (1 << 0)

#define I80_R12_MASK        (0xff << 8)

/* I80_R13 - Burst Write */
#define BURST_ISSUE         (1 << 31)
#define FIFO_RESET          (1 << 30)
#define BURST_16BIT         (0 << 29)
#define BURST_18BIT         (1 << 29)
#define LATCH_FULL          (1 << 28)
#define FIFO_FULL           (1 << 27)
#define FIFO_EMPTY          (1 << 26)
#define PIXEL_CNT(V)        (((V) & 0xffffff) << 0)
#define PIXEL_CNT_MASK      0xffffff

/* I80_R17 - MISC */
#define END_ADDRESS_EN      (1 << 2)
#define SHIFT2LSB_8BIT      (1 << 0)
#define SHIFT2LSB_9BIT      (2 << 0)

/* I80_R18 - GRAM End Address */
#define HGRAM_AD_END(V)     (((V) & 0xffff) << 16)
#define VGRAM_AD_END(V)     (((V) & 0xffff) << 0)

/*
 * Error Code
 */

enum {
    E_BAD_BUS_WIDTH = 100,      /* invalid bus width */
    E_BAD_DATA_WIDTH = 101,     /* invalid data width */
    E_CMD_TIMEOUT = 102,        /* command issue timeout */
    E_OUT_OF_RANGE = 103,       /* location out of range */
};

/*
 * I80 Structure
 */

struct i80_lcm;

struct i80c {
    unsigned char data_width:1;
#define DATA_WIDTH_16       0
#define DATA_WIDTH_18       1

    unsigned char bus_width:2;
#define BUS_WIDTH_8         0
#define BUS_WIDTH_9         1
#define BUS_WIDTH_16        2
#define BUS_WIDTH_18        3

    unsigned char vidsrc:2;
#define I80_VIDSRC_CPU      0
#define I80_VIDSRC_CMOS     1
#define I80_VIDSRC_CVBS     2
#define I80_VIDSRC_T515     3

    unsigned char vflip:1;
#define VFLIP_OFF           0
#define VFLIP_ON            1

    unsigned char hflip:1;
#define HFLIP_OFF           0
#define HFLIP_ON            1

    unsigned short pixel;       /* buffer for 16-bit data */
    unsigned int total;         /* number of pixel to write */
    unsigned int count;         /* number of pixel has been written */

    unsigned int r04;           /* for write */
    unsigned int r12;           /* for read */

    struct i80_lcm *lcm;
};

struct i80_lcm {
    unsigned short xmax;
    unsigned short ymax;

    unsigned short wmi; /* Write Memory Index */
    unsigned short rmi; /* Read  Memory Index */

    unsigned char hac;  /* Half bus, Address Cycle count */
    unsigned char hrd;  /* Half bus, Register Dummy cycle count */
    unsigned char hmd;  /* Half bus, Memory Dummy cycle count */

    unsigned char fac;  /* Full bus, Address Cycle count */
    unsigned char frd;  /* Full bus, Register Dummy cycle count */
    unsigned char fmd;  /* Full bus, Memory Dummy cycle count */

    unsigned int r00, r01, r02, r03, r05, r17, r18; /* default value */



    int (*window) (struct i80c *pi80, int x, int y, int w, int h);
    int (*locate) (struct i80c *pi80, int x, int y);


};


/*
 * Pixel Format Conversion
 */

#define RGB666_MASK     0x3FFFF
#define RGB666_WHITE    0x3FFFF
#define RGB666_YELLOW   0x3FFC0
#define RGB666_CYAN     0x00FFF
#define RGB666_GREEN    0x00FC0
#define RGB666_MAGENTA  0x3F03F
#define RGB666_RED      0x3F000
#define RGB666_BLUE     0x0003F
#define RGB666_BLACK    0x00000
#define RGB666_FG       RGB666_WHITE
#define RGB666_BG       RGB666_BLACK

#define RGB888_MASK     0xFFFFFF
#define RGB888_WHITE    0xFFFFFF
#define RGB888_YELLOW   0xFFFF00
#define RGB888_CYAN     0x00FFFF
#define RGB888_GREEN    0x00FF00
#define RGB888_MAGENTA  0xFF00FF
#define RGB888_RED      0xFF0000
#define RGB888_BLUE     0x0000FF
#define RGB888_BLACK    0x000000
#define RGB888_FG       RGB888_WHITE
#define RGB888_BG       RGB888_BLACK

#define RGB565_TO_RGB666(C) ((((C) & (0x1f << 0)) <<  1) | (((C) & (0x3f <<  5)) << 1) | (((C) & (0x1f << 11)) <<  2))
#define RGB666_TO_RGB565(C) ((((C) & (0x1f << 1)) >>  1) | (((C) & (0x3f <<  6)) >> 1) | (((C) & (0x1f << 13)) >>  2))

#define RGB888_TO_RGB666(C) ((((C) & (0x3f << 2)) >>  2) | (((C) & (0x3f << 10)) >> 4) | (((C) & (0x3f << 18)) >>  6))
#define RGB666_TO_RGB888(C) ((((C) & (0x3f << 0)) <<  2) | (((C) & (0x3f <<  6)) << 4) | (((C) & (0x3f << 12)) <<  6))

#define BGR565_TO_RGB666(C) ((((C) & (0x1f << 0)) << 13) | (((C) & (0x3f <<  5)) << 1) | (((C) & (0x1f << 11)) >> 10))
#define BGR666_TO_RGB666(C) ((((C) & (0x3f << 0)) << 12) | (((C) & (0x3f <<  6)) << 0) | (((C) & (0x3f << 12)) >> 12))

/*
 * I80 Control API
 */
extern struct i80c *pi80;


int i80_write_reg (int idx, int val);
int i80_read_reg (int idx);

int i80_write_regv ( int idx, char *val, int cnt);
int i80_read_regv (int idx, char *val, int cnt);

int i80_write_data (struct i80c *pi80, int val);
int i80_read_data (struct i80c *pi80);

int i80_burst_setup (struct i80c *pi80, int cnt);
int i80_burst_write (struct i80c *pi80, int val);
int i80_burst_write_is_done (struct i80c *pi80);

int i80_set_bus_width (int width);
int i80_get_bus_width (void);

int i80_set_data_width (int bus_width);
int i80_get_data_width (void);

int i80_set_input (int sel);
int i80_get_input (void);


#define I80_OUTPUT_CMD      0
#define I80_OUTPUT_VSYNC    1
#define I80_OUTPUT_RGB      2
int i80_set_output (int sel);
int i80_get_output (void);


void i80_reset ();
int i80_init (enum I80_PANEL_LIST i80_panel, int bus_width, int width, int height);

int i80_set_point (int x, int y, int color);
int i80_get_point (int x, int y);
int i80_clear (int color);




/*
 * I80 Panel API
 */



extern int (*i80_window) (struct i80c *pi80, int x, int y, int w, int h);
extern int (*i80_locate) (struct i80c *pi80, int x, int y);

/*
 * I80 GUI
 */

void gui_line (struct i80c *pi80, int x0, int y0, int x1, int y1, int color);
void gui_circle (struct i80c *pi80, int x, int y, int r, int color, int fill);
void gui_rectangle (struct i80c *pi80, int x, int y, int w, int h, int color, int fill);
void gui_square (struct i80c *pi80, int x, int y, int len, int color, int fill);
void gui_text (struct i80c *pi80, int x, int y, char *str, int fg_color, int bg_color);

void gui_draw_yuv (struct i80c *pi80, int x, int y, int w, int h, void *src);
void gui_dump (struct i80c *pi80, int x, int y, int w, int h);

void gui_read_rgb565 (struct i80c *pi80, int x, int y, int w, int h, void *dst);
void gui_write_rgb565 (struct i80c *pi80, int x, int y, int w, int h, void *src);

void gui_read_rgb888 (struct i80c *pi80, int x, int y, int w, int h, void *dst);
void gui_write_rgb888 (struct i80c *pi80, int x, int y, int w, int h, void *src);

/*
 * I80 Test Pattern
 */

void i80_colorbar (struct i80c *pi80, int invert);
void i80_pcolorbar (struct i80c *pi80, int invert);
void i80_white_ramp (struct i80c *pi80, int down);
void i80_red_ramp (struct i80c *pi80, int down);
void i80_green_ramp (struct i80c *pi80, int down);
void i80_blue_ramp (struct i80c *pi80, int down);
void i80_chess_board (struct i80c *pi80, int invert);

/*
 * for debug only
 */


#ifdef I80_DEBUG
#define READL(addr)     ({ unsigned int   v = readl (addr); printf ("%s:%-3d read4  0x%08x 0x%08x\n", __FILE__, __LINE__, addr, v); v; })
#define WRITEL(v, addr) do { printf ("%s:%-3d write4 0x%08x 0x%08x\n", __FILE__, __LINE__, addr, v); writel (v, addr); } while (0)
#else
#define READL(addr)     readl(addr)
#define WRITEL(v, addr) writel(v, addr)
#endif

#define latch_delay(loop) do { volatile int i; for (i = 0; i < loop; i++); } while (0)

#endif /* __I80_H */
