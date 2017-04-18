/**
 *  @file   upgrade_sd.c
 *  @brief  sd upgrade program
 *  $Id: upgrade_sd.c,v 1.2 2014/02/24 09:43:18 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date 
 */

#include "upgrade.h"

/*
 * SD header
 */
#define T300_SDC_BASE_T      			0xba800000
#define SDC_CMD_REG						(T300_SDC_BASE_T + 0x0000)
#define SDC_ARGU_REG					(T300_SDC_BASE_T + 0x0004)
#define SDC_RESPONSE0_REG				(T300_SDC_BASE_T + 0x0008)
#define SDC_RESPONSE1_REG				(T300_SDC_BASE_T + 0x000C)
#define SDC_RESPONSE2_REG				(T300_SDC_BASE_T + 0x0010)
#define SDC_RESPONSE3_REG				(T300_SDC_BASE_T + 0x0014)
#define SDC_RSP_CMD_REG					(T300_SDC_BASE_T + 0x0018)
#define SDC_DATA_CTRL_REG				(T300_SDC_BASE_T + 0x001C)
#define SDC_DATA_TIMER_REG			    (T300_SDC_BASE_T + 0x0020)
#define SDC_DATA_LEN_REG				(T300_SDC_BASE_T + 0x0024)
#define SDC_STATUS_REG					(T300_SDC_BASE_T + 0x0028)
#define SDC_CLEAR_REG					(T300_SDC_BASE_T + 0x002C)
#define SDC_INT_MASK_REG				(T300_SDC_BASE_T + 0x0030)
#define SDC_POWER_CTRL_REG			    (T300_SDC_BASE_T + 0x0034)
#define SDC_CLOCK_CTRL_REG			    (T300_SDC_BASE_T + 0x0038)
#define SDC_BUS_WIDTH_REG				(T300_SDC_BASE_T + 0x003C)
#define SDC_DATA_WINDOW_REG			    (T300_SDC_BASE_T + 0x0040)
#define SDC_FEATURE_REG                 (T300_SDC_BASE_T + 0x0044)

/*
 * DMA header
 */
#define DMA_BASE                        0xb9800000
#define DMA_INT                         (DMA_BASE + 0x00)
#define DMA_INT_TC                      (DMA_BASE + 0x04)
#define DMA_INT_TC_CLR                  (DMA_BASE + 0x08)
#define DMA_INT_ERRABT                  (DMA_BASE + 0x0c)
#define DMA_ERRABT_CLR                  (DMA_BASE + 0x10)
#define DMA_TC                          (DMA_BASE + 0x14)
#define DMA_ERRABT                      (DMA_BASE + 0x18)
#define DMA_EN                          (DMA_BASE + 0x1c)
#define DMA_BUSY                        (DMA_BASE + 0x20)
#define DMA_CSR                         (DMA_BASE + 0x24)
#define DMA_SYNC                        (DMA_BASE + 0x28)
#define DMA_CH_CSR(ch)                  (DMA_BASE + 0x100 + (0x20 * (ch)))
#define DMA_CH_CFG(ch)                  (DMA_BASE + 0x104 + (0x20 * (ch)))
#define DMA_CH_SrcAddr(ch)              (DMA_BASE + 0x108 + (0x20 * (ch)))
#define DMA_CH_DstAddr(ch)              (DMA_BASE + 0x10c + (0x20 * (ch)))
#define DMA_CH_LLP(ch)                  (DMA_BASE + 0x110 + (0x20 * (ch)))
#define DMA_CH_SIZE(ch)                 (DMA_BASE + 0x114 + (0x20 * (ch)))

/* DMA Channel */
#define DMA_CF              0
#define DMA_SD              1
#define DMA_MS              2
#define DMA_XD              3
#define DMA_NOR             4
#define DMA_NAND            5
#define DMA_IIS             6

/* DMA SD Channel */
#define DMA_SD_CFG          DMA_CH_CFG(DMA_SD)
#define DMA_SD_SrcAddr      DMA_CH_SrcAddr(DMA_SD)
#define DMA_SD_DstAddr      DMA_CH_DstAddr(DMA_SD)
#define DMA_SD_SIZE         DMA_CH_SIZE(DMA_SD)
#define DMA_SD_LLP          DMA_CH_LLP(DMA_SD)
#define DMA_SD_CSR          DMA_CH_CSR(DMA_SD)

/* DMA_CH_CFG */
#define SRC_HE              (1 << 7)
#define DST_HE              (1 << 13)
#define SRC_CH(ch)          ((ch) << 3)
#define DST_CH(ch)          ((ch) << 9)

/* bit mapping of channel control register */
#define CSR_CH_EN       1
#define CSR_READ        0x0
#define CSR_WRITE       0x0

#define CSR_DST_INC     (0 << 3)
#define CSR_DST_DEC     (1 << 3)
#define CSR_DST_FIX     (2 << 3)

#define CSR_SRC_INC     (0 << 5)
#define CSR_SRC_DEC     (1 << 5)
#define CSR_SRC_FIX     (2 << 5)

#define CSR_HWH         (1 << 7)    /* hardware handshake mode */

#define CSR_DST_W8      (0 << 8)
#define CSR_DST_W16     (1 << 8)
#define CSR_DST_W32     (2 << 8)

#define CSR_SRC_W8      (0 << 11)
#define CSR_SRC_W16     (1 << 11)
#define CSR_SRC_W32     (2 << 11)

#define CSR_ABORT       (1 << 15)

#define CSR_BURST_1     (0 << 16)
#define CSR_BURST_4     (1 << 16)
#define CSR_BURST_8     (2 << 16)
#define CSR_BURST_16    (3 << 16)
#define CSR_BURST_32    (4 << 16)
#define CSR_BURST_64    (5 << 16)
#define CSR_BURST_128   (6 << 16)
#define CSR_BURST_256   (7 << 16)

#define LLPC_SRC_W8     (0 << 25)
#define LLPC_SRC_W16    (1 << 25)
#define LLPC_SRC_W32    (2 << 25)

#define LLPC_DST_W8     (0 << 22)
#define LLPC_DST_W16    (1 << 22)
#define LLPC_DST_W32    (2 << 22)

#define LLPC_SRC_INC    (0 << 20)
#define LLPC_SRC_DEC    (1 << 20)
#define LLPC_SRC_FIX    (2 << 20)

#define LLPC_DST_INC    (0 << 18)
#define LLPC_DST_DEC    (1 << 18)
#define LLPC_DST_FIX    (2 << 18)

#define LLPC_READ       (0 << 16)
#define LLPC_WRITE      (0 << 16)

/* command register */
#define CMD_SDC_RST     (1 << 10)
#define CMD_CMD_EN      (1 << 9)
#define CMD_APP_CMD     (1 << 8)
#define CMD_LONG_RSP    (1 << 7)
#define CMD_NEED_RSP    (1 << 6)
#define CMD_CMD_MASK    (0x3f)
#define SDC_DATA_READ           (BLK_SIZE_512 | DATA_READ | DATA_DMA | DATA_EN)

#define CMD(cmd)        ((cmd) | CMD_CMD_EN)
#define ACMD(cmd)       ((cmd) | CMD_APP_CMD | CMD_CMD_EN)

/* status register */
#define STATUS_WRITE_PROT       (1 << 12)
#define STATUS_CARD_DETECT      (1 << 11)
#define STATUS_CARD_CHANGE      (1 << 10)
#define STATUS_FIFO_ORUN        (1 << 9)
#define STATUS_FIFO_URUN        (1 << 8)
#define STATUS_DATA_END         (1 << 7)
#define STATUS_CMD_SENT         (1 << 6)
#define STATUS_DATA_CRC_OK      (1 << 5)
#define STATUS_RSP_CRC_OK       (1 << 4)
#define STATUS_DATA_TIMEOUT     (1 << 3)
#define STATUS_RSP_TIMEOUT      (1 << 2)
#define STATUS_DATA_CRC_FAIL    (1 << 1)
#define STATUS_RSP_CRC_FAIL     (1 << 0)

/* data register */
#define DATA_EN                 (1 << 6)
#define DATA_DMA                (1 << 5)
#define DATA_WRITE              (1 << 4)
#define DATA_READ               0

#define BLK_SIZE_8              3
#define BLK_SIZE_256            8
#define BLK_SIZE_512            9
#define BLK_SIZE_1024           10
#define BLK_SIZE_2048           11

/* SD card state */
#define SD_ST_IDLE  0
#define SD_ST_READY 1
#define SD_ST_IDENT 2
#define SD_ST_STBY  3
#define SD_ST_TRAN  4
#define SD_ST_DATA  5
#define SD_ST_RCV   6
#define SD_ST_PRG   7
#define SD_ST_DIS   8

#define DMA_SD_INT          (0x00000001 << DMA_SD)
#define DMA_SD_ERR          (0x00000001 << DMA_SD)
#define DMA_SD_ABT          (0x00010000 << DMA_SD)

#define LL2C_DMAFF_TH(ll)   ((((ll) >> 29) & 0x7) << 24)
#define LL2C_TCMASK(ll)     ((((ll) >> 28) & 0x1) << 31)
#define LL2C_SRC_WIDTH(ll)  ((((ll) >> 25) & 0x7) << 11)
#define LL2C_DST_WIDTH(ll)  ((((ll) >> 22) & 0x7) << 8)
#define LL2C_SRCAD_CTL(ll)  ((((ll) >> 20) & 0x3) << 5)
#define LL2C_DSTAD_CTL(ll)  ((((ll) >> 18) & 0x3) << 3)
#define LL2C_SRC_SEL(ll)    ((((ll) >> 17) & 0x1) << 2)
#define LL2C_DST_SEL(ll)    ((((ll) >> 16) & 0x1) << 1)

#define LLP2CSR(xll) LL2C_DMAFF_TH(xll) | LL2C_TCMASK(xll) | LL2C_SRC_WIDTH(xll) | LL2C_DST_WIDTH(xll) | LL2C_SRCAD_CTL(xll) |  LL2C_DSTAD_CTL(xll) | LL2C_SRC_SEL(xll) | LL2C_DST_SEL(xll)

#define SDHC_CARD       2
#define SD_CARD         1
#define MMC_CARD        0

/* default config */
#define LLP_CTL     (LLPC_READ | LLPC_SRC_FIX | LLPC_DST_INC | LLPC_SRC_W32 | LLPC_DST_W32)
#define CSR_CFG     (CSR_BURST_4 | CSR_READ | CSR_HWH | CSR_CH_EN)
#define SDC_CFG     (BLK_SIZE_512 | DATA_READ | DATA_DMA | DATA_EN)

#define STATUS_TX_END   (STATUS_DATA_TIMEOUT | STATUS_DATA_END | STATUS_DATA_CRC_FAIL)
#define STATUS_TX_OK    (STATUS_DATA_END | STATUS_DATA_CRC_OK)


__text__
static int _sdc_sendcmd (int cmd, int arg, unsigned int *resp)
{
    unsigned int status;

    cmd |= CMD_NEED_RSP;

    while (1) {
        /* clear command relative bits of status register */
        writel (0x7ff, SDC_CLEAR_REG);

        /* write argument */
        writel (arg, SDC_ARGU_REG);

        /* send command */
        writel (cmd, SDC_CMD_REG);

        /* wait command done */
        while (((status = readl (SDC_STATUS_REG)) & 0xff) == 0);

        /* check status */
        if (status & STATUS_RSP_CRC_OK)
            break;
    }

    resp[0] = readl (SDC_RESPONSE0_REG);
    if (cmd & CMD_LONG_RSP) {
        resp[1] = readl (SDC_RESPONSE1_REG);
        resp[2] = readl (SDC_RESPONSE2_REG);
        resp[3] = readl (SDC_RESPONSE3_REG);
    }

    return 0;
}

__text__
int _sdc_read_sector (unsigned int start, char *buf)
{
    unsigned int status;

    if (_ctx.sdc.card.type != SDHC_CARD)
        start *= 512;

    /* wait until transfer state */
    while (1) {
        _sdc_sendcmd (CMD(13), _ctx.sdc.rca, &status);
        if (((status >> 9) & 0xf) == SD_ST_TRAN)
            break;
    }

    while (1) {
        /* stop dma transfer */
        writel (CSR_ABORT, DMA_SD_CSR);

        /* reset dma */
        writel (DMA_SD_INT, DMA_INT_TC_CLR);
        writel (DMA_SD_ERR | DMA_SD_ABT, DMA_ERRABT_CLR);

        /* setup DMAC */
        writel (SRC_HE | SRC_CH(DMA_SD) | 0x7, DMA_SD_CFG);
        writel (virt_to_phys (SDC_DATA_WINDOW_REG), DMA_SD_SrcAddr);
        writel (virt_to_phys (buf), DMA_SD_DstAddr);
        writel (SECT_SIZE >> 2, DMA_SD_SIZE);
        writel (0, DMA_SD_LLP);
        writel (CSR_CFG | LLP2CSR (LLP_CTL), DMA_SD_CSR);

        /* setup SDC */
        writel (_ctx.sdc.card.readtimeout * 2, SDC_DATA_TIMER_REG);
        writel (SECT_SIZE, SDC_DATA_LEN_REG);
        writel (SDC_DATA_READ, SDC_DATA_CTRL_REG);
        
        //_sdc_sendcmd (SECT_SIZE > 512 ? CMD (18) : CMD (17), start, &status);
        _sdc_sendcmd (CMD (17), start, &status);

        /* wait until data transfer done */
        while (((status = readl (SDC_STATUS_REG)) & STATUS_TX_END) == 0);
        if ((status & STATUS_TX_OK) == STATUS_TX_OK)
            break;
    }

    /* wait until dma done */
    while ((readl (DMA_TC) & (1 << DMA_SD)) == 0);

    /* reset dma */
    writel (DMA_SD_INT, DMA_INT_TC_CLR);
    writel (DMA_SD_ERR | DMA_SD_ABT, DMA_ERRABT_CLR);

    /* wait until transfer state */
    while (1) {
        _sdc_sendcmd (CMD(13), _ctx.sdc.rca, &status);
        if (((status >> 9) & 0xf) == SD_ST_TRAN)
            break;
    }

    /* flush data cache */
    _flush_dcache_all ();

    return 0;
}
