/**
 *  @file   jreg.h
 *  @brief  jpeg codec register definition header file
 *  $Id: jreg.h,v 1.1.1.1 2013/12/18 03:44:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/07  Jonathan New file.
 *
 */


#ifndef __JREG_H
#define __JREG_H


/* jpeg codec register */
#define JCODEC_CTRL_BASE_ADDR                   0xB8800000
#define JCODEC_CMD_REG                          (JCODEC_CTRL_BASE_ADDR + 0x0000)
#define JCODEC_STATUS_REG                       (JCODEC_CTRL_BASE_ADDR + 0x0004)
#define JCODEC_INT_MASK_REG                     (JCODEC_CTRL_BASE_ADDR + 0x0008)
#define JCODEC_INT_STATUS_REG                   (JCODEC_CTRL_BASE_ADDR + 0x000C)
#define JCODEC_IMG_INFO_0_REG                   (JCODEC_CTRL_BASE_ADDR + 0x0010)
#define JCODEC_IMG_INFO_1_REG                   (JCODEC_CTRL_BASE_ADDR + 0x0014)
#define JCODEC_DESP_CONF_REG                    (JCODEC_CTRL_BASE_ADDR + 0x0018)
#define JCODEC_MISC_REG                         (JCODEC_CTRL_BASE_ADDR + 0x001C)
#define JCODEC_BUF_0_HS_CTL_REG                 (JCODEC_CTRL_BASE_ADDR + 0x0020)
#define JCODEC_BUF_0_HS_USZ_REG                 (JCODEC_CTRL_BASE_ADDR + 0x0024)
#define JCODEC_BUF_0_ADDR_REG                   (JCODEC_CTRL_BASE_ADDR + 0x0028)
#define JCODEC_BD_OFFSET                        (0x10)
#define JCODEC_STATIS_REG                       (JCODEC_CTRL_BASE_ADDR + 0x00A0)
#define JCODEC_Y_QUNT_BASE_REG                  (JCODEC_CTRL_BASE_ADDR + 0x0100)
#define JCODEC_C_QUNT_BASE_REG                  (JCODEC_CTRL_BASE_ADDR + 0x0200)

/* pre rec register */
#define PREC_CTRL_BASE_ADDR                     0xBB800000
#define PREC_SYS_REG                            (PREC_CTRL_BASE_ADDR + 0x0000)
#define PREC_CONF_REG                           (PREC_CTRL_BASE_ADDR + 0x0004)
#define PREC_STATUS_REG                         (PREC_CTRL_BASE_ADDR + 0x0008)
#define PREC_INT_MASK_REG                       (PREC_CTRL_BASE_ADDR + 0x0010)
#define PREC_INT_STATUS_REG                     (PREC_CTRL_BASE_ADDR + 0x0014)
#define PREC_BUF_DESP_REG                       (PREC_CTRL_BASE_ADDR + 0x0020)
#define PREC_BUF_0_HS_REG                       (PREC_CTRL_BASE_ADDR + 0x0024)
#define PREC_BUF_1_HS_REG                       (PREC_CTRL_BASE_ADDR + 0x0028)
#define PREC_OSC_REG                            (PREC_CTRL_BASE_ADDR + 0x0100)
#define PREC_MFILTER_REG                        (PREC_CTRL_BASE_ADDR + 0x0200)

/* line buffer register */
#define LB_BUFFER_BASE_ADDR                     0xBC800000
#define LB_BUFFER_0_ADDR                        (LB_BUFFER_BASE_ADDR + 0x0000)
#define LB_BUFFER_1_ADDR                        (LB_BUFFER_BASE_ADDR + 0x4000)

#define LB_MAX_WIDTH_Y                          720
#define LB_MAX_WIDTH_C                          360
#define LB_LINES                                8
#define LB_Y_LINESTEP                           720
#define LB_CB_LINESTEP                          360
#define LB_CR_LINESTEP                          360
#define LB_Y_ADDR_OFFSET                        0
#define LB_CB_ADDR_OFFSET                       5760
#define LB_CR_ADDR_OFFSET                       8640
#define LB_CB_ADDR_OFFSET_DW                    1440 // (LB_CB_ADDR_OFFSET / 4)
#define LB_CR_ADDR_OFFSET_DW                    2160 // (LB_CR_ADDR_OFFSET / 4)

/* pre play register */
#define PPLAY_CTRL_BASE_ADDR                    0xBC000000
#define PPLAY_SYS_REG                           (PPLAY_CTRL_BASE_ADDR + 0x0000)
#define PPLAY_LB_CONF_REG                       (PPLAY_CTRL_BASE_ADDR + 0x0004)
#define PPLAY_FRM_SIZE_REG                      (PPLAY_CTRL_BASE_ADDR + 0x0008)
#define PPLAY_TIMING_REG                        (PPLAY_CTRL_BASE_ADDR + 0x000C)
#define PPLAY_DEF_COLOR_REG                     (PPLAY_CTRL_BASE_ADDR + 0x0010)
#define PPLAY_TOOLATE_CNT_REG                   (PPLAY_CTRL_BASE_ADDR + 0x001C)
#define PPLAY_LB_STAT_REG                       (PPLAY_CTRL_BASE_ADDR + 0x0020)

/* motion detection register */
#define MD_CTRL_BASE_ADDR                        0xBD400000
#define MD_SYS_REG                              (MD_CTRL_BASE_ADDR + 0x00)
#define MD_CONFIG_REG                           (MD_CTRL_BASE_ADDR + 0x04)
#define MD_STATUS_REG                           (MD_CTRL_BASE_ADDR + 0x08)
#define MD_INT_MASK_REG                         (MD_CTRL_BASE_ADDR + 0x10)
#define MD_INT_STATUS_REG                       (MD_CTRL_BASE_ADDR + 0x14)

#define MD_ENABLE                               1
#define MD_DISABLE                              0

#define FMT_Y                                   (0 << 18)
#define FMT_Cb                                  (1 << 18)
#define FMT_Cr                                  (2 << 18)
#define FMT_YYCbCr                              (3 << 18)

#define INT_FINISH                              1

/* jcodec command register bit */
#define JC_CMD_RST                              (1 << 2)
#define JC_CMD_ENC                              (1 << 1)
#define JC_CMD_DEC                              (1 << 0)
/* jcodec status register bit */
#define JC_ST_HW_DBG                            (0x3fffff << 10) /* [31:10] */
#define JC_ST_ENC_TB_EMPTY                      (1 << 9)
#define JC_ST_DEC_TB_EMPTY                      (1 << 8)
#define JC_ST_ENC_CB_EMPTY                      (1 << 7)
#define JC_ST_DEC_CB_EMPTY                      (1 << 6)
#define JC_ST_PIPE0_IDLE                        (1 << 5)
#define JC_ST_PIPE1_IDLE                        (1 << 4)
#define JC_ST_PIPE2_ENC_IDLE                    (1 << 3)
#define JC_ST_PIPE2_DEC_IDLE                    (1 << 2)
#define JC_ST_PIPE2_AHB_IDLE                    (1 << 1)
#define JC_ST_RST_ST                            (1 << 0)
/* jcodec interrupt register bit */
#define JC_INT_BS_OVERFLOW                      (1 << 1)
#define JC_INT_BD_COM                           (1 << 0)
/* jcodec image info register bit */
#define JC_IMG_INFO_H_REAL                      (0x3ff  << 20)/* [29:20] */
#define JC_IMG_INFO_H                           (0x3ff  << 10)/* [19:10] */
#define JC_IMG_INFO_W                           (0x3ff)       /* [ 9: 0] */
#define JC_IMG_MB_CNT                           (0x3fff)      /* [13: 0] */
/* jcodec descriptor configuration register bit */
#define JC_DCONF_DESP_SIZE                      (0xffff << 16)/* [31:16] */
#define JC_DCONF_DESP_CNT                       (0x3)         /* [ 1: 0] */
#define JC_DCONF_DESP_8                         (0x3)         /* 8 descriptors */
#define JC_DCONF_DESP_4                         (0x2)         /* 4 descriptors */
#define JC_DCONF_DESP_3                         (0x1)         /* 3 descriptors */
#define JC_DCONF_DESP_2                         (0x0)         /* 2 descriptors */
/* jcodec misc register bit */
#define JC_MISC_AHB_32B                         (1   << 4)
#define JC_MISC_LB_RPTR                         (0x3 << 2)    /* [ 3: 2] */
#define JC_MISC_LB_WPTR                         (0x3)         /* [ 1: 0] */
/* jcodec buffer descriptor handshake register bit */
#define JC_BDHS_IMG_START                       (1      << 31)
#define JC_BDHS_TS                              (0x7fff << 16)/* [30:16] */
#define JC_BDHS_IDX                             (0xff   << 8) /* [15: 8] */
#define JC_BDHS_IMG_IDX                         (0x3f   << 3) /* [ 7: 3] */
#define JC_BDHS_IMG_FIELD                       (0x1    << 2) /* [    2] */
#define JC_BDHS_IMG_END                         (1      << 1)
#define JC_BDHS_VALID                           (0x1)
#define JC_BDHS_USED_SIZE                       (0x1ffff)     /* [16: 0] */
#define JC_BDHS_OFFSET                          (0xffff << 16)/* [31:16] */
#define JC_BDHS_ADDR                            (0xffff)      /* [15: 0] */
/* jcodec statistic register bit */
#define JC_STATIS_ERR_CNT_1                     (0xff   << 8) /* [15: 8] */
#define JC_STATIS_ERR_CNT_2                     (0xff)        /* [ 7: 0] */
/* jcodec Y quantization value register bit */
#define JC_QUAN_VALUE                           (0xff)        /* [ 7: 0] */

/* prec system register bit */
#define PREC_SYS_RST                            (1 << 1)
#define PREC_SYS_ENA                            (0x1)
/* prec status register bit */
#define PREC_ST_BD_START                        (1      << 31)
#define PREC_ST_BD_TS                           (0x7fff << 16)/* [30:16] */
#define PREC_ST_WHOLE_ST                        (0x3    << 12)/* [13:12] */
#define PREC_ST_BMAN_ST                         (0x7    << 8) /* [10: 8] */
#define PREC_ST_BD_IDX                          (0x3f   << 2) /* [ 7: 2] */
#define PREC_ST_RST_ST                          (1      << 0)
/* prec interrupt register bit */
#define PREC_INT_OFIFO                          (1 << 4)
#define PREC_INT_IFIFO                          (1 << 3)
#define PREC_INT_IMG_COM                        (1 << 2)
#define PREC_INT_IMG_INCOM                      (1 << 1)
#define PREC_INT_BD_END                         (1 << 0)
/* prec buffer descriptor register bit */
#define PREC_LB_RPTR                            (0x3 << 2)
#define PREC_LB_WPTR                            (0x3 << 0)
/* prec buffer descriptor register bit */
#define PREC_BD_START                           (1      << 31)
#define PREC_BD_TS                              (0x7fff << 16)/* [30:16] */
#define PREC_BD_IDX                             (0x3f   << 2) /* [7:2] */

/* pre-play system register bit */
#define PPLAY_SYS_RST                           (0x1    << 7)
#define PPLAY_SYS_ENA                           (0x1    << 6)
#define PPLAY_SYS_GEN_TIMING_ENA                (0x1)
#define PPLAY_SYS_TM_MODE                       (0x3    << 8) /* [ 9: 8] */
#define PPLAY_SYS_DLY_MOD                       (0x1    << 10)
/* pre-play line buffer config register bit */
#define PPLAY_LB_CONF_DTO                       (0xffff)      /* [15: 0] */
#define PPLAY_LB_CONF_PREFILL                   (0xffff << 16)/* [31:16] */
/* pre-play frame size register bit */
#define PPLAY_FRM_H_SIZE                        (0x3ff)       /* [ 9: 0] */
#define PPLAY_FRM_V_SIZE                        (0x3ff  << 16)/* [25:16] */
/* pre-play back porch register bit */
#define PPLAY_TIMING_H_BP                       (0xff)        /* [ 7: 0] */
#define PPLAY_TIMING_V_BP                       (0x3f   << 8) /* [13: 8] */
/* pre-play delay count register bit */
#define PPLAY_TIMING_DLY_CNT                    (0xff   << 16)/* [ 7: 0] */
/* pre-play default color register bit */
#define PPLAY_DEF_COLOR_Y                       (0xff)        /* [ 7: 0] */
#define PPLAY_DEF_COLOR_U                       (0xff   << 8) /* [15: 8] */
#define PPLAY_DEF_COLOR_V                       (0xff   << 16)/* [24:16] */
/* pre-play frame start toolate count register bit */
#define PPLAY_TOOLATE_CNT                       (0xff)        /* [ 7: 0] */
/* pre-play line buffer status register bit */
#define PPLAY_LB_ST_1_FIELD                     (0x1    << 7)
#define PPLAY_LB_ST_1_FSTART                    (0x1    << 6)
#define PPLAY_LB_ST_0_FIELD                     (0x1    << 5)
#define PPLAY_LB_ST_0_FSTART                    (0x1    << 4)
#define PPLAY_LB_ST_RPTR                        (0x3    << 2) /* [ 3: 2] */
#define PPLAY_LB_ST_WPTR                        (0x3)         /* [ 1: 0] */


#endif /* __JREG_H */

