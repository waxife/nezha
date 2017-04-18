/**
 *  @file   enc_common.h
 *  @brief  header file for encode process
 *  $Id: enc_common.h,v 1.6 2014/07/18 03:05:35 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2011/06/09  New file.
 *
 */
#ifndef __ENC_COMMON_H
#define __ENC_COMMON_H
#include <fat32.h>


/* bit definition for buffer descriptor registers */
#define BIT_VBIT                (1 << 0)
#define VBIT_R                  (1 << 0)
#define IMG_END_R               (1 << 1)
#define IMG_FIELD_R(x)          (((x) >> 2) & 0x1)
#define IMG_IDX_R(x)            (((x) >> 3) & 0x1f)
#define BD_IDX_R(x)             (((x) >> 8) & 0xff)
#define TS_R(x)                 (((x) >> 16) & 0x7f)
#define IMG_START_R             (1 << 31)

/* buffer size definition */
#define MD_SIZE                 4800
#define ATOM_SIZE               4096
#define A_ATOM_SIZE             256
#define HDR_SIZE                1024

#define HEADER_LEN              1024
#define BD_NUM                  8           /* number of buffer descriptor */
#define BD_NUM_MASK             0x7
#define A_BD_NUM                4
#define A_BD_NUM_MASK           0x3



enum ATOM_TYPE {
    AVI_HEADER = 0,
    VFRM_HEADER,
    AFRM_HEADER,
    V_DATA,
    A_DATA,
};

enum {
    E_FREE_HDR                  = 201,
    E_TRANSFER_FLL              = 202,
    E_PUT_DONE                  = 203,
    E_UPD_FRM_SIZE              = 204,
    E_GET_DONE                  = 205,
    E_PREPARE_HEADER            = 206,
    E_FLUSH_FLL                 = 207,
    E_GET_FREE                  = 208,      /* failed to get free atom */
    E_PUT_FREE                  = 209,      /* failed to put free atom to enc shut */
    E_GET_FREE_ATOM             = 210,
    E_VBIT_TIMEOUT              = 211,      /* can not get vaild vbit */
    E_BD_BUFFER                 = 212,      /* BD buffer is not enough */
    E_MEM_ALLOC                 = 213,      /* memory doesn't allocate on BD */
};


#define ST_NONE     0
#define ST_INIT     1
#define ST_FRAME    2
#define ST_DROP     3


typedef struct {
    void            *next;
    uint32_t        pins:3;
    uint32_t        offset:14;
    uint32_t        type:3;
    uint32_t        fill_size:14;
} list_t;

struct atom_t {
    list_t          l;
#if CONFIG_RESERVE_CODEC_BUFFER
    uint8_t         *buf;
#else
    uint8_t         buf[ATOM_SIZE];
#endif
};

struct head_t {
    list_t          l;
    uint8_t         buf[HDR_SIZE];
};

struct chunk_t;

#define ONE_FIELD 0
#define TWO_FIELD 1

#define ONE_FIELD_MAGIC 0x63643030
#define TWO_FIELD_MAGIC 0x63645050

/* struct app0_info{} is moved to sys/include/app0.h */
#include <app0.h>

#endif /* __ENC_COMMON_H */
