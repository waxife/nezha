/**
 *  @file   ov7725.c
 *  @brief  omniVision camera chip driver
 *  $Id: ov7725.c,v 1.1.1.1 2013/12/18 03:43:49 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/09/24  hugo  New file. (idea from ov7725.c in linux-2.6.36)
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
#define OV7725_FRAME_RATE 30

/*
 * The 7725 sits on i2c with ID 0x42
 */
#define OV7725_I2C_ADDR 0x42

/*
 * OV7725 Device Control Register List
 */
#define REG_GAIN        0x00    /* Gain lower 8 bits (rest in vref) */
#define REG_BLUE        0x01    /* blue gain */
#define REG_RED         0x02    /* red gain */
#define REG_VREF        0x03    /* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1        0x04    /* Control 1 (reserved) */
#define REG_BAVE        0x05    /* U/B Average level */
#define REG_GbAVE       0x06    /* Y/Gb Average level */
#define REG_RAVE        0x07    /* V/R Average level */
#define REG_AECH        0x08    /* AEC MSBs [15:8] */
#define REG_COM2        0x09    /* Control 2 */
#define   COM2_SSLEEP   0x10    /* Soft sleep mode */
#define REG_PID         0x0a    /* Product ID MSB */
#define REG_VER         0x0b    /* Product ID LSB */
#define REG_COM3        0x0c    /* Control 3 */
#define   COM3_FLIP     0x80    /* Vertical flip */
#define   COM3_MIRROR   0x40    /* Mirror image */
#define   COM3_SWAP     0x08    /* Byte swap */
#define   COM3_CBARTP   0x01    /* Sensor color bar test pattern enable */
#define REG_COM4        0x0d    /* Control 4 */
#define REG_COM5        0x0e    /* Control 5 */
#define   COM5_AFRMC    0x80    /*   auto frame rate control on/off */
#define REG_COM6        0x0f    /* Control 6 */
#define REG_AEC         0x10    /* More bits of AEC value */
#define REG_CLKRC       0x11    /* Clock control */
#define   CLK_EXT       0x40    /* Use external clock directly */
#define   CLK_SCALE     0x3f    /* Mask for internal clock scale */
#define REG_COM7        0x12    /* Control 7 */
#define   COM7_RESET    0x80    /* Register reset */
#define   COM7_FMT_MASK 0x40
#define   COM7_FMT_VGA  0x00
#define   COM7_FMT_QVGA 0x40    /* QVGA format */
#define   COM7_BT656    0x20    /* BT.656 protocol on/off */
#define   COM7_RGB_MASK 0x0c    /* bits 0 and 2 - RGB format */
#define   COM7_YU       0x00    /* YUV */
#define   COM7_RGB      0x10    /* RGB */
#define   COM7_BAYER    0x11    /* Bayer format */
#define   COM7_PBAYER   0x01    /* "Processed bayer" */
#define REG_COM8        0x13    /* Control 8 */
#define   COM8_FASTAEC  0x80    /* Enable fast AGC/AEC */
#define   COM8_AECSTEP  0x40    /* Unlimited AEC step size */
#define   COM8_BFILT    0x20    /* Band filter enable */
#define   COM8_AECBAND  0x10    /* Enable AEC below banding value */
#define   COM8_FINEAEC  0x08    /* Fine AEC enable */
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
#define   COM10_D_RNG   0x01    /* Data from [10] to [F0](8 MSBs) */
#define REG_HSTART      0x17    /* Horiz start high bits */
#define REG_HSTOP       0x18    /* Horiz stop high bits */
#define REG_VSTART      0x19    /* Vert start high bits */
#define REG_VSTOP       0x1a    /* Vert stop high bits */
#define REG_PSHFT       0x1b    /* Pixel delay after HREF */
#define REG_MIDH        0x1c    /* Manuf. ID high */
#define REG_MIDL        0x1d    /* Manuf. ID low */
#define REG_COM11       0x20    /* Control 11 */
#define   COM11_SFRM    0x02    /* single frame on/off */
#define   COM11_SFRMT   0x01    /* single frame transfer trigger */

#define REG_AEW         0x24    /* AGC/AEC upper limit */
#define REG_AEB         0x25    /* AGC/AEC lower limit */
#define REG_VPT         0x26    /* AGC/AEC fast mode op region */
#define REG_HREF        0x32    /* HREF pieces */
#define REG_COM12       0x3d    /* Control 12 */
#define   COM12_DCOFS   0x3f    /* Mask for DC offset compensation for analog process */
#define REG_COM13       0x3e    /* Control 13 */
#define   COM13_BLC     0x80    /* Analog processing channel BLC on/off */
#define   COM13_ADCBLC  0x40    /* ADC channel BLC on/off */
#define REG_COM14       0x3f    /* Control 14 */
#define   COM14_ADOFSM  0x0c    /* Mask for Edge Enhancement Adjustment - AD offset compensation */
#define   COM14_EADR2B  0x00    /* Use R/Gr channel value for B/Gb */
#define   COM14_EADB2R  0x04    /* Use B/Gb channel value for R/Gr */
#define   COM14_EADIDP  0x0c    /* Use B/Gb/R/Gr channel value independently */
#define   COM14_APOFSM  0x03    /* Mask for Edge Enhancement Adjustment - analog processing offset compensation */
#define   COM14_EAPR2B  0x00    /* Use R/Gr channel value for B/Gb */
#define   COM14_EAPB2R  0x04    /* Use B/Gb channel value for R/Gr */
#define   COM14_EAPIDP  0x0c    /* Use B/Gb/R/Gr channel value independently */
#define REG_COM15       0x40    /* Control 15 */
#define   COM15_AD128   0x08    /* AD add 128 bit offset */
#define REG_COM16       0x41    /* Control 16 */
#define   COM16_BLC2LSB 0x03    /* Mask for BLC target 2 LSBs */
#define REG_EDGE0       0x8f    /* Edge enhancement control 0 */
#define   EDGE0_STRN    0x1f    /* Mask for sharpness(edge enhancement) strength */

/*
 * This matrix defines how the colors are generated, must be
 * tweaked to adjust hue and saturation.
 *
 * Order: v-red, v-green, v-blue, u-red, u-green, u-blue
 *
 * They are nine-bit signed quantities, with the sign bit
 * stored in 0x58.  Sign for v-red is bit 0, and up from there.
 */
#define REG_CMATRIX_BASE 0x94
#define CMATRIX_LEN 6
#define REG_CMATRIX_SIGN 0x9a


#define REG_BRIGHT      0x9b    /* Brightness */
#define REG_CONTRAS     0x9c    /* Contrast control */

#define REG_GFIX        0x4d    /* Analog fix gain Amplifier */

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

//#define OV7725_60FPS    1
#define OV7725_30FPS    1
//#define OV7725_25FPS    1
static const struct regval_list ov7725_cfg[] = {

/* the following is based on XCLK = 24MHz */
/* frame rate fomula:
 * fps * (640+144) * (510 + dummy_rows) * 2 = 24M(PCLK) */

#ifdef OV7725_60FPS
// 60fps at 24MHz input clock
// all default value

#elif OV7725_30FPS
// 30fps at 24MHz input clock, for power 60Hz
{0x11,0x01}, // clock pre-scalar
{0x0e,0x65}, // COM5, frame rate control

#elif OV7725_25FPS
// 25fps at 24MHz input clock, for power 50Hz
{0x11,0x01}, // clock pre-scalar
{0x33,0x66}, // number of dummy rows, LSBs
{0x0e,0x65}, // COM5, frame rate control
#endif

};

#define OV7725_CFG_NUM  (sizeof(ov7725_cfg) / sizeof(ov7725_cfg[0]))

/*
 * Low-level register I/O
 */

int
ov7725_write (unsigned char reg, unsigned char value)
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
ov7725_read (unsigned char reg, unsigned char *value)
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
 * Synopsis     int ov7725_set_reg (struct ovc *pov, unsigned char reg, unsigned char val);
 * Description  write value to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_reg (struct ovc *pov, unsigned char reg, unsigned char val)
{
    return ov7725_write (reg, val);
}

/*
 * Synopsis     int ov7725_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk);
 * Description  write bits at positions specified by mask to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 *              msk - enable bit mask
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk)
{
    unsigned char old, new;
    int rc;

    rc = ov7725_read (reg, &old);
    if (rc < 0) {
        ERROR ("ov7725_read(%02X)\n", reg);
        return rc;
    }

    new = (old & ~msk) | (val & msk);
    rc = ov7725_write (reg, new);
    if (rc < 0) {
        ERROR ("ov7725_write(%02X, %02X)\n", reg, new);
        return rc;
    }

    return 0;
}

/*
 * Synopsis     int ov7725_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val);
 * Description  read value from a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val)
{
    return ov7725_read (reg, val);
}

/*
 * Synopsis     void ov7725_dump (struct ovc *pov);
 * Description  dump all registers
 * Parameters   pov - point to ov private data
 * Return       none
 */
void
ov7725_dump (struct ovc *pov)
{
    int reg;
    unsigned char val;
    int rc;

    for (reg = 0; reg < 256; reg++) {
        if (reg % 16 == 0)
            DBG_PRINT ("%02X    ", reg);

        rc = ov7725_read (reg, &val);
        if (rc < 0) {
            ERROR ("ov7725_read (%02X)\n", reg);
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
static const int ov7725_cmatrix[CMATRIX_LEN] = { 128, -128, 0, -34, -94, 128 };

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
static const int ov7725_sin_table[] = {
    0, 87, 173, 258, 342, 422,
    499, 573, 642, 707, 766, 819,
    866, 906, 939, 965, 984, 996,
    1000
};

static int
ov7725_sine (int theta)
{
    int chs = 1;
    int sine;

    if (theta < 0) {
        theta = -theta;
        chs = -1;
    }
    if (theta <= 90)
        sine = ov7725_sin_table[theta / SIN_STEP];
    else {
        theta -= 90;
        sine = 1000 - ov7725_sin_table[theta / SIN_STEP];
    }
    return sine * chs;
}

static int
ov7725_cosine (int theta)
{
    theta = 90 - theta;
    if (theta > 180)
        theta -= 360;
    else if (theta < -180)
        theta += 360;
    return ov7725_sine (theta);
}

static void
ov7725_calc_cmatrix (struct ovc *pov, int matrix[CMATRIX_LEN])
{
    int i;
    /*
     * Apply the current saturation setting first.
     */
    for (i = 0; i < CMATRIX_LEN; i++)
        matrix[i] = (ov7725_cmatrix[i] * pov->sat) >> 7;
    /*
     * Then, if need be, rotate the hue value.
     */
    if (pov->hue != 0) {
        int sinth, costh, tmpmatrix[CMATRIX_LEN];

        memcpy (tmpmatrix, matrix, CMATRIX_LEN * sizeof (int));
        sinth = ov7725_sine (pov->hue);
        costh = ov7725_cosine (pov->hue);

        matrix[0] = (tmpmatrix[3] * sinth + tmpmatrix[0] * costh) / 1000;
        matrix[1] = (tmpmatrix[4] * sinth + tmpmatrix[1] * costh) / 1000;
        matrix[2] = (tmpmatrix[5] * sinth + tmpmatrix[2] * costh) / 1000;
        matrix[3] = (tmpmatrix[3] * costh - tmpmatrix[0] * sinth) / 1000;
        matrix[4] = (tmpmatrix[4] * costh - tmpmatrix[1] * sinth) / 1000;
        matrix[5] = (tmpmatrix[5] * costh - tmpmatrix[2] * sinth) / 1000;
    }
}

static int
ov7725_store_cmatrix (int matrix[CMATRIX_LEN])
{
    int i, ret;
    unsigned char signbits = 0;

    /*
     * Weird crap seems to exist in the upper part of
     * the sign bits register, so let's preserve it.
     */
    ret = ov7725_read (REG_CMATRIX_SIGN, &signbits);
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
        ret += ov7725_write (REG_CMATRIX_BASE + i, raw);
    }
    ret += ov7725_write (REG_CMATRIX_SIGN, signbits);
    return ret;
}

/*
 * Synopsis     int ov7725_set_sat (struct ovc *pov, unsigned char val);
 * Description  set saturation
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_sat (struct ovc *pov, unsigned char val)
{
    int matrix[CMATRIX_LEN];
    int rc;

    pov->sat = val;
    ov7725_calc_cmatrix (pov, matrix);
    rc = ov7725_store_cmatrix (matrix);
    return rc;
}

/*
 * Synopsis     int ov7725_get_sat (struct ovc *pov, unsigned char *val);
 * Description  get saturation
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_sat (struct ovc *pov, unsigned char *val)
{
    *val = pov->sat;
    return 0;
}

/*
 * Synopsis     int ov7725_set_hue (struct ovc *pov, int val);
 * Description  set hue
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_hue (struct ovc *pov, int val)
{
    int matrix[CMATRIX_LEN];
    int rc;

    if (val < -180 || val > 180)
        return -1;

    pov->hue = val;
    ov7725_calc_cmatrix (pov, matrix);
    rc = ov7725_store_cmatrix (matrix);
    return rc;
}

/*
 * Synopsis     int ov7725_get_hue (struct ovc *pov, int *val);
 * Description  get hue
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_hue (struct ovc *pov, int *val)
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
 * Synopsis     int ov7725_set_bright (struct ovc *pov, unsigned char val);
 * Description  set brightness
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_bright (struct ovc *pov, unsigned char val)
{
    int rc;

    /* enable bright/contrast control */
    rc = ov7725_write (0xa6, 0x04);
    if (rc < 0) goto EXIT;

    /* sing bit:
     *   register address 0xab, bit [3], but also must set bit[2] */
    if (val > 127) {
        rc = ov7725_write (0xab, 0x06);
        if (rc < 0) goto EXIT;
        rc = ov7725_write (REG_BRIGHT, val & 0x7f);
    } else {
        rc = ov7725_write (0xab, 0x0e);
        if (rc < 0) goto EXIT;
        rc = ov7725_write (REG_BRIGHT, (128 - val));
    }

EXIT:
    return rc;
}

/*
 * Synopsis     int ov7725_get_bright (struct ovc *pov, unsigned char *val);
 * Description  get brightness
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_bright (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0, sign = 0;
    int rc = ov7725_read (REG_BRIGHT, &v);

    rc = ov7725_read (0xab, &v);
    if (rc < 0) goto EXIT;
    sign = (v >> 3) & 0x1;

    rc = ov7725_read (REG_BRIGHT, &v);
    if (sign)
        *val = 128 - (v & 0x7f);
    else
    *val = v + 128;

EXIT:
    return rc;
}

/*
 * Synopsis     int ov7725_set_contrast (struct ovc *pov, unsigned char val);
 * Description  set contrast
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_contrast (struct ovc *pov, unsigned char val)
{
    int rc = 0;

    /* enable bright/contrast control */
    rc = ov7725_write (0xa6, 0x04);
    if (rc < 0) return rc;

    /* constrast range: (-4 ~ +4)
     * 0 : val 0x20
     * +1: val +4
     * -1: val -4
     */
    return ov7725_write (REG_CONTRAS, val);
}

/*
 * Synopsis     int ov7725_get_contrast (struct ovc *pov, unsigned char *val);
 * Description  get contrast
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_contrast (struct ovc *pov, unsigned char *val)
{
    return ov7725_read (REG_CONTRAS, val);
}

/*
 * Synopsis     int ov7725_set_vflip (struct ovc *pov, unsigned char val);
 * Description  set virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_vflip (struct ovc *pov, unsigned char val)
{
    unsigned char v = 0;

    ov7725_read (REG_COM3, &v);
    if (val)
        v |= COM3_FLIP;
    else
        v &= ~COM3_FLIP;

    return ov7725_write (REG_COM3, v);
}

/*
 * Synopsis     int ov7725_get_vflip (struct ovc *pov, unsigned char *val);
 * Description  get virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_vflip (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0;
    int rc;

    rc = ov7725_read (REG_COM3, &v);
    *val = (v & COM3_FLIP) == COM3_FLIP;

    return rc;
}

/*
 * Synopsis     int ov7725_set_hflip (struct ovc *pov, unsigned char val);
 * Description  set horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_hflip (struct ovc *pov, unsigned char val)
{
    unsigned char v = 0;

    ov7725_read (REG_COM3, &v);
    if (val)
        v |= COM3_MIRROR;
    else
        v &= ~COM3_MIRROR;

    return ov7725_write (REG_COM3, v);
}

/*
 * Synopsis     int ov7725_get_hflip (struct ovc *pov, unsigned char *val);
 * Description  get horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_hflip (struct ovc *pov, unsigned char *val)
{
    unsigned char v = 0;
    int rc;

    rc = ov7725_read (REG_COM3, &v);
    *val = (v & COM3_MIRROR) == COM3_MIRROR;

    return rc;
}

/*
 * Synopsis     int ov7725_set_gain (struct ovc *pov, unsigned char val);
 * Description  set gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_gain (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7725_write (REG_GAIN, val);
    if (rc < 0)
        return rc;

    /* disable AGC */
    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    rc = ov7725_write (REG_COM8, com8 & ~COM8_AGC);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7725_get_gain (struct ovc *pov, unsigned char *val);
 * Description  get gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_gain (struct ovc *pov, unsigned char *val)
{
    return ov7725_read (REG_GAIN, val);
}

/*
 * Synopsis     int ov7725_set_autogain (struct ovc *pov, unsigned char val);
 * Description  set auto gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_autogain (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    if (val)
        com8 |= COM8_AGC;
    else
        com8 &= ~COM8_AGC;
    rc = ov7725_write (REG_COM8, com8);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7725_get_autogain (struct ovc *pov, unsigned char *val);
 * Description  get auto gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_autogain (struct ovc *pov, unsigned char *val)
{
    unsigned char com8 = 0;
    int rc;

    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    *val = (com8 & COM8_AGC) != 0;
    return 0;
}

/*
 * Synopsis     int ov7725_set_exp (struct ovc *pov, unsigned short val);
 * Description  set exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_exp (struct ovc *pov, unsigned short val)
{
    unsigned char com8, aec, aech;
    int rc;

    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    aec = val & 0xff;
    aech = (val >> 8) & 0xff;

    rc = ov7725_write (REG_AEC, aec) + ov7725_write (REG_AECH, aech);
    if (rc < 0)
        return rc;

    /* disable AEC */
    rc = ov7725_write (REG_COM8, com8 & ~COM8_AEC);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7725_get_exp (struct ovc *pov, unsigned char *val);
 * Description  get exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_exp (struct ovc *pov, unsigned short *val)
{
    unsigned char aec, aech;
    int rc;

    rc = ov7725_read (REG_AEC, &aec) + ov7725_read (REG_AECH, &aech);
    if (rc < 0)
        return rc;

    *val = (aech << 8) | aec;
    return 0;
}

/*
 * Synopsis     int ov7725_set_autoexp (struct ovc *pov, unsigned char val);
 * Description  set auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int
ov7725_set_autoexp (struct ovc *pov, unsigned char val)
{
    unsigned char com8;
    int rc;

    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    if (val)
        com8 |= COM8_AEC;
    else
        com8 &= ~COM8_AEC;
    rc = ov7725_write (REG_COM8, com8);
    if (rc < 0)
        return rc;

    return 0;
}

/*
 * Synopsis     int ov7725_get_autoexp (struct ovc *pov, unsigned char *val);
 * Description  get auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int
ov7725_get_autoexp (struct ovc *pov, unsigned char *val)
{
    unsigned char com8;
    int rc;

    rc = ov7725_read (REG_COM8, &com8);
    if (rc < 0)
        return rc;

    *val = (com8 & COM8_AEC) != 0;
    return 0;

}

/*
 * Synopsis     int ov7725_init (struct ovc *pov);
 * Description  init ov camera chip
 *                - reset chip
 *                - check manufacturer ID, product ID, Version
 *                - setup hardware window (HREF, VREF)
 *                - setup registers (scaleing, gamma, AGC, AEC, color matrix)
 * Parameters   pov - point to ov private data
 * Return       0 for success, or -1 for any error
 */
int
ov7725_init (struct ovc *pov)
{
	
    unsigned char midh, midl, pid, ver;
    int i, rc;
	
    /* init sccb bus */
    sccb_init ();

#if 1
    /* reset chip */
    ov7725_write (REG_COM7, COM7_RESET);

    /* check OV manufacturer id - R1C=0x7f, R1D=0xa2 */
    rc = ov7725_read (REG_MIDH, &midh);
    if (rc < 0)
        return rc;
    rc = ov7725_read (REG_MIDL, &midl);
    if (rc < 0)
        return rc;

    /* check OV product id - R0A=0x77, R0B=0x21 */
    rc = ov7725_read (REG_PID, &pid);
    if (rc < 0)
        return rc;
    rc = ov7725_read (REG_VER, &ver);
    if (rc < 0)
        return rc;

    /* check IDs - midh=0x7f, midl=0xa2, pid=0x77, ver=0x21 */
    DBG_PRINT("MIDH=%02x, MIDL=%02x, PID=%02x, VER=%02x\n", midh, midl, pid, ver);

    if (midh != 0x7f || midl != 0xa2 || pid != 0x77 || ver != 0x21) {
        ERROR ("OV Camera Chip init failed\n");
        //return -1;
    }

    /* init */
#if 1
    DBG_PRINT ("set default registers of ov7725\n");
    for (i = 0; i < OV7725_CFG_NUM; i++) {
        rc = ov7725_write (ov7725_cfg[i].reg, ov7725_cfg[i].val);
        if (rc < 0)
            return rc;
    }
#endif
#if 0
    for (i = 0; i < OV7725_CFG_NUM; i++) {
        rc = ov7725_read (ov7725_cfg[i].reg, &ver);
		DBG_PRINT("reg=%02x, val=%02x\n",ov7725_cfg[i].reg ,ver);
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
