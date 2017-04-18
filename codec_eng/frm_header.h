/* 
 *  @file   frm_header.h
 *  @brief  header file of frame
 *
 *  $Id: frm_header.h,v 1.8 2014/07/20 06:35:18 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/25  sherman  New file.
 *
 */

#ifndef _FMT_H_
#define _FMT_H_
//#include <enc_process.h>

#define JPEG_HDR_502    0
#define JPEG_HDR_EDDY   0

/* define yuv color sample */
/* byte order in memory: VYUY VYUY ... */
/*                        Y U Y V */
#define YUV_BLACK       0x00800080
#define YUV_WHITE       0xFF80FF80
#define YUV_RED         0x4C544CFF
#define YUV_GREEN       0x952B9515
#define YUV_BLUE        0x1DFF1D6B

#define YUV_YELLOW      0xE100E194
#define YUV_CYAN        0xB2ABB200
#define YUV_MAGENTA     0x69D469EA
#define YUV_PURPLE      0x34AA34B5
#define YUV_BLUEGREEN   0x59955940
#define YUV_DARKGRAY    0xC080C080
#define YUV_GRAY        0x80808080
#define YUV_LIGHTGRAY   0x4D804D80

#define YUV_DARKBLUE    0x34AA345A
#define YUV_DARKGREEN   0x52755245
#define YUV_SKYBLUE     0x8EBF8E75
#define YUV_LIGHTBLUE   0x12D01272
#define YUV_LIGHTPURPLE 0x87A0877A

/* Y */
#define Y_BLACK         0x00000000
#define Y_WHITE         0xFFFFFFFF
#define Y_RED           0x4C4C4C4C
#define Y_GREEN         0x95959595
#define Y_BLUE          0x1D1D1D1D
                                  
#define Y_YELLOW        0xE1E1E1E1
#define Y_CYAN          0xB2B2B2B2
#define Y_MAGENTA       0x69696969
#define Y_PURPLE        0x34343434
#define Y_BLUEGREEN     0x59595959
#define Y_DARKGRAY      0xC0C0C0C0
#define Y_GRAY          0x80808080
#define Y_LIGHTGRAY     0x4D4D4D4D
                                  
#define Y_DARKBLUE      0x34343434
#define Y_DARKGREEN     0x52525252
#define Y_SKYBLUE       0x8E8E8E8E
#define Y_LIGHTBLUE     0x12121212
#define Y_LIGHTPURPLE   0x87878787

/* U */
#define U_BLACK         0x80808080
#define U_WHITE         0x80808080
#define U_RED           0x54545454
#define U_GREEN         0x2B2B2B2B
#define U_BLUE          0xFFFFFFFF
                                  
#define U_YELLOW        0x00000000
#define U_CYAN          0xABABABAB
#define U_MAGENTA       0xD4D4D4D4
#define U_PURPLE        0xAAAAAAAA
#define U_BLUEGREEN     0x95959595
#define U_DARKGRAY      0x80808080
#define U_GRAY          0x80808080
#define U_LIGHTGRAY     0x80808080
                                  
#define U_DARKBLUE      0xAAAAAAAA
#define U_DARKGREEN     0x75757575
#define U_SKYBLUE       0xBFBFBFBF
#define U_LIGHTBLUE     0xD0D0D0D0
#define U_LIGHTPURPLE   0xA0A0A0A0

/* V */
#define V_BLACK         0x80808080
#define V_WHITE         0x80808080
#define V_RED           0xFFFFFFFF
#define V_GREEN         0x15151515
#define V_BLUE          0x6B6B6B6B
                                  
#define V_YELLOW        0x94949494
#define V_CYAN          0x00000000
#define V_MAGENTA       0xEAEAEAEA
#define V_PURPLE        0xB5B5B5B5
#define V_BLUEGREEN     0x40404040
#define V_DARKGRAY      0x80808080
#define V_GRAY          0x80808080
#define V_LIGHTGRAY     0x80808080
                                  
#define V_DARKBLUE      0x5A5A5A5A
#define V_DARKGREEN     0x45454545
#define V_SKYBLUE       0x75757575
#define V_LIGHTBLUE     0x72727272
#define V_LIGHTPURPLE   0x7A7A7A7A

#define SOI     0xd8ff
#define EOI     0xd9ff
#define APP0    0xe0ff
#define APP1    0xe1ff
#define DHT     0xc4ff
#define DQT     0xdbff
#define SOF0    0xc0ff
#define SOS     0xdaff

typedef enum {
    SYNC_START_BYTE,
    CHECK_MARKER,
    PARSE_EXIF,
    PARSE_APP,
    PARSE_TABLE_QUANT,
    PARSE_TABLE_HUFF,
    PARSE_RESTART_INTERVAL,
    PARSE_FRAME_HEADER,
    PARSE_SCAN_HEADER,
    PARSE_ECS,
    PARSE_EOI,
    PARSE_ONE_MARKER_SYNTAX,
    PARSE_UNKNOW
} JDECODE_STATE;

typedef enum {
    DEC_FIRST,
    DEC_NORMAL,
    DEC_LAST
} JPEG_DEC_SEG_OPT;

typedef enum {
    CS_YCbCr,
    CS_GRAYSCALE,
    CS_UNKNOW
} JPEG_COLOR_SPACE;

typedef enum {
    J_FORMAT_YUV420,
    J_FORMAT_YUV422,
    J_FORMAT_YUV422V, /* erect */
    J_FORMAT_YUV444,
    J_FORMAT_GRAY,
    J_FORMAT_UNKNOW
} JPEG_YUV_FORMAT;

struct q_table_t {
    uint8_t  qval[64];
};

#if 0
struct huffman_tbl_t {
    unsigned char bit[17];
    unsigned char val[256];
} HUFFMAN_TBL;
#endif

typedef struct {
    int id;
    int id_in_sof;
    int h_samp_factor;
    int v_samp_factor;
    int q_tbl_id;
    int dc_tbl_id;
    int ac_tbl_id;
} component_info;

typedef enum {
    FULL = 0,
    DIV_4,
    DIV_16,
    DIV_64
} DSCALE_OPT;


#pragma pack(push, 1)
struct jhdr_frame_t {
    uint16_t    marker;        /* SOF0 */
    uint16_t    sz;            /* length */
    uint8_t     smp_precision; /* number of lines */
    uint16_t    height;        /* number of lines */
    uint16_t    width;         /* number of samples per line */
    uint8_t     num_comp;      /* number of components */
    struct f_comp_t {
        uint8_t cid;           /* component id */
        uint8_t factor;        /* sample factor, [7:4] horizontal, [3:0] virtical */
        uint8_t tquant;        /* q table */
    } comp_id[3];
//};
} __attribute__((packed));

struct jhdr_scan_t {
    uint16_t    marker;        /* SOS */
    uint16_t    sz;            /* length */
    uint8_t     num_comp;      /* number of image components */
    struct s_comp_t {
        uint8_t cid;           /* component id */
        uint8_t thuff;         /* huffman table, [7:4] dc, [3:0] ac */
    } comp_id[3];
    uint8_t     start_spec;    /* start of spectral */
    uint8_t     end_spec;      /* end of spectral */
    uint8_t     approx;        /* approximation bit position */
} __attribute__((packed));


struct jhdr_huff_tbl_t {
    uint16_t    marker;        /* DHT */
    uint16_t    sz;            /* length */
    uint8_t     tid;           /* [7:4] table clase (0: dc, 1:ac), [3:0] destination id */
    uint8_t     num_bitlen[16];/* number of huffman code of length i */
} __attribute__((packed));

/* struct jhdr_app0_t{} is moved to sys/include/app0.h */
#include <app0.h>

struct jhdr_quant_tbl_t {
    uint16_t    marker;        /* DQT */
    uint16_t    sz;            /* length */
    uint8_t     qid;           /* q table , [7:4] element precision, [3:0] destination id */
    uint8_t     qval[64];      /* value of elements */
} __attribute__((packed));

struct jhdr_img_t {
    uint16_t    marker;        /* SOI */
    struct jhdr_app0_t app0;
    struct jhdr_quant_tbl_t q_tbl_y;
    struct jhdr_quant_tbl_t q_tbl_c;
    struct jhdr_frame_t frame;
    struct jhdr_scan_t  scan;

} __attribute__((packed));
#define JHDR_SIZE   (sizeof(struct jhdr_img_t))

struct jhdr_img_dht_t {
    uint16_t marker;            /* SOI */
    struct jhdr_app0_t app0;

    struct jhdr_huff_tbl_t huff_tbl_y_dc;
    uint8_t huff_tbl_y_dc_val[12];
    struct jhdr_huff_tbl_t huff_tbl_y_ac;
    uint8_t huff_tbl_y_ac_val[162];
    struct jhdr_huff_tbl_t huff_tbl_c_dc;
    uint8_t huff_tbl_c_dc_val[12];
    struct jhdr_huff_tbl_t huff_tbl_c_ac;
    uint8_t huff_tbl_c_ac_val[162];

    struct jhdr_quant_tbl_t q_tbl_y;
    struct jhdr_quant_tbl_t q_tbl_c;
    struct jhdr_frame_t frame;
    struct jhdr_scan_t scan;
} __attribute__ ((packed));
#define JHDR_DHT_SIZE   (sizeof(struct jhdr_img_dht_t))

/* no huffman table */
struct jhdr_img_lite_t {
    uint16_t    marker;        /* SOI */

    struct jhdr_app0_t app0;

    struct jhdr_quant_tbl_t q_tbl_y;
    struct jhdr_quant_tbl_t q_tbl_c;

    struct jhdr_frame_t frame;
    struct jhdr_scan_t  scan;

} __attribute__((packed));

struct video_frm_app0 {
    uint32_t time;
    uint8_t field_id;
    int32_t fno;
    uint32_t size;
} __attribute__((packed));
#pragma pack(pop)


#endif
