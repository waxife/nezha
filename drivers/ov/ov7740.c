/**
 *  @file   ov7740.c
 *  @brief  omniVision camera chip driver
 *  $Id: ov7740.c,v 1.1.1.1 2013/12/18 03:43:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/09/24  hugo  New file. (idea from ov7740.c in linux-2.6.36)
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <debug.h>

#include "sccb.h"
#include "ov.h"

/*
 * Basic window sizes.  These probably belong somewhere more globally
 * useful.
 */
#define VGA_WIDTH   640
#define VGA_HEIGHT  480
#define QVGA_WIDTH  320
#define QVGA_HEIGHT 240
#define CIF_WIDTH   352
#define CIF_HEIGHT  288
#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

/*
 * Our nominal (default) frame rate.
 */
#define OV7740_FRAME_RATE 30

/*
 * The 7740 sits on i2c with ID 0x42
 */
#define OV7740_I2C_ADDR 0x42

/*
 * OV7740 Device Control Register List
 */
#define REG_GAIN        0x00    /* Gain lower 8 bits (rest in vref) */
#define REG_BLUE        0x01    /* blue gain */
#define REG_RED         0x02    /* red gain */
#define REG_VREF        0x03    /* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1        0x04    /* Control 1 */
#define   COM1_CCIR656  0x40    /* CCIR656 enable */
#define REG_BAVE        0x05    /* U/B Average level */
#define REG_GbAVE       0x06    /* Y/Gb Average level */
#define REG_AECHH       0x07    /* AEC MS 5 bits */
#define REG_RAVE        0x08    /* V/R Average level */
#define REG_COM2        0x09    /* Control 2 */
#define   COM2_SSLEEP   0x10    /* Soft sleep mode */
#define REG_PID         0x0a    /* Product ID MSB */
#define REG_VER         0x0b    /* Product ID LSB */
#define REG_COM3        0x0c    /* Control 3 */
#define   COM3_SWAP     0x40    /* Byte swap */
#define   COM3_SCALEEN  0x08    /* Enable scaling */
#define   COM3_DCWEN    0x04    /* Enable downsamp/crop/window */
#define REG_COM4        0x0d    /* Control 4 */
#define REG_COM5        0x0e    /* All "reserved" */
#define REG_COM6        0x0f    /* Control 6 */
#define REG_AECH        0x10    /* More bits of AEC value */
#define REG_CLKRC       0x11    /* Clocl control */
#define   CLK_EXT       0x40    /* Use external clock directly */
#define   CLK_SCALE     0x3f    /* Mask for internal clock scale */
#define REG_COM7        0x12    /* Control 7 */
#define   COM7_RESET    0x80    /* Register reset */
#define   COM7_FMT_MASK 0x38
#define   COM7_FMT_VGA  0x00
#define   COM7_FMT_CIF  0x20    /* CIF format */
#define   COM7_FMT_QVGA 0x10    /* QVGA format */
#define   COM7_FMT_QCIF 0x08    /* QCIF format */
#define   COM7_RGB      0x04    /* bits 0 and 2 - RGB format */
#define   COM7_YU       0x00    /* YUV */
#define   COM7_BAYER    0x01    /* Bayer format */
#define   COM7_PBAYER   0x05    /* "Processed bayer" */
#define REG_COM8        0x13    /* Control 8 */
#define   COM8_FASTAEC  0x80    /* Enable fast AGC/AEC */
#define   COM8_AECSTEP  0x40    /* Unlimited AEC step size */
#define   COM8_BFILT    0x20    /* Band filter enable */
#define   COM8_AGC      0x04    /* Auto gain enable */
#define   COM8_AWB      0x02    /* White balance enable */
#define   COM8_AEC      0x01    /* Auto exposure enable */
#define REG_COM9        0x14    /* Control 9  - gain ceiling */
#define REG_COM10       0x15    /* Control 10 */
#define   COM10_HSYNC   0x40    /* HSYNC instead of HREF */
#define   COM10_PCLK_HB 0x20    /* Suppress PCLK on horiz blank */
#define   COM10_HREF_REV 0x08   /* Reverse HREF */
#define   COM10_VS_LEAD 0x04    /* VSYNC on clock leading edge */
#define   COM10_VS_NEG  0x02    /* VSYNC negative */
#define   COM10_HS_NEG  0x01    /* HSYNC negative */
#define REG_HSTART      0x17    /* Horiz start high bits */
#define REG_HSTOP       0x18    /* Horiz stop high bits */
#define REG_VSTART      0x19    /* Vert start high bits */
#define REG_VSTOP       0x1a    /* Vert stop high bits */
#define REG_PSHFT       0x1b    /* Pixel delay after HREF */
#define REG_MIDH        0x1c    /* Manuf. ID high */
#define REG_MIDL        0x1d    /* Manuf. ID low */
#define REG_MVFP        0x1e    /* Mirror / vflip */
#define   MVFP_MIRROR   0x20    /* Mirror image */
#define   MVFP_FLIP     0x10    /* Vertical flip */

#define REG_AEW         0x24    /* AGC upper limit */
#define REG_AEB         0x25    /* AGC lower limit */
#define REG_VPT         0x26    /* AGC/AEC fast mode op region */
#define REG_HSYST       0x30    /* HSYNC rising edge delay */
#define REG_HSYEN       0x31    /* HSYNC falling edge delay */
#define REG_HREF        0x32    /* HREF pieces */
#define REG_TSLB        0x3a    /* lots of stuff */
#define   TSLB_YLAST    0x04    /* UYVY or VYUY - see com13 */
#define REG_COM11       0x3b    /* Control 11 */
#define   COM11_NIGHT   0x80    /* NIght mode enable */
#define   COM11_NMFR    0x60    /* Two bit NM frame rate */
#define   COM11_HZAUTO  0x10    /* Auto detect 50/60 Hz */
#define   COM11_50HZ    0x08    /* Manual 50Hz select */
#define   COM11_EXP     0x02
#define REG_COM12       0x3c    /* Control 12 */
#define   COM12_HREF    0x80    /* HREF always */
#define REG_COM13       0x3d    /* Control 13 */
#define   COM13_GAMMA   0x80    /* Gamma enable */
#define   COM13_UVSAT   0x40    /* UV saturation auto adjustment */
#define   COM13_UVSWAP  0x01    /* V before U - w/TSLB */
#define REG_COM14       0x3e    /* Control 14 */
#define   COM14_DCWEN   0x10    /* DCW/PCLK-scale enable */
#define REG_EDGE        0x3f    /* Edge enhancement factor */
#define REG_COM15       0x40    /* Control 15 */
#define   COM15_R10F0   0x00    /* Data range 10 to F0 */
#define   COM15_R01FE   0x80    /*            01 to FE */
#define   COM15_R00FF   0xc0    /*            00 to FF */
#define   COM15_RGB565  0x10    /* RGB565 output */
#define   COM15_RGB555  0x30    /* RGB555 output */
#define REG_COM16       0x41    /* Control 16 */
#define   COM16_AWBGAIN 0x08    /* AWB gain enable */
#define REG_COM17       0x42    /* Control 17 */
#define   COM17_AECWIN  0xc0    /* AEC window - must match COM4 */
#define   COM17_CBAR    0x08    /* DSP Color bar */

/*
 * This matrix defines how the colors are generated, must be
 * tweaked to adjust hue and saturation.
 *
 * Order: v-red, v-green, v-blue, u-red, u-green, u-blue
 *
 * They are nine-bit signed quantities, with the sign bit
 * stored in 0x58.  Sign for v-red is bit 0, and up from there.
 */
#define REG_CMATRIX_BASE 0x4f
#define CMATRIX_LEN 6
#define REG_CMATRIX_SIGN 0x58


#define REG_BRIGHT      0x55    /* Brightness */
#define REG_CONTRAS     0x56    /* Contrast control */

#define REG_GFIX        0x69    /* Fix gain control */

#define REG_REG76       0x76    /* OV's name */
#define   R76_BLKPCOR   0x80    /* Black pixel correction enable */
#define   R76_WHTPCOR   0x40    /* White pixel correction enable */

#define REG_RGB444      0x8c    /* RGB 444 control */
#define   R444_ENABLE   0x02    /* Turn on RGB444, overrides 5x5 */
#define   R444_RGBX     0x01    /* Empty nibble at end */

#define REG_HAECC1      0x9f    /* Hist AEC/AGC control 1 */
#define REG_HAECC2      0xa0    /* Hist AEC/AGC control 2 */

#define REG_BD50MAX     0xa5    /* 50hz banding step limit */
#define REG_HAECC3      0xa6    /* Hist AEC/AGC control 3 */
#define REG_HAECC4      0xa7    /* Hist AEC/AGC control 4 */
#define REG_HAECC5      0xa8    /* Hist AEC/AGC control 5 */
#define REG_HAECC6      0xa9    /* Hist AEC/AGC control 6 */
#define REG_HAECC7      0xaa    /* Hist AEC/AGC control 7 */
#define REG_BD60MAX     0xab    /* 60hz banding step limit */

/*
 * The default register settings, as obtained from OmniVision.  There
 * is really no making sense of most of these - lots of "reserved" values
 * and such.
 *
 * These settings give VGA YUYV.
 */

struct regval_list {
    unsigned char reg;
    unsigned char val;
};

static const struct regval_list ov7740_cfg[] = {

#if 0	
    {REG_COM7, COM7_RESET},
    {REG_CLKRC, 0x1},           /* clock scale: 1=30fps, 2=20fps, 3=15fps */
    {0x92, 0x00},               /* Dummy Line LSB */
    {0x93, 0x00},               /* Dummy Line MSB */
    {0x2a, 0x00},               /* Dummy Pixel Insert MSB */
    {0x2b, 0x00},               /* Dummy Pixel Insert LSB */
    {0x2d, 0x43},               /* Dummy Line Insert LSB in Vertical Direction */
    {0x2e, 0x00},               /* Dummy Line Insert MSB in Vertical Direction */
    {0x3b, 0x0a},
    {0x9d, 0x66},               /* 50 Hz Banding Filter Value */
    {0x9e, 0x66},               /* 60 Hz Banding Filter Value */
    {REG_TSLB, 0x04},           // UV_Swap[1]=0
    {REG_COM7, 0x00},           /* VGA YUV */
    /*
     * Set the hardware window.  These values from OV don't entirely
     * make sense - hstop is less than hstart.  But they work...
     */
    {REG_HSTART, 0x13},
    {REG_HSTOP, 0x01},
    {REG_HREF, 0xb6},
    {REG_VSTART, 0x02},
    {REG_VSTOP, 0x7a},
    {REG_VREF, 0x0a},

    {REG_COM3, 0x00},
    {REG_COM14, 0x00},

    /* Mystery scaling numbers */
    {0x70, 0x3a},
    {0x71, 0x35},
    {0x72, 0x11},
    {0x73, 0xf0},
    {0xa2, 0x02},
    {REG_COM10, 0x10},          /* bit[4]=1, PCLK reverse */

    /* Gamma curve values */
    {0x7a, 0x20},
    {0x7b, 0x10},
    {0x7c, 0x1e},
    {0x7d, 0x35},
    {0x7e, 0x5a},
    {0x7f, 0x69},
    {0x80, 0x76},
    {0x81, 0x80},
    {0x82, 0x88},
    {0x83, 0x8f},
    {0x84, 0x96},
    {0x85, 0xa3},
    {0x86, 0xaf},
    {0x87, 0xc4},
    {0x88, 0xd7},
    {0x89, 0xe8},

    /* AGC and AEC parameters.  Note we start by disabling those features,
       then turn them only after tweaking the values. */
    {REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT},
    {REG_GAIN, 0x00},
    {REG_AECH, 0x00},
    {REG_COM4, 0x40},           /* magic reserved bit */
    {REG_COM9, 0x18},           /* 4x gain + magic rsvd bit */
    {REG_BD50MAX, 0x05},
    {REG_BD60MAX, 0x07},
    {REG_AEW, 0x95},
    {REG_AEB, 0x33},
    {REG_VPT, 0xe3},
    {REG_HAECC1, 0x78},
    {REG_HAECC2, 0x68},
    {0xa1, 0x03},               /* magic */
    {REG_HAECC3, 0xd8},
    {REG_HAECC4, 0xd8},
    {REG_HAECC5, 0xf0},
    {REG_HAECC6, 0x90},
    {REG_HAECC7, 0x94},
    {REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC},

    /* Almost all of these are magic "reserved" values.  */
    {REG_COM5, 0x61},
    {REG_COM6, 0x4b},
    {0x16, 0x02},
    {REG_MVFP, 0x07},
    {0x21, 0x02},
    {0x22, 0x91},
    {0x29, 0x07},
    {0x33, 0x0b},
    {0x35, 0x0b},
    {0x37, 0x1d},
    {0x38, 0x71},
    {0x39, 0x2a},
    {REG_COM12, 0x78},
    {0x4d, 0x40},
    {0x4e, 0x20},
    {REG_GFIX, 0x00},
    {0x6b, 0x4a},
    {0x74, 0x10},
    {0x8d, 0x4f},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},
    {0x96, 0x00},
    {0x9a, 0x00},
    {0xb0, 0x84},
    {0xb1, 0x0c},
    {0xb2, 0x0e},
    {0xb3, 0x82},
    {0xb8, 0x0a},

    /* More reserved magic, some of which tweaks white balance */
    {0x43, 0x0a},
    {0x44, 0xf0},
    {0x45, 0x34},
    {0x46, 0x58},
    {0x47, 0x28},
    {0x48, 0x3a},
    {0x59, 0x88},
    {0x5a, 0x88},
    {0x5b, 0x44},
    {0x5c, 0x67},
    {0x5d, 0x49},
    {0x5e, 0x0e},
    {0x6c, 0x0a},
    {0x6d, 0x55},
    {0x6e, 0x11},
    {0x6f, 0x9f},               /* "9e for advance AWB" */
    {0x6a, 0x40},
    {REG_BLUE, 0x40},
    {REG_RED, 0x60},
    {REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC | COM8_AWB},

    /* Matrix coefficients for sat=128, hue=0 */
    {0x4f, 0x80},
    {0x50, 0x80},
    {0x51, 0x00},
    {0x52, 0x22},
    {0x53, 0x5e},
    {0x54, 0x80},
    {0x58, 0x9e},

    {REG_COM16, COM16_AWBGAIN},
    {REG_EDGE, 0x00},
    {0x75, 0x05},
    {0x76, 0xe1},
    {0x4c, 0x00},
    {0x77, 0x01},
    {REG_COM13, 0xc2},          // UV_Swap[0]=0
    {0x4b, 0x09},
    {0xc9, 0x60},
    {REG_COM16, 0x38},
    {0x56, 0x40},

    {0x34, 0x11},
    {REG_COM11, COM11_EXP | COM11_HZAUTO},
    {0xa4, 0x88},
    {0x96, 0x00},
    {0x97, 0x30},
    {0x98, 0x20},
    {0x99, 0x30},
    {0x9a, 0x84},
    {0x9b, 0x29},
    {0x9c, 0x03},
    {0x9d, 0x4c},
    {0x9e, 0x3f},
    {0x78, 0x04},

    /* Extra-weird stuff.  Some sort of multiplexor register */
    {0x79, 0x01},
    {0xc8, 0xf0},
    {0x79, 0x0f},
    {0xc8, 0x00},
    {0x79, 0x10},
    {0xc8, 0x7e},
    {0x79, 0x0a},
    {0xc8, 0x80},
    {0x79, 0x0b},
    {0xc8, 0x01},
    {0x79, 0x0c},
    {0xc8, 0x0f},
    {0x79, 0x0d},
    {0xc8, 0x20},
    {0x79, 0x09},
    {0xc8, 0x80},
    {0x79, 0x02},
    {0xc8, 0xc0},
    {0x79, 0x03},
    {0xc8, 0x40},
    {0x79, 0x05},
    {0xc8, 0x30},
    {0x79, 0x26},
#else
	
{0x12, 0x80},
{0x13, 0x00},

{0x11, 0x00},
{0x12, 0x00},
{0xd5, 0x10},
//{0x0c, 0x12},
{0x0d, 0x34},
{0x17, 0x25},
{0x18, 0xa0},
{0x19, 0x03},
{0x1a, 0xf0},
{0x1b, 0x89},
{0x22, 0x03},
//{0x28, 0x7f},
{0x29, 0x17},
{0x2b, 0xf8},
{0x2c, 0x01},
{0x31, 0xa0},
{0x32, 0xf0},
{0x33, 0xc4},
{0x35, 0x05},
{0x36, 0x3f},

{0x04, 0x60},
{0x27, 0x80},
{0x3d, 0x0f},
{0x3e, 0x82},
{0x3f, 0x40},
{0x40, 0x7f},
{0x41, 0x6a},
{0x42, 0x29},
{0x44, 0xe5},
{0x45, 0x41},
{0x47, 0x42},
{0x48, 0x00},
{0x49, 0x61},
{0x4a, 0xa1},
{0x4b, 0x46},
{0x4c, 0x18},
{0x4d, 0x50},
{0x4e, 0x13},
{0x64, 0x00},
{0x67, 0x88},
{0x68, 0x1a},

{0x14, 0x38},
{0x24, 0x3c},
{0x25, 0x30},
{0x26, 0x72},
{0x50, 0x97},
{0x51, 0x7e},
{0x52, 0x00},
{0x53, 0x00},
{0x20, 0x00},
{0x21, 0x23},
{0x38, 0x14},
{0xe9, 0x00},
{0x56, 0x55},
{0x57, 0xff},
{0x58, 0xff},
{0x59, 0xff},
{0x5f, 0x04},
{0xec, 0x00},
{0x13, 0xff},

{0x80, 0x7d},
{0x81, 0x3f},
{0x82, 0x32},
{0x83, 0x01},
{0x38, 0x11},
{0x84, 0x70},
{0x85, 0x00},
{0x86, 0x03},
{0x87, 0x01},
{0x88, 0x05},
{0x89, 0x30},
{0x8d, 0x30},
{0x8f, 0x85},
{0x93, 0x30},
{0x95, 0x85},
{0x99, 0x30},
{0x9b, 0x85},

{0x9c, 0x08},
{0x9d, 0x12},
{0x9e, 0x23},
{0x9f, 0x45},
{0xa0, 0x55},
{0xa1, 0x64},
{0xa2, 0x72},
{0xa3, 0x7f},
{0xa4, 0x8b},
{0xa5, 0x95},
{0xa6, 0xa7},
{0xa7, 0xb5},
{0xa8, 0xcb},
{0xa9, 0xdd},
{0xaa, 0xec},
{0xab, 0x1a},

{0xce, 0x78},
{0xcf, 0x6e},
{0xd0, 0x0a},
{0xd1, 0x0c},
{0xd2, 0x84},
{0xd3, 0x90},
{0xd4, 0x1e},

{0x5a, 0x24},
{0x5b, 0x1f},
{0x5c, 0x88},
{0x5d, 0x60},

{0xac, 0x6e},
{0xbe, 0xff},
{0xbf, 0x00},

/*50/60Hz auto detection is XCLK dependent
the following is based on XCLK = 24MHz */
{0x70, 0x00},
{0x71, 0x34},
{0x74, 0x28},
{0x75, 0x98},
{0x76, 0x00},
{0x77, 0x08},
{0x78, 0x01},
{0x79, 0xc2},
{0x7d, 0x02},
{0x7a, 0x4e},
{0x7b, 0x1f},
{0xEC, 0x00}, //00/80 for manual/auto
{0x7c, 0x0c},
	
#endif
};

#define OV7740_CFG_NUM  (sizeof(ov7740_cfg) / sizeof(ov7740_cfg[0]))

/*
 * Low-level register I/O
 */

int
ov7740_write (unsigned char reg, unsigned char value)
{
    int rc;
	//printf("sccb_write(%02x,%02x)\n", reg, value);
    rc = sccb_write (reg, value);
    if (rc < 0)
        ERROR ("sccb_write(%02x,%02x)\n", reg, value);
    if (reg == REG_COM7 && (value & COM7_RESET))
        usleep (1000);          /* Wait 1ms for reset to run */

    return rc;
}

int
ov7740_read (unsigned char reg, unsigned char *value)
{
    int rc;

    rc = sccb_read (reg, value);
    if (rc < 0)
        ERROR ("sccb_read(%02x)\n", reg);

    return rc;
}

/*
 * set/get registers
 */

/*
 * Synopsis     int ov7740_set_reg (struct ovc *pov, unsigned char reg, unsigned char val);
 * Description  write value to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_reg (struct ovc *pov, unsigned char reg, unsigned char val)
{
    return ov7740_write (reg, val);
}

/*
 * Synopsis     int ov7740_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk);
 * Description  write bits at positions specified by mask to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 *              msk - enable bit mask
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk)
{
    unsigned char old, new;
    int rc;

    rc = ov7740_read (reg, &old);
    if (rc < 0) {
        ERROR ("ov7740_read(%02X)\n", reg);
        return rc;
    }

    new = (old & ~msk) | (val & msk);
    rc = ov7740_write (reg, new);
    if (rc < 0) {
        ERROR ("ov7740_write(%02X, %02X)\n", reg, new);
        return rc;
    }

    return 0;
}

/*
 * Synopsis     int ov7740_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val);
 * Description  read value from a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val)
{
    return ov7740_read (reg, val);
}

/*
 * Synopsis     void ov7740_dump (struct ovc *pov);
 * Description  dump all registers
 * Parameters   pov - point to ov private data
 * Return       none
 */
void
ov7740_dump (struct ovc *pov)
{
    int reg;
    unsigned char val;
    int rc;

    for (reg = 0; reg < 256; reg++) {
        if (reg % 16 == 0)
            DBG_PRINT ("%02X    ", reg);

        rc = ov7740_read (reg, &val);
        if (rc < 0) {
            ERROR ("ov7740_read (%02X)\n", reg);
            return;
        }
        DBG_PRINT ("%02X ", val);

        if ((reg + 1) % 8 == 0)
            DBG_PRINT (" ");
        if ((reg + 1) % 16 == 0)
            DBG_PRINT ("\n");
    }
}

/*
 * code for setting saturation and hue
 */

/* Color Matrix of YUYV4:2:2 - the magic nubmers come from OmniVision */
static const int ov7740_cmatrix[CMATRIX_LEN] = { 128, -128, 0, -34, -94, 128 };

/*
 * Hue also requires messing with the color matrix.  It also requires
 * trig functions, which tend not to be well supported in the kernel.
 * So here is a simple table of sine values, 0-90 degrees, in steps
 * of five degrees.  Values are multiplied by 1000.
 *
 * The following naive approximate trig functions require an argument
 * carefully limited to -180 <= theta <= 180.
 */

#define SIN_STEP 5
static const int ov7740_sin_table[] = {
    0, 87, 173, 258, 342, 422,
    499, 573, 642, 707, 766, 819,
    866, 906, 939, 965, 984, 996,
    1000
};

static int
ov7740_sine (int theta)
{
    int chs = 1;
    int sine;

    if (theta < 0) {
        theta = -theta;
        chs = -1;
    }
    if (theta <= 90)
        sine = ov7740_sin_table[theta / SIN_STEP];
    else {
        theta -= 90;
        sine = 1000 - ov7740_sin_table[theta / SIN_STEP];
    }
    return sine * chs;
}

static int
ov7740_cosine (int theta)
{
    theta = 90 - theta;
    if (theta > 180)
        theta -= 360;
    else if (theta < -180)
        theta += 360;
    return ov7740_sine (theta);
}

static void
ov7740_calc_cmatrix (struct ovc *pov, int matrix[CMATRIX_LEN])
{
    int i;
    /*
     * Apply the current saturation setting first.
     */
    for (i = 0; i < CMATRIX_LEN; i++)
        matrix[i] = (ov7740_cmatrix[i] * pov->sat) >> 7;
    /*
     * Then, if need be, rotate the hue value.
     */
    if (pov->hue != 0) {
        int sinth, costh, tmpmatrix[CMATRIX_LEN];

        memcpy (tmpmatrix, matrix, CMATRIX_LEN * sizeof (int));
        sinth = ov7740_sine (pov->hue);
        costh = ov7740_cosine (pov->hue);

        matrix[0] = (tmpmatrix[3] * sinth + tmpmatrix[0] * costh) / 1000;
        matrix[1] = (tmpmatrix[4] * sinth + tmpmatrix[1] * costh) / 1000;
        matrix[2] = (tmpmatrix[5] * sinth + tmpmatrix[2] * costh) / 1000;
        matrix[3] = (tmpmatrix[3] * costh - tmpmatrix[0] * sinth) / 1000;
        matrix[4] = (tmpmatrix[4] * costh - tmpmatrix[1] * sinth) / 1000;
        matrix[5] = (tmpmatrix[5] * costh - tmpmatrix[2] * sinth) / 1000;
    }
}

static int
ov7740_store_cmatrix (int matrix[CMATRIX_LEN])
{
    int i, ret;
    unsigned char signbits = 0;

    /*
     * Weird crap seems to exist in the upper part of
     * the sign bits register, so let's preserve it.
     */
    ret = ov7740_read (REG_CMATRIX_SIGN, &signbits);
    signbits &= 0xc0;

    for (i = 0; i < CMATRIX_LEN; i++) {
        unsigned char raw;

        if (matrix[i] < 0) {
            signbits |= (1 << i);
            if (matrix[i] < -255)
                raw = 0xff;
            else
                raw = (-1 * matrix[i]) & 0xff;
        } else {
            if (matrix[i] > 255)
                raw = 0xff;
            else
                raw = matrix[i] & 0xff;
        }
        ret += ov7740_write (REG_CMATRIX_BASE + i, raw);
    }
    ret += ov7740_write (REG_CMATRIX_SIGN, signbits);
    return ret;
}

/*
 * Synopsis     int ov7740_set_sat (struct ovc *pov, unsigned char val);
 * Description  set saturation
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_sat (struct ovc *pov, unsigned char val)
{
    int matrix[CMATRIX_LEN];
    int rc;

    pov->sat = val;
    ov7740_calc_cmatrix (pov, matrix);
    rc = ov7740_store_cmatrix (matrix);
    return rc;
}

/*
 * Synopsis     int ov7740_get_sat (struct ovc *pov, unsigned char *val);
 * Description  get saturation
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_sat (struct ovc *pov, unsigned char *val)
{
    *val = pov->sat;
    return 0;
}

/*
 * Synopsis     int ov7740_set_hue (struct ovc *pov, int val);
 * Description  set hue
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_hue (struct ovc *pov, int val)
{
    int matrix[CMATRIX_LEN];
    int rc;

    if (val < -180 || val > 180)
        return -1;

    pov->hue = val;
    ov7740_calc_cmatrix (pov, matrix);
    rc = ov7740_store_cmatrix (matrix);
    return rc;
}

/*
 * Synopsis     int ov7740_get_hue (struct ovc *pov, int *val);
 * Description  get hue
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_hue (struct ovc *pov, int *val)
{
    *val = pov->hue;
    return 0;
}

/*
 * Some weird registers seem to store values in a sign/magnitude format!
 */
#define SM2ABS(V)   ((V & 0x80) == 0 ? (V + 128) : (128 - (V & 0x7f)))
#define ABS2SM(V)   (V > 127 ? (V & 0x7f) : ((128 - V) | 0x80))

/*
 * Synopsis     int ov7740_set_bright (struct ovc *pov, unsigned char val);
 * Description  set brightness
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_bright (struct ovc *pov, unsigned char val)
{
    unsigned char com8 = 0, v;
    int rc;

    /* disable AEC */
    ov7740_read (REG_COM8, &com8);
    com8 &= ~COM8_AEC;
    ov7740_write (REG_COM8, com8);

    /* set gain value */
    v = ABS2SM (val);
    rc = ov7740_write (REG_BRIGHT, v);

    return rc;
}

/*
 * Synopsis     int ov7740_get_bright (struct ovc *pov, unsigned char *val);
 * Description  get brightness
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_bright (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0;
    int rc = ov7740_read (REG_BRIGHT, &v);

    *val = SM2ABS (v);
    return rc;
}

/*
 * Synopsis     int ov7740_set_contrast (struct ovc *pov, unsigned char val);
 * Description  set contrast
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_contrast (struct ovc *pov, unsigned char val)
{
    return ov7740_write (REG_CONTRAS, val);
}

/*
 * Synopsis     int ov7740_get_contrast (struct ovc *pov, unsigned char *val);
 * Description  get contrast
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_contrast (struct ovc *pov, unsigned char *val)
{
    return ov7740_read (REG_CONTRAS, val);
}

/*
 * Synopsis     int ov7740_set_vflip (struct ovc *pov, unsigned char val);
 * Description  set virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_vflip (struct ovc *pov, unsigned char val)
{
    unsigned char v = 0;

    ov7740_read (REG_MVFP, &v);
    if (val)
        v |= MVFP_FLIP;
    else
        v &= ~MVFP_FLIP;

    return ov7740_write (REG_MVFP, v);
}

/*
 * Synopsis     int ov7740_get_vflip (struct ovc *pov, unsigned char *val);
 * Description  get virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_vflip (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0;
    int rc;

    rc = ov7740_read (REG_MVFP, &v);
    *val = (v & MVFP_FLIP) == MVFP_FLIP;

    return rc;
}

/*
 * Synopsis     int ov7740_set_hflip (struct ovc *pov, unsigned char val);
 * Description  set horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_hflip (struct ovc *pov, unsigned char val)
{
    unsigned char v = 0;

    ov7740_read (REG_MVFP, &v);
    if (val)
        v |= MVFP_MIRROR;
    else
        v &= ~MVFP_MIRROR;

    return ov7740_write (REG_MVFP, v);
}

/*
 * Synopsis     int ov7740_get_hflip (struct ovc *pov, unsigned char *val);
 * Description  get horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_hflip (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0;
    int rc;

    rc = ov7740_read (REG_MVFP, &v);
    *val = (v & MVFP_MIRROR) == MVFP_MIRROR;

    return rc;
}

/*
 * Synopsis     int ov7740_set_gain (struct ovc *pov, unsigned char val);
 * Description  set gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_gain (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7740_write (REG_GAIN, val);
    if (rc < 0)
        return rc;

    /* disable AGC */
    rc = ov7740_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    rc = ov7740_write (REG_COM8, com8 & ~COM8_AGC);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7740_get_gain (struct ovc *pov, unsigned char *val);
 * Description  get gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_gain (struct ovc *pov, unsigned char *val)
{
    return ov7740_read (REG_GAIN, val);
}

/*
 * Synopsis     int ov7740_set_autogain (struct ovc *pov, unsigned char val);
 * Description  set auto gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_autogain (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7740_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    if (val)
        com8 |= COM8_AGC;
    else
        com8 &= ~COM8_AGC;
    rc = ov7740_write (REG_COM8, com8);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7740_get_autogain (struct ovc *pov, unsigned char *val);
 * Description  get auto gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_autogain (struct ovc *pov, unsigned char *val)
{
    unsigned char com8 = 0;
    int rc;

    rc = ov7740_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    *val = (com8 & COM8_AGC) != 0;
    return 0;
}

/*
 * Synopsis     int ov7740_set_exp (struct ovc *pov, unsigned short val);
 * Description  set exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_exp (struct ovc *pov, unsigned short val)
{
    unsigned char com1, com8, aech, aechh;
    int rc;

    rc = ov7740_read (REG_COM1, &com1) + ov7740_read (REG_COM8, &com8) + ov7740_read (REG_AECHH, &aechh);
    if (rc < 0)
        return rc;

    com1 = (com1 & 0xfc) | (val & 0x03);
    aech = (val >> 2) & 0xff;
    aechh = (aechh & 0xc0) | ((val >> 10) & 0x3f);

    rc = ov7740_write (REG_COM1, com1) + ov7740_write (REG_AECH, aech) + ov7740_write (REG_AECHH, aechh);
    if (rc < 0)
        return rc;

    /* disable AEC */
    rc = ov7740_write (REG_COM8, com8 & ~COM8_AEC);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7740_get_exp (struct ovc *pov, unsigned char *val);
 * Description  get exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_exp (struct ovc *pov, unsigned short *val)
{
    unsigned char com1, aech, aechh;
    int rc;

    rc = ov7740_read (REG_COM1, &com1) + ov7740_read (REG_AECH, &aech) + ov7740_read (REG_AECHH, &aechh);
    if (rc < 0)
        return rc;

    *val = ((aechh & 0x3f) << 10) | (aech << 2) | (com1 & 0x03);
    return 0;
}

/*
 * Synopsis     int ov7740_set_autoexp (struct ovc *pov, unsigned char val);
 * Description  set auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7740_set_autoexp (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7740_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    if (val)
        com8 |= COM8_AEC;
    else
        com8 &= ~COM8_AEC;
    rc = ov7740_write (REG_COM8, com8);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7740_get_autoexp (struct ovc *pov, unsigned char *val);
 * Description  get auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7740_get_autoexp (struct ovc *pov, unsigned char *val)
{
    unsigned char com8;
    int rc;

    rc = ov7740_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    *val = (com8 & COM8_AEC) != 0;
    return 0;

}

/*
 * Synopsis     int ov7740_init (struct ovc *pov);
 * Description  init ov camera chip
 *                - reset chip
 *                - check manufacturer ID, product ID, Version
 *                - setup hardware window (HREF, VREF)
 *                - setup registers (scaleing, gamma, AGC, AEC, color matrix)
 * Parameters   pov - point to ov private data
 * Return       0 for success, or -1 for any error
 */
int
ov7740_init (struct ovc *pov)
{
	
    unsigned char midh, midl, pid, ver;
    int i, rc;
	
    printf("7740 init\n");
    
    /* init sccb bus */
    sccb_init ();

#if 1

    /* reset chip */
    ov7740_write (0x12, 0x80);

    /* check OV manufacturer id - R1C=0x7f, R1D=0xa2 */
    rc = ov7740_read (REG_MIDH, &midh);
    if (rc < 0)
        return rc;
    rc = ov7740_read (REG_MIDL, &midl);
    if (rc < 0)
        return rc;

    /* check OV product id - R0A=0x77, R0B=0x40 */
    rc = ov7740_read (REG_PID, &pid);
    if (rc < 0)
        return rc;
    rc = ov7740_read (REG_VER, &ver);
    if (rc < 0)
        return rc;

    /* check IDs - midh=0x7f, midl=0xa2, pid=0x77, ver=0x42 */
    DBG_PRINT("MIDH=%02x, MIDL=%02x, PID=%02x, VER=%02x\n", midh, midl, pid, ver);

    if (midh != 0x7f || midl != 0xa2 || pid != 0x77 || ver != 0x42) {
        ERROR ("OV Camera Chip init failed\n");
        //return -1;
    }

    /* init */
    DBG_PRINT ("set default registers of ov7740\n");
    for (i = 0; i < OV7740_CFG_NUM; i++) {
        rc = ov7740_write (ov7740_cfg[i].reg, ov7740_cfg[i].val);
        if (rc < 0)
            return rc;
    }
#if 0
	for (i = 0; i < OV7740_CFG_NUM; i++) {
        rc = ov7740_read (ov7740_cfg[i].reg, &ver);
		DBG_PRINT("reg=%02x, val=%02x\n",ov7740_cfg[i].reg ,ver);
        if (rc < 0)
            return rc;
    }
#endif	
    /* setting private data */
    pov->sat = 128;
    pov->hue = 0;
#endif
    return 0;
}
