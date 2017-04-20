#ifndef IML_CVBS_H
#define IML_CVBS_H
#include "config.h"
#include "iml.h"
#include <sys.h>

static const struct reg panel_init_p0[]={
{0xE0,0xD8},
{0xE3,0x0D},
{0xE2,0x00},
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x09,0x41},
{0x12,0x18},
{0x13,0x08},
{0x91,0x30},
{0x9C,0x23},
{0x9D,0x00},
{0x9E,0x00},
{0x9F,0x6C},
{0xB0,0xC8},
{0xB2,0x2B},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0x58},
{0xB7,0x02},
{0xB8,0xD0},
{0xB9,0x04},
{0xBA,0x8A},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xC0,0x01},
{0xC1,0x18},
{0xC4,0x10},
{0xC7,0x0D},
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
{0xB0,0xA3},
{0xB2,0xDF},
{0xB3,0x95},
{0xB4,0x9F},
{0xB5,0xA6},
{0xB6,0x95},
{0xB7,0x97},
};
static const struct reg panel_init_p2[]={
{0x3F,0x01},
{0x3F,0x00},
};
#define NUM_REG_PANEL_INITAL_P0 (sizeof(panel_init_p0) / sizeof(panel_init_p0[0]))
#define NUM_REG_PANEL_INITAL_P1 (sizeof(panel_init_p1) / sizeof(panel_init_p1[0]))
#define NUM_REG_PANEL_INITAL_P2 (sizeof(panel_init_p2) / sizeof(panel_init_p2[0]))

static const struct reg reg_cvbs_ntsc_full_page0[]={
/* ADC */
{0x00,0xF2},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0x3B},
{0x09,0x01},
{0x0A,0xF0},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x12},
{0x3D,0x00},
{0x3E,0x10},
/* Picture enhance */
{0x61,0x08},
{0x62,0x04},
{0x63,0x04},
{0x64,0x02},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
/* Scaler */
{0x70,0x20},
{0x72,0x14},
{0x73,0x1C},
{0x74,0xA2},
{0x75,0x0F},
{0x85,0x0F},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x28},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0xDC},
{0xB9,0x04},
{0xBA,0x6F},
{0xBB,0x01},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x00},
{0xD9,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0xE0},
{0xDF,0x01},
/* freerun off */
{0xC0,0x01},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00}
};
#define NUM_REG_CVBS_NTSC_FULL_PAGE0 (sizeof(reg_cvbs_ntsc_full_page0) / sizeof(reg_cvbs_ntsc_full_page0[0]))
static const struct reg reg_cvbs_ntsc_full_page2[]={
{0x00,0x00},
{0x01,0x09},
{0x02,0x4B},
{0x03,0x40},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x7B},
{0x09,0x20},
{0x0A,0xAB},
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
{0x4B,0xBF},
{0x68,0x60},
{0x6C,0x02},
{0x80,0x13},
};
#define NUM_REG_CVBS_NTSC_FULL_PAGE2 (sizeof(reg_cvbs_ntsc_full_page2) / sizeof(reg_cvbs_ntsc_full_page2[0]))
static const struct reg reg_cvbs_ntsc_lmod_page0[]={
/* ADC */
{0x00,0xF2},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x60},
{0x08,0x54},
{0x09,0x01},
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
/* Scaler */
{0x3D,0x01},
{0x3E,0x03},
{0x3F,0x1C},
{0x70,0x20},
{0x72,0x73},
{0x73,0x25},
{0x74,0xE1},
{0x75,0x10},
{0x85,0x0D},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x09},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0x3F},
{0xB9,0x05},
{0xBA,0x0C},
{0xBB,0x01},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x20},
{0xDA,0x1A},
{0xDC,0x4F},
{0xDD,0x02},
{0xDE,0x90},
{0xDF,0x01},
/* freerun off */
{0xC0,0x01},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};
#define NUM_REG_CVBS_NTSC_LMOD_PAGE0 (sizeof(reg_cvbs_ntsc_lmod_page0) / sizeof(reg_cvbs_ntsc_lmod_page0[0]))
static const struct reg reg_cvbs_ntsc_lmod_page2[]={
{0x00,0x00},
{0x01,0x09},
{0x02,0x4B},
{0x03,0x40},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x7B},
{0x09,0x20},
{0x0A,0xAB},
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
{0x4B,0xBF},
{0x68,0x60},
{0x6C,0x02},
{0x80,0x13},
};
#define NUM_REG_CVBS_NTSC_LMOD_PAGE2 (sizeof(reg_cvbs_ntsc_lmod_page2) / sizeof(reg_cvbs_ntsc_lmod_page2[0]))
static const struct reg reg_cvbs_pal_lmod_page0[]={
/* ADC */
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0xE8},
{0x09,0x41},
{0x0A,0xF0},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x09},
/* Picture enhance */
{0x61,0x08},
{0x62,0x02},
{0x63,0x04},
{0x64,0x05},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
/* Scaler */
{0x3D,0x00},
{0x3E,0x13},
{0x70,0x20},
{0x72,0x70},
{0x73,0x1C},
{0x74,0x5C},
{0x75,0x0F},
{0x78,0x00},
{0x85,0x14},
/* Output timing */
{0xB0,0xC8},
{0xB1,0x00},
{0xB2,0x2B},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0x58},
{0xB7,0x02},
{0xB8,0xD0},
{0xB9,0x04},
{0xBA,0x8A},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0x58},
{0xDF,0x02},
/* freerun off */
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};
#define NUM_REG_CVBS_PAL_LMOD_PAGE0 (sizeof(reg_cvbs_pal_lmod_page0) / sizeof(reg_cvbs_pal_lmod_page0[0]))
static const struct reg reg_cvbs_pal_lmod_page2[]={
{0x00,0x32},
{0x01,0x08},
{0x02,0x4B},
{0x03,0x4A},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x80},
{0x09,0x20},
{0x0A,0x80},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0x4C},
{0x10,0x48},
{0x11,0x41},
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
{0x2E,0x84},
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
{0x4B,0xB7},
{0x68,0x00},
{0x6C,0x02},
{0x80,0x17},
};
#define NUM_REG_CVBS_PAL_LMOD_PAGE2 (sizeof(reg_cvbs_pal_lmod_page2) / sizeof(reg_cvbs_pal_lmod_page2[0]))
static const struct reg reg_play_pal_lmod_page0[]={
/* ADC */
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0xE8},
{0x09,0x41},
{0x0A,0xF0},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x09},
/* Picture enhance */
{0x61,0x08},
{0x62,0x02},
{0x63,0x04},
{0x64,0x05},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
{0x68,0x80},
{0x69,0x80},
{0x6A,0x00},
{0x6B,0x7F},
/* Scaler */
{0x3D,0x00},
{0x3E,0x13},
{0xE0,0xD8},
{0x72,0x70},
{0x73,0x1C},
{0x74,0x5C},
{0x75,0x0F},
{0x78,0x00},
{0x85,0x14},
/* Output timing */
{0xB0,0xC8},
{0xB1,0x00},
{0xB2,0x2B},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0x58},
{0xB7,0x02},
{0xB8,0xD0},
{0xB9,0x04},
{0xBA,0x8A},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x00},
{0xD9,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0x58},
{0xDF,0x02},
/* freerun off */
{0xC0,0x01},
{0xC1,0x18},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};
#define NUM_REG_PLAY_PAL_LMOD_PAGE0 (sizeof(reg_play_pal_lmod_page0) / sizeof(reg_play_pal_lmod_page0[0]))
static const struct reg reg_play_pal_lmod_page2[]={
{0x00,0x32},
{0x01,0x08},
{0x02,0x4B},
{0x03,0x4A},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x80},
{0x09,0x20},
{0x0A,0x80},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0xCF},
{0x10,0x48},
{0x11,0x41},
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
{0x2E,0x84},
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
{0x00,0x0C},
{0x80,0x17},
};
#define NUM_REG_PLAY_PAL_LMOD_PAGE2 (sizeof(reg_play_pal_lmod_page2) / sizeof(reg_play_pal_lmod_page2[0]))

/*used*/
static const struct reg reg_cvbs_pal_full_page0[]={
/* ADC */
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0xD0},
{0x09,0x41},
{0x0A,0xF0},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x09},
/* Picture enhance */
{0x61,0xc0},//bob
{0x62,0x0d},//bob
{0x63,0x06},//bob
{0x64,0x01},//bob
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
/* Scaler */
{0x3D,0x00},
{0x3E,0x13},
{0x70,0x20},
{0x72,0x00},/*for edge issue at right side*/
{0x73,0x1C},
{0x74,0x5C},
{0x75,0x0F},
{0x78,0x00},
{0x85,0x14},
/* Output timing */
{0xB0,0xC8},
{0xB1,0x00},
{0xB2,0x2B},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0x58},
{0xB7,0x02},
{0xB8,0xD0},
{0xB9,0x04},
{0xBA,0x8A},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0x58},
{0xDF,0x02},
/* freerun off */
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};
#define NUM_REG_CVBS_PAL_FULL_PAGE0 (sizeof(reg_cvbs_pal_full_page0) / sizeof(reg_cvbs_pal_full_page0[0]))
static const struct reg reg_cvbs_pal_full_page2[]={
{0x00,0x32},
{0x01,0x09},//bob
{0x02,0x4B},
{0x03,0x4A},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x50},//bob
{0x09,0x12},//bob
{0x0A,0x60},//bob
{0x0B,0x00},
{0x0C,0x60},//bob
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0xCF},
{0x10,0x48},
{0x11,0x41},
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
{0x2E,0x84},
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
{0x4B,0xB7},
{0x68,0x60},
{0x6C,0x02},
{0x80,0x17},
};
#define NUM_REG_CVBS_PAL_FULL_PAGE2 (sizeof(reg_cvbs_pal_full_page2) / sizeof(reg_cvbs_pal_full_page2[0]))

static const struct reg reg_play_pal_full_page0[]={
/* ADC */
{0x00,0xF0},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0xD0},
{0x09,0x41},
{0x0A,0xF0},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x09},
/* Picture enhance */
{0x61,0xc0},//bob
{0x62,0x0d},//bob
{0x63,0x06},//bob
{0x64,0x01},//bob
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
{0x68,0x80},
{0x69,0x80},
{0x6A,0x00},
{0x6B,0x7F},
/* Scaler */
{0x3D,0x00},
{0x3E,0x13},
{0xE0,0xD8},
{0x72,0x00},
{0x73,0x1C},
{0x74,0x5C},
{0x75,0x0F},
{0x78,0x00},
{0x85,0x14},
/* Output timing */
{0xB0,0xC8},
{0xB1,0x00},
{0xB2,0x26},//bob
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0x58},
{0xB7,0x02},
{0xB8,0xAB},//bob
{0xB9,0x04},
{0xBA,0x8A},
{0xBB,0x02},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x00},
{0xD9,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0x58},
{0xDF,0x02},
/* freerun off */
{0xC0,0x01},
{0xC1,0x18},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};

#define NUM_REG_PLAY_PAL_FULL_PAGE0 (sizeof(reg_play_pal_full_page0) / sizeof(reg_play_pal_full_page0[0]))
static const struct reg reg_play_pal_full_page2[]={
{0x00,0x32},
{0x01,0x08},
{0x02,0x4B},
{0x03,0x4A},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x80},
{0x09,0x20},
{0x0A,0x70},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0xCF},
{0x10,0x48},
{0x11,0x41},
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
{0x2E,0x84},
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
{0x00,0x0C},
{0x80,0x17},
};

#define NUM_REG_PLAY_PAL_FULL_PAGE2 (sizeof(reg_play_pal_full_page2) / sizeof(reg_play_pal_full_page2[0]))

/*not used*/
static const struct reg reg_play_ntsc_lmod_page0[]={
/* ADC */
{0x00,0xF2},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x60},
{0x08,0x54},
{0x09,0x01},
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
/* Scaler */
{0x3D,0x01},
{0x3E,0x03},
{0x3F,0x1C},
{0x70,0x20},
{0x72,0x73},
{0x73,0x25},
{0x74,0xE1},
{0x75,0x10},
{0x85,0x0D},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x09},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0x48},
{0xB9,0x05},
{0xBA,0xD4},
{0xBB,0x01},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x04},
{0xBF,0x00},
{0xD8,0x20},
{0xDA,0x1A},
{0xDC,0x4F},
{0xDD,0x02},
{0xDE,0x90},
{0xDF,0x01},
/* freerun off */
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00},
};
static const struct reg reg_play_ntsc_lmod_page2[]={
{0x00,0x00},
{0x01,0x09},
{0x02,0x4B},
{0x03,0x40},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA1},
{0x08,0x81},
{0x09,0x34},
{0x0A,0xAB},
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
{0x6C,0x02},
{0x80,0x13},
};
static const struct reg reg_play_ntsc_full_page0[]={
/* ADC */
{0x00,0x60},
{0x01,0x00},
{0x02,0x28},
{0x06,0x80},
{0x07,0x70},
{0x08,0x3B},
{0x09,0x41},
/* source select */
/* CVBS input */
{0x0E,0x01},
{0x3C,0x12},
{0x3D,0x00},
{0x3E,0x10},
/* Picture enhance */
{0x61,0x08},
{0x62,0x04},
{0x63,0x04},
{0x64,0x02},
{0x65,0x08},
{0x66,0x08},
{0x67,0x1E},
/* Scaler */
{0x70,0x20},
{0x72,0x14},
{0x73,0x1C},
{0x74,0xA2},
{0x75,0x0F},
{0x85,0x15},
/* Output timing */
{0xB0,0x80},
{0xB1,0x00},
{0xB2,0x28},
{0xB3,0x00},
{0xB4,0x20},
{0xB5,0x03},
{0xB6,0xE0},
{0xB7,0x01},
{0xB8,0xDC},
{0xB9,0x04},
{0xBA,0x6F},
{0xBB,0x01},
{0xBC,0x20},
{0xBD,0x00},
{0xBE,0x10},
{0xBF,0x00},
{0xD8,0x00},
{0xD9,0x00},
{0xDA,0x00},
{0xDB,0x00},
{0xDC,0x20},
{0xDD,0x03},
{0xDE,0xE0},
{0xDF,0x01},
/* freerun off */
{0xC0,0x01},
{0xC2,0x00},
/* dpll divider */
{0xC4,0x10},
{0xE2,0x00}
};
static const struct reg reg_play_ntsc_full_page2[]={
{0x00,0x00},
{0x01,0x09},
{0x02,0x4B},
{0x03,0x40},
{0x04,0xDD},
{0x05,0x32},
{0x06,0x0A},
{0x07,0xA0},
{0x08,0x81},
{0x09,0x34},
{0x0A,0xAB},
{0x0B,0x00},
{0x0C,0x8A},
{0x0D,0x03},
{0x0E,0xAF},
{0x0F,0x4C},
{0x10,0x48},
{0x11,0x89},
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
{0x80,0x13},
};

#define NUM_REG_CVBS_PAL_LMODE_PAGE0 (sizeof(reg_cvbs_pal_lmod_page0) / sizeof(reg_cvbs_pal_lmod_page0[0]))
#define NUM_REG_CVBS_PAL_LMODE_PAGE2 (sizeof(reg_cvbs_pal_lmod_page2) / sizeof(reg_cvbs_pal_lmod_page2[0]))
#define NUM_REG_PLAY_NTSC_FULL_PAGE0 (sizeof(reg_play_ntsc_full_page0) / sizeof(reg_play_ntsc_full_page0[0]))
#define NUM_REG_PLAY_NTSC_FULL_PAGE2 (sizeof(reg_play_ntsc_full_page2) / sizeof(reg_play_ntsc_full_page2[0]))
#define NUM_REG_PLAY_NTSC_LMOD_PAGE0 (sizeof(reg_play_ntsc_lmod_page0) / sizeof(reg_play_ntsc_lmod_page0[0]))
#define NUM_REG_PLAY_NTSC_LMOD_PAGE2 (sizeof(reg_play_ntsc_lmod_page2) / sizeof(reg_play_ntsc_lmod_page2[0]))
#define NUM_REG_PLAY_PAL_LMODE_PAGE0 (sizeof(reg_play_pal_lmod_page0) / sizeof(reg_play_pal_lmod_page0[0]))
#define NUM_REG_PLAY_PAL_LMODE_PAGE2 (sizeof(reg_play_pal_lmod_page2) / sizeof(reg_play_pal_lmod_page2[0]))
#endif
