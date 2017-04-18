/**
 *  @file   jcommon.h
 *  @brief  jpeg codec common header file
 *  $Id: jcommon.h,v 1.7 2014/07/17 03:31:13 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/07  Jonathan New file.
 *
 */


#ifndef __JCOMMON_H
#define __JCOMMON_H


#include "jreg.h"


#define MAGIC_JJPEG     0xEE229955


/*********************
 * definition
**********************/
#define J_BUFSIZE                               4096
#define MAX_BD_NUM                              8
//#define BD_NUM                                  8
#define BUF_NUM                                 8//10
#define JCODEC_MAX_WIDTH                        720
#define JCODEC_MAX_HEIGHT                       480

#define FLASH_FAULT_COUNT                       90000


#define MAX_JPG_FILE_SIZE                       250*1024
#define MAX_AVI_FILE_SIZE                       25*1024*1024
//#define MAX_AVI_FILE_SIZE                       0x7fffffff // (2*1024*1024*1024 - 1)

/* JPEG CODEC setting */
#define JCODEC_BUF_HS_CTL_REG(IDX)  (JCODEC_BUF_0_HS_CTL_REG + JCODEC_BD_OFFSET * (IDX))
#define JCODEC_BUF_HS_USZ_REG(IDX)  (JCODEC_BUF_0_HS_USZ_REG + JCODEC_BD_OFFSET * (IDX))
#define JCODEC_BUF_ADDR_REG(IDX)    (JCODEC_BUF_0_ADDR_REG + JCODEC_BD_OFFSET * (IDX))

#define SET_JCODEC_BUF_ADDR_REG(IDX, OFFSET, BUF) writel(((OFFSET) << 16) | ((unsigned int)(BUF) & 0xffff), JCODEC_BUF_ADDR_REG (IDX))

#define SET_JCODEC_BUF_HS_CTL_REG(IDX, VAL) writel(VAL, JCODEC_BUF_HS_CTL_REG (IDX))
#define GET_JCODEC_BUF_HS_CTL_REG(IDX)      readl (JCODEC_BUF_HS_CTL_REG (IDX))

#define VALID4CPU   0
#define VALID4CODEC 1
#define DECODE_NOT_DONE(IDX)                (GET_JCODEC_BUF_HS_CTL_REG(IDX) & VALID4CODEC)

/*********************
 * ERROR code
**********************/
#define ERR_JC_CTRL_NOT_READY                   3801
#define ERR_JC_BUF_NOT_CONSIST                  3802
#define ERR_JC_RESOURCE_LIMIT                   3803
#define ERR_JC_NOT_VALID_FRAME                  3804

#define ERR_JC_SD_IO                            3901


int jcodec_hw_reset2(int dec_enc);
int print_jpeg_reg_value2(int wi, int ti);
int print_prec_reg_value2();

int set_quantization_table (int quality);
int set_jdecode (int img_w, int img_h, int des_size, int des_cnt);

#endif /* __JCOMMON_H */

