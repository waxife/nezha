#ifndef IML_HANNSTAR_7_L_MODE_H_
#define IML_HANNSTAR_7_L_MODE_H_
#include "config.h"
#include "iml.h"
#include <sys.h>


/* crystal 27MHz */
static const struct reg panel_init_p0[]={
{0xE0,0xD8},
{0xE3,0x0D},
{0xE2,0x00},
{0x00,0xF0},
{0x01,0x02},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x09,0x40},
{0x91,0x87},
{0x9C,0x23},
{0x9D,0x00},
{0x9E,0x00},
{0x9F,0x6c},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
};
static const struct reg panel_init_p1[]={
{0x68,0x16},
{0x69,0x0D},
{0x6A,0x30},
{0x6B,0x05},
{0x6C,0x60},
{0x63,0x14},
{0x64,0x00},
{0x66,0x28},
{0x67,0x00},
{0x62,0x80},
{0x65,0x80},
};
static const struct reg panel_init_p2[]={
{0x3F,0x01},
{0x3F,0x00},
};
#define NUM_REG_PANEL_INITAL_P0 (sizeof(panel_init_p0) / sizeof(panel_init_p0[0]))
#define NUM_REG_PANEL_INITAL_P1 (sizeof(panel_init_p1) / sizeof(panel_init_p1[0]))
#define NUM_REG_PANEL_INITAL_P2 (sizeof(panel_init_p2) / sizeof(panel_init_p2[0]))
static const struct reg reg_hannstar_7_cvbs_page0[]={
/* ADC */
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x60},
{0x08,0x54},
{0x09,0x41},
{0x0A,0xD7},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x12},
/* Picture enhance */
{0x61,0x88},
{0x62,0x09},
{0x63,0x04},
{0x64,0x02},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
{0x68,0x80},
{0x69,0x80},
{0x6A,0x00},
{0x6B,0x7F},
/* Scaler */
{0x3D,0x8A},
{0x3E,0x16},
{0x3F,0x1C},
{0x70,0x20},
{0x72,0x73},
{0x73,0x25},
{0x74,0x00},
{0x75,0x11},
{0x85,0x1B},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x00},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0x4A},
{0xB9,0x05},
{0xBA,0x0C},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x09},
{0xBF,0x00},
{0xD8,0x20},
{0xDA,0x1F},
{0xDC,0x4F},
{0xDD,0x02},
{0xDE,0x90},
{0xDF,0x01},
/* freerun off */
{0xC0,0x01},
{0xC1,0x10},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
/* rgb swap */
{0xC7,0x00},
{0xE2,0x00},
};
#define NUM_REG_HANNSTAR_7_CVBS_PAGE0 (sizeof(reg_hannstar_7_cvbs_page0) / sizeof(reg_hannstar_7_cvbs_page0[0]))
static const struct reg reg_hannstar_7_cvbs_page2[]={
{0x00,0x00},
{0x01,0x09},
{0x02,0x4B},
{0x03,0x40},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x67},
{0x09,0x15},
{0x0A,0x59},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0xCF},
{0x10,0x7F},
{0x11,0x41},
{0x12,0x06},
{0x13,0x82},
{0x14,0x40},
{0x15,0x64},
{0x16,0x74},
{0x17,0xCB},
{0x18,0x21},
{0x19,0xF0},
{0x1A,0x7C},
{0x1B,0x1F},
{0x1C,0x20},
{0x1D,0x00},
{0x1E,0x00},
{0x1F,0x00},
{0x20,0x3E},
{0x21,0x3E},
{0x22,0x00},
{0x23,0x80},
{0x24,0xE9},
{0x25,0x0F},
{0x26,0x2D},
{0x27,0x50},
{0x28,0x22},
{0x29,0x4E},
{0x2A,0xD6},
{0x2B,0x4E},
{0x2C,0x23},
{0x2D,0x64},
{0x2E,0x78},
{0x2F,0x50},
{0x30,0x22},
{0x31,0x61},
{0x32,0x70},
{0x33,0x0E},
{0x34,0x6C},
{0x35,0x90},
{0x36,0x70},
{0x37,0x0E},
{0x38,0x00},
{0x39,0x09},
{0x40,0x0C},
{0x68,0x60},
{0x6C,0x01},
{0x80,0x10},
};
#define NUM_REG_HANNSTAR_7_CVBS_PAGE2 (sizeof(reg_hannstar_7_cvbs_page2) / sizeof(reg_hannstar_7_cvbs_page2[0]))
static const struct reg reg_hannstar_7_cvbs_pal_page0[]={
/* ADC */
{0x00,0x60},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x60},
{0x08,0xFF},
{0x09,0x41},
{0x0A,0xD7},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x09},
/* Picture enhance */
{0x61,0x88},
{0x62,0x0E},
{0x63,0x02},
{0x64,0x02},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
{0x68,0x80},
{0x69,0x80},
{0x6A,0x00},
{0x6B,0x7F},
/* Scaler */
{0x3D,0x01},
{0x3E,0x10},
{0x70,0x20},
{0x72,0x68},
{0x73,0x25},
{0x74,0x62},
{0x75,0x14},
{0x79,0x08},
{0x85,0x56},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x00},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0x59},
{0xB9,0x06},
{0xBA,0x6F},
{0xBB,0x02},
{0xBC,0x18},
{0xBD,0x00},
{0xBE,0x2A},
{0xBF,0x00},
{0xD8,0x1F},
{0xDA,0x24},
{0xDC,0x58},
{0xDC,0x58},
{0xDD,0x02},
{0xDE,0x90},
{0xDF,0x01},
/* freerun off */
//{0xC2,0x12},
{0xC0,0x01},
{0xC1,0x10},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
/* rgb swap */
{0xC7,0x00},
{0xE2,0x00},
};
#define NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE0 (sizeof(reg_hannstar_7_cvbs_pal_page0) / sizeof(reg_hannstar_7_cvbs_pal_page0[0]))
static const struct reg reg_hannstar_7_cvbs_pal_page2[]={
{0x00,0x32},
{0x01,0x08},
{0x02,0x4B},
{0x03,0x4A},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x5E},
{0x09,0x15},
{0x0A,0x67},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0xCF},
{0x10,0x7F},
{0x11,0x89},
{0x12,0x06},
{0x13,0x82},
{0x14,0x40},
{0x15,0x64},
{0x16,0x74},
{0x17,0xCB},
{0x18,0x2A},
{0x19,0x09},
{0x1A,0x8A},
{0x1B,0xCD},
{0x1C,0x20},
{0x1D,0x00},
{0x1E,0x00},
{0x1F,0x00},
{0x20,0x3E},
{0x21,0x3E},
{0x22,0x00},
{0x23,0x80},
{0x24,0xE9},
{0x25,0x0F},
{0x26,0x2D},
{0x27,0x50},
{0x28,0x22},
{0x29,0x4E},
{0x2A,0xD6},
{0x2B,0x4E},
{0x2C,0x23},
{0x2D,0x64},
{0x2E,0x78},
{0x2F,0x50},
{0x30,0x2D},
{0x31,0xC1},
{0x32,0x70},
{0x33,0x0E},
{0x34,0x6C},
{0x35,0x90},
{0x36,0x70},
{0x37,0x0E},
{0x38,0x00},
{0x39,0x09},
{0x40,0x0C},
{0x68,0x60},
{0x6C,0x02},
{0x80,0x11},
};
#define NUM_REG_HANNSTAR_7_CVBS_PAL_PAGE2 (sizeof(reg_hannstar_7_cvbs_pal_page2) / sizeof(reg_hannstar_7_cvbs_pal_page2[0]))
#endif
