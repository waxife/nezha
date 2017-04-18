/**
 *  @file   audio.T582B.h
 *  @brief  header file of audio encode/decode driver
 *  $Id: audio.T582B.h,v 1.7 2014/02/25 03:15:04 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/01  Hugo     New file.
 *
 */

#ifndef __AUDIO_T582B_H_
#define __AUDIO_T582B_H_

#include "sdm.h"

/*****************************************************************************
 * Register Description
 *****************************************************************************/

#define AU_CTL_BASE                 0xBD000000
#define AU_CTL_DEC_SYSTEM           (AU_CTL_BASE + 0x00000000)
#define AU_CTL_ENC_SYSTEM           (AU_CTL_BASE + 0x00000080)
#define AU_CTL_CONFIG               (AU_CTL_BASE + 0x00000004)
#define AU_CTL_STATUS0              (AU_CTL_BASE + 0x00000008)
#define AU_CTL_STATUS1              (AU_CTL_BASE + 0x0000000C)
#define AU_CTL_INT_MASK             (AU_CTL_BASE + 0x00000010)
#define AU_CTL_INT_STATUS           (AU_CTL_BASE + 0x00000014)
#define AU_CTL_STATUS2              (AU_CTL_BASE + 0x00000018)
#define AU_CTL_DEC_BD0_CONF         (AU_CTL_BASE + 0x00000020)
#define AU_CTL_DEC_BD1_CONF         (AU_CTL_BASE + 0x00000024)
#define AU_CTL_DEC_BD2_CONF         (AU_CTL_BASE + 0x00000028)
#define AU_CTL_DEC_BD3_CONF         (AU_CTL_BASE + 0x0000002C)
#define AU_CTL_ENC_BD0_CONF         (AU_CTL_BASE + 0x00000030)
#define AU_CTL_ENC_BD1_CONF         (AU_CTL_BASE + 0x00000034)
#define AU_CTL_ENC_BD2_CONF         (AU_CTL_BASE + 0x00000038)
#define AU_CTL_ENC_BD3_CONF         (AU_CTL_BASE + 0x0000003C)
#define AU_CTL_DEC_BD0_HS           (AU_CTL_BASE + 0x00000040)
#define AU_CTL_DEC_BD0_ADDR         (AU_CTL_BASE + 0x00000044)
#define AU_CTL_ENC_BD0_HS           (AU_CTL_BASE + 0x00000048)
#define AU_CTL_ENC_BD0_ADDR         (AU_CTL_BASE + 0x0000004C)
#define AU_CTL_DEC_BD1_HS           (AU_CTL_BASE + 0x00000050)
#define AU_CTL_DEC_BD1_ADDR         (AU_CTL_BASE + 0x00000054)
#define AU_CTL_ENC_BD1_HS           (AU_CTL_BASE + 0x00000058)
#define AU_CTL_ENC_BD1_ADDR         (AU_CTL_BASE + 0x0000005C)
#define AU_CTL_DEC_BD2_HS           (AU_CTL_BASE + 0x00000060)
#define AU_CTL_DEC_BD2_ADDR         (AU_CTL_BASE + 0x00000064)
#define AU_CTL_ENC_BD2_HS           (AU_CTL_BASE + 0x00000068)
#define AU_CTL_ENC_BD2_ADDR         (AU_CTL_BASE + 0x0000006C)
#define AU_CTL_DEC_BD3_HS           (AU_CTL_BASE + 0x00000070)
#define AU_CTL_DEC_BD3_ADDR         (AU_CTL_BASE + 0x00000074)
#define AU_CTL_ENC_BD3_HS           (AU_CTL_BASE + 0x00000078)
#define AU_CTL_ENC_BD3_ADDR         (AU_CTL_BASE + 0x0000007C)
#define AU_CTL_SGMDLT_CONF          (AU_CTL_BASE + 0x000000A0)
#define AU_CTL_SGMDLT_ADC_STATUS    (AU_CTL_BASE + 0x000000A4)

#define AU_ENC_BD_HS_BASE           AU_CTL_ENC_BD0_HS
#define AU_ENC_BD_ADDR_BASE         AU_CTL_ENC_BD0_ADDR
#define AU_BD_OFFSET                0x10

/* decode descriptor registers */
#define AU_DEC_BD_CONF(N)           (AU_CTL_DEC_BD0_CONF + (N)*4)
#define AU_DEC_BD_HS(N)             (AU_CTL_DEC_BD0_HS   + (N)*0x10)
#define AU_DEC_BD_ADDR(N)           (AU_CTL_DEC_BD0_ADDR + (N)*0x10)

/* encode descriptor registers */
#define AU_ENC_BD_CONF(N)           (AU_CTL_ENC_BD0_CONF + (N)*4)
#define AU_ENC_BD_HS(N)             (AU_CTL_ENC_BD0_HS   + (N)*0x10)
#define AU_ENC_BD_ADDR(N)           (AU_CTL_ENC_BD0_ADDR + (N)*0x10)


/*****************************************************************************
 * Register Setting
 *****************************************************************************/

/* AU_CTL_DEC_SYSTEM */
#define DEC_RESUME                  (1 << 4)
#define DEC_PAUSE                   (1 << 3)
#define DEC_EN                      (1 << 2)
#define DEC_RESET                   (1 << 0)

#define DEC_RESUME_OFFSET           4
#define DEC_PAUSE_OFFSET            3
#define DEC_EN_OFFSET               2
#define DEC_RESET_OFFSET            0

/* AU_CTL_ENC_SYSTEM */
#define ENC_EN                      (1 << 1)
#define ENC_RESET                   (1 << 0)

#define ENC_EN_OFFSET               1
#define ENC_RESET_OFFSET            0

/* AU_CTL_CONFIG */
#define ENC_FORMAT(V)               (((V) & 0x7) << 19)
#define DEC_FORMAT(V)               (((V) & 0x7) << 16)
#define ENC_BUILDIN(V)              (((V) & 0x1) << 15)
#define ENC_BUILDIN_MODE(V)         (((V) & 0x7) << 12)
#define DEC_BUILDIN(V)              (((V) & 0x1) << 11)
#define DEC_BUILDIN_MODE(V)         (((V) & 0x7) << 8)
#define WS_POLAR(V)                 (((V) & 0x1) << 6)
#define BCLK_POLAR(V)               (((V) & 0x1) << 5)
#define ACLK_POLAR(V)               (((V) & 0x1) << 4)
#define DATA_MODE(V)                (((V) & 0x1) << 3)
#define LS_STUFF(V)                 (((V) & 0x1) << 2)
#define IIS_CSEL(V)                 (((V) & 0x1) << 1)

#define FORMAT_ENC_OFFSET           19
#define FORMAT_ENC_BITS             3
#define FORMAT_DEC_OFFSET           16
#define FORMAT_DEC_BITS             3
#define BUILDIN_ENC_OFFSET          12
#define BUILDIN_ENC_BITS            4
#define BUILDIN_DEC_OFFSET          8
#define BUILDIN_DEC_BITS            4

#define BUILDIN_ENC_ENABLE_OFFSET   15
#define BUILDIN_ENC_ENABLE_BITS     1
#define BUILDIN_ENC_MODE_OFFSET     12
#define BUILDIN_ENC_MODE_BITS       3
#define BUILDIN_DEC_ENABLE_OFFSET   11
#define BUILDIN_DEC_ENABLE_BITS     1
#define BUILDIN_DEC_MODE_OFFSET     8
#define BUILDIN_DEC_MODE_BITS       3


#define ENC_FORMAT_MASK             ENC_FORMAT(-1)
#define ENC_G726_16                 ENC_FORMAT(0)
#define ENC_PCM                     ENC_FORMAT(1)
#define ENC_U_LAW                   ENC_FORMAT(2)
#define ENC_A_LAW                   ENC_FORMAT(3)
#define ENC_G726_32                 ENC_FORMAT(4)

#define DEC_FORMAT_MASK             DEC_FORMAT(-1)
#define DEC_G726_16                 DEC_FORMAT(0)
#define DEC_PCM                     DEC_FORMAT(1)
#define DEC_U_LAW                   DEC_FORMAT(2)
#define DEC_A_LAW                   DEC_FORMAT(3)
#define DEC_G726_32                 DEC_FORMAT(4)

#define ENC_BUILDIN_MASK            (ENC_BUILDIN(-1) | ENC_BUILDIN_MODE(-1))
#define ENC_BUILDIN_DISABLE         ENC_BUILDIN(0)
#define ENC_BUILDIN_ENABLE          ENC_BUILDIN(1)
#define ENC_BUILDIN_256SIN          ENC_BUILDIN_MODE(0)
#define ENC_BUILDIN_1KSIN           ENC_BUILDIN_MODE(1)
#define ENC_BUILDIN_0X8000          ENC_BUILDIN_MODE(2)
#define ENC_BUILDIN_0X7FFF          ENC_BUILDIN_MODE(3)
#define ENC_BUILDIN_0X8000_0X7FFF   ENC_BUILDIN_MODE(4)
#define ENC_BUILDIN_0X0000          ENC_BUILDIN_MODE(5)

#define DEC_BUILDIN_MASK            (DEC_BUILDIN(-1) | DEC_BUILDIN_MODE(-1))
#define DEC_BUILDIN_DISABLE         DEC_BUILDIN(0)
#define DEC_BUILDIN_ENABLE          DEC_BUILDIN(1)
#define DEC_BUILDIN_256SIN          DEC_BUILDIN_MODE(0)
#define DEC_BUILDIN_1KSIN           DEC_BUILDIN_MODE(1)
#define DEC_BUILDIN_0X8000          DEC_BUILDIN_MODE(2)
#define DEC_BUILDIN_0X7FFF          DEC_BUILDIN_MODE(3)
#define DEC_BUILDIN_0X8000_0X7FFF   DEC_BUILDIN_MODE(4)
#define DEC_BUILDIN_0X0000          DEC_BUILDIN_MODE(5)

#define BUILDIN_DISABLE             (0 << 3)
#define BUILDIN_ENABLE              (1 << 3)
#define BUILDIN_256SIN              (BUILDIN_ENABLE | 0)
#define BUILDIN_1KSIN               (BUILDIN_ENABLE | 1)
#define BUILDIN_0x8000              (BUILDIN_ENABLE | 2)
#define BUILDIN_0x7FFF              (BUILDIN_ENABLE | 3)
#define BUILDIN_0x8000_0x7FFF       (BUILDIN_ENABLE | 4)
#define BUILDIN_0x0000              (BUILDIN_ENABLE | 5)

#define WS_POLAR_FALLING            WS_POLAR(0)
#define WS_POLAR_RISING             WS_POLAR(1)
#define BCLK_POLAR_STANDARD         BCLK_POLAR(0)
#define BCLK_POLAR_INVERSE          BCLK_POLAR(1)
#define ACLK_POLAR_NORMAL           ACLK_POLAR(0)
#define ACLK_POLAR_INVERSE          ACLK_POLAR(1)
#define DATA_MODE_STANDARD          DATA_MODE(0)
#define DATA_MODE_LEFT              DATA_MODE(1)
#define LB_STUFF_ZERO               LS_STUFF(0)
#define LB_STUFF_RANDOM             LS_STUFF(1)
#define IIS_CSEL_L                  IIS_CSEL(0)
#define IIS_CSEL_R                  IIS_CSEL(1)

/* AU_CTL_STATUS0 */
#define AU_ENC_STATE(V)             (((V) >> 9) & 0x7)
#define AU_DEC_STATE(V)             (((V) >> 6) & 0x7)
#define AU_ENC_BDPTR(V)             (((V) >> 4) & 0x3)
#define AU_DEC_BDPTR(V)             (((V) >> 2) & 0x3)
#define AU_DEC_ISPAUSE(V)           (((V) >> 1) & 0x1)
#define AU_RESET_BUSY(V)            (((V) >> 0) & 0x1)

#define ST0_DEC_PAUSE_OFFSET        1
#define RSTATUS_OFFSET              0

#define AU_RSTATUS_READY            0
#define AU_RSTATUS_BUSY             1

/* AU_CTL_STATUS1 */
#define AU_ENC_BD_TS(V)             (((V) >> 16) & 0x7fff)
#define AU_ENC_BD_INDEX(V)          (((V) >> 8) & 0xff)
#define AU_ENC_FIFO_CNT(V)          (((V) >> 4) & 0x7)
#define AU_DEC_FIFO_CNT(V)          (((V) >> 0) & 0x7)

/* AU_CTL_STATUS2 */
#define AU_ENC_PCM_CNT(V)           (((V) >> 24) & 0xff)
#define AU_DEC_PCM_CNT(V)           (((V) >> 16) & 0xff)
#define AU_ENC_IIS_CNT(V)           (((V) >> 8) & 0xff)
#define AU_DEC_IIS_CNT(V)           (((V) >> 0) & 0xff)

/* AU_CTL_INT */
#define INT_ENC_BDES                (1 << 7)
#define INT_ENC_BDES_UF             (1 << 6)
#define INT_ENC_BDES_OF             (1 << 5)
#define INT_DEC_BDES                (1 << 4)
#define INT_DEC_BDES_UF             (1 << 3)
#define INT_DEC_BDES_OF             (1 << 2)
#define INT_FIFO_UF                 (1 << 1)
#define INT_FIFO_OF                 (1 << 0)

/* AU_CTL_SGMDLT_CONF */
#define SDM_ENC_GAIN(V)             (((V) & 0xf) << 20)
#define SDM_ENC_FREQ(V)             (((V) & 0x1) << 18)
#define SDM_ENC_BOOST(V)            (((V) & 0x1) << 17)
#define SDM_ENC_PWR(V)              (((V) & 0x1) << 16)
#define SDM_DEC_GAIN(V)             (((V) & 0xf) << 8)
#define SDM_DEC_PWR_VCM(V)          (((V) & 0xf) << 5)
#define SDM_DEC_PWR(V)              (((V) & 0xf) << 4)
#define SDM_DEC_PDN_TEST3(V)        (((V) & 0xf) << 3)
#define SDM_DEC_PDN_TEST2(V)        (((V) & 0xf) << 2)
#define SDM_DEC_PDN_TEST1(V)        (((V) & 0xf) << 1)
#define SDM_DEC_PDN_TEST0(V)        (((V) & 0xf) << 0)
#define SDM_DEC_PDN_TESTs           (0xf << 0)

#define SDM_ENC_GAIN_2_75X          SDM_ENC_GAIN(0xf)
#define SDM_ENC_GAIN_2_52X          SDM_ENC_GAIN(0xe)
#define SDM_ENC_GAIN_2_30X          SDM_ENC_GAIN(0xd)
#define SDM_ENC_GAIN_1_96X          SDM_ENC_GAIN(0xc)
#define SDM_ENC_GAIN_1_68X          SDM_ENC_GAIN(0xb)
#define SDM_ENC_GAIN_1_44X          SDM_ENC_GAIN(0xa)
#define SDM_ENC_GAIN_1_25X          SDM_ENC_GAIN(0x9)
#define SDM_ENC_GAIN_1_00X          SDM_ENC_GAIN(0x8)
#define SDM_ENC_GAIN_0_82X          SDM_ENC_GAIN(0x7)
#define SDM_ENC_GAIN_0_70X          SDM_ENC_GAIN(0x6)
#define SDM_ENC_GAIN_0_61X          SDM_ENC_GAIN(0x5)
#define SDM_ENC_GAIN_0_52X          SDM_ENC_GAIN(0x4)
#define SDM_ENC_GAIN_0_44X          SDM_ENC_GAIN(0x3)
#define SDM_ENC_GAIN_0_37X          SDM_ENC_GAIN(0x2)
#define SDM_ENC_GAIN_0_31X          SDM_ENC_GAIN(0x1)
#define SDM_ENC_GAIN_0_25X          SDM_ENC_GAIN(0x0)

#define SDM_DEC_GAIN_2_75X          SDM_DEC_GAIN(0xf)
#define SDM_DEC_GAIN_2_52X          SDM_DEC_GAIN(0xe)
#define SDM_DEC_GAIN_2_30X          SDM_DEC_GAIN(0xd)
#define SDM_DEC_GAIN_1_96X          SDM_DEC_GAIN(0xc)
#define SDM_DEC_GAIN_1_68X          SDM_DEC_GAIN(0xb)
#define SDM_DEC_GAIN_1_44X          SDM_DEC_GAIN(0xa)
#define SDM_DEC_GAIN_1_25X          SDM_DEC_GAIN(0x9)
#define SDM_DEC_GAIN_1_00X          SDM_DEC_GAIN(0x8)
#define SDM_DEC_GAIN_0_82X          SDM_DEC_GAIN(0x7)
#define SDM_DEC_GAIN_0_70X          SDM_DEC_GAIN(0x6)
#define SDM_DEC_GAIN_0_61X          SDM_DEC_GAIN(0x5)
#define SDM_DEC_GAIN_0_52X          SDM_DEC_GAIN(0x4)
#define SDM_DEC_GAIN_0_44X          SDM_DEC_GAIN(0x3)
#define SDM_DEC_GAIN_0_37X          SDM_DEC_GAIN(0x2)
#define SDM_DEC_GAIN_0_31X          SDM_DEC_GAIN(0x1)
#define SDM_DEC_GAIN_0_25X          SDM_DEC_GAIN(0x0)

#define SDM_ENC_FREQ_NORMAL         SDM_ENC_FREQ(0)
#define SDM_ENC_FREQ_DOUBLE         SDM_ENC_FREQ(1)
#define SDM_ENC_BOOST_NORMAL        SDM_ENC_BOOST(0)
#define SDM_ENC_BOOST_4X            SDM_ENC_BOOST(1)
#define SDM_ENC_PWR_OFF             SDM_ENC_PWR(0)
#define SDM_ENC_PWR_ON              SDM_ENC_PWR(1)
#define SDM_DEC_PWR_OFF             SDM_DEC_PWR(0)
#define SDM_DEC_PWR_ON              SDM_DEC_PWR(1)

#define SDM_ENC_GAIN_OFFSET         20
#define SDM_ENC_GAIN_BITS           4
#define SDM_DEC_GAIN_OFFSET         8
#define SDM_DEC_GAIN_BITS           4
#define SDM_ENC_PWR_OFFSET          16
#define SDM_DEC_PWR_OFFSET          4
#define SDM_DEC_PDN_TEST_OFFSET     0
#define SDM_DEC_PDN_TEST_BITS       4

#define DES_SIZE_1024                       (0x3ff << 16)
#define DES_CNT_4                           (3 << 0)

#define DESC_SIZE_1024                      1024
#define DESC_SIZE_2048                      2048


struct bd_t {
    unsigned int addr[4];
    unsigned int offset[4];
    int size;
    unsigned char num;
    unsigned char w_ptr;
    unsigned char r_ptr;
};

struct au_io {
    unsigned int buffer;
    unsigned int length;
    unsigned int offset;
    int bd_head;
    int bd_tail;
};

struct au_cfg {
    int buf_len;
    int des_num;
    int format;
};

struct au_ctx_t {
    unsigned int cfg_dmode;
    unsigned int cmd_mode;          /* [1]: enc, [0]: dec */
    unsigned int cfg_int_mask;
    unsigned int ts;
    unsigned int desc_idx;

    struct bd_t dec_bd[1];
    struct bd_t enc_bd[1];

    unsigned int dec_format;
    unsigned int enc_format;

    unsigned int cfg_iis_csel;
    unsigned int cfg_buildm;        /* build-in test pattern, [15:8]: enc, [7:0]: dec */
    unsigned int cfg_build;         /* build-in test enc, [1]: enc, [0]: dec */

    unsigned int pause;

    int nor_pa_start;
    int nor_pa_cur;
    int nor_size;
    int nor_dec_start;

    /* configurable parameters */
    struct au_cfg cfg;

    /* variables for experimental API */
    struct au_io rd;
    struct au_io wr;
};

#define AUDIO_DRV_STATISTIC 0
#if AUDIO_DRV_STATISTIC
struct au_statistic {
    unsigned int fifo_overflow_int;
    unsigned int enc_bdes_int;
};
extern struct au_statistic au_st[1];
#endif /* AUDIO_DRV_STATISTIC */

/* --- AUDIO control config --- */
/* cmd_mode */
#define AU_CMD_DEC          0x1
#define AU_CMD_ENC          0x2
#define AU_CMD_FULL_DUPLEX  0x3

/* enc_format, dec_format */
enum{
    G726_16 = 0,
    PCM,
    U_LAW,
    A_LAW,
    G726_32,
    ACODEC_UNKNOWN
};

/* aud_cfg_iis_csel */
#define L_CHANEL            0
#define R_CHANEL            1

/* aud_cfg_buildm */
enum{
    RATE_8K_256 = 0,
    RATE_8K_1K,
    ALL_8000,
    ALL_7FFF,
    INTERLEAVE,
    ALL_0000,
};

/* cfg_ws_polar */
#define FALLING             0
#define RISING              1

/* cfg_bclk_polar */
#define STD_POL             0
#define INV_POL             1

/* cfg_aclk_polar */
#define INTERNAL_ACLK       0
#define INVERSE_ACLK        1

/* cfg_dmode */
#define STD_IIS_DMODE       0
#define LEFT_ADJ_DMODE      1

/* cfg_lb_stuff */
#define ZERO                0
#define RANDOM              1

/* cfg_build */
#define NORMAL_DATA         0
#define TEST_WAVE           1

#endif /* __AUDIO2_H_ */
