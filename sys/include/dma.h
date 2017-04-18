/**
 *  @file   dma.h
 *  @brief  AHB DMA driver header
 *  $Id: dma.h,v 1.6 2014/07/25 08:57:35 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/05  New file.
 *
 */
#ifndef __AHB_DMA_H
#define __AHB_DMA_H

/* dma register and offset */
#define DMA_BASE                0xb9800000
#define DMA_INT                 (DMA_BASE + 0x00)
#define DMA_INT_TC              (DMA_BASE + 0x04)
#define DMA_INT_TC_CLR          (DMA_BASE + 0x08)
#define DMA_INT_ERRABT          (DMA_BASE + 0x0c)
#define DMA_ERRABT_CLR          (DMA_BASE + 0x10)
#define DMA_TC                  (DMA_BASE + 0x14)
#define DMA_ERRABT              (DMA_BASE + 0x18)
#define DMA_EN					(DMA_BASE + 0x1c)
#define DMA_BUSY				(DMA_BASE + 0x20)
#define DMA_CSR                 (DMA_BASE + 0x24)
#define DMA_SYNC                (DMA_BASE + 0x28)
#define DMA_CH_CSR(ch)          (DMA_BASE + 0x100 + (0x20 * (ch)))
#define DMA_CH_CFG(ch)          (DMA_BASE + 0x104 + (0x20 * (ch)))
#define DMA_CH_SrcAddr(ch)      (DMA_BASE + 0x108 + (0x20 * (ch)))
#define DMA_CH_DstAddr(ch)      (DMA_BASE + 0x10c + (0x20 * (ch)))
#define DMA_CH_LLP(ch)          (DMA_BASE + 0x110 + (0x20 * (ch)))
#define DMA_CH_SIZE(ch)         (DMA_BASE + 0x114 + (0x20 * (ch)))


/* DMA Channel */
#define DMA_CF      0
#define DMA_SD      1
#define DMA_MS      2
#define DMA_XD      3
#define DMA_NOR     4
#define DMA_NAND    5
#define DMA_IIS     6


#define ERRABT      0x10001

/* DMA_CH_CFG */
#define SRC_HE      (1 << 7)
#define DST_HE      (1 << 13)
#define SRC_CH(ch)  ((ch) << 3)
#define DST_CH(ch)  ((ch) << 9)


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

#define LLPC_TCMASK     (1 << 28)

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

//#define LLPC_READ       (2 << 16)
//#define LLPC_WRITE      (1 << 16)

#define LLPC_READ       (0 << 16)
#define LLPC_WRITE      (0 << 16)

/* LLP Config */
#define LL2C_DMAFF_TH(ll)   ((((ll) >> 29) & 0x7) << 24)
#define LL2C_TCMASK(ll)     ((((ll) >> 28) & 0x1) << 31)
#define LL2C_SRC_WIDTH(ll)  ((((ll) >> 25) & 0x7) << 11)
#define LL2C_DST_WIDTH(ll)  ((((ll) >> 22) & 0x7) << 8)
#define LL2C_SRCAD_CTL(ll)  ((((ll) >> 20) & 0x3) << 5)
#define LL2C_DSTAD_CTL(ll)  ((((ll) >> 18) & 0x3) << 3)
#define LL2C_SRC_SEL(ll)    ((((ll) >> 17) & 0x1) << 2)
#define LL2C_DST_SEL(ll)    ((((ll) >> 16) & 0x1) << 1)

#define LLP_CTL_RD          (LLPC_READ | LLPC_SRC_FIX | LLPC_DST_INC | LLPC_SRC_W32 | LLPC_DST_W32)
#define LLP_CTL_WR          (LLPC_WRITE | LLPC_SRC_INC | LLPC_DST_FIX | LLPC_SRC_W32 | LLPC_DST_W32)
#define CSR_CFG             (CSR_BURST_4 | CSR_READ | CSR_HWH | CSR_CH_EN)
#define SDC_CFG             (BLK_SIZE_512 | DATA_READ | DATA_DMA | DATA_EN)

#define LLP2CSR(xll) LL2C_DMAFF_TH(xll) | LL2C_TCMASK(xll) | LL2C_SRC_WIDTH(xll) | LL2C_DST_WIDTH(xll) | LL2C_SRCAD_CTL(xll) |  LL2C_DSTAD_CTL(xll) | LL2C_SRC_SEL(xll) | LL2C_DST_SEL(xll)

/* DMA SD Setting */
#define DMA_SD_CFG          DMA_CH_CFG(DMA_SD)
#define DMA_SD_SrcAddr      DMA_CH_SrcAddr(DMA_SD)
#define DMA_SD_DstAddr      DMA_CH_DstAddr(DMA_SD)
#define DMA_SD_SIZE         DMA_CH_SIZE(DMA_SD)
#define DMA_SD_LLP          DMA_CH_LLP(DMA_SD)
#define DMA_SD_CSR          DMA_CH_CSR(DMA_SD)

#define DMA_SD_INT          (0x00000001 << DMA_SD)
#define DMA_SD_ERR          (0x00000001 << DMA_SD)
#define DMA_SD_ABT          (0x00010000 << DMA_SD)

#define LLP_SD_SETUP_RD(LLP, ADDR, SIZE, MORE) do { (LLP)->srcaddr = virt_to_phys (SDC_DATA_WINDOW_REG); (LLP)->dstaddr = virt_to_phys (ADDR); (LLP)->totsize = (SIZE) >> 2; (LLP)->control = LLP_CTL_RD; (LLP)->llp = (MORE) ? virt_to_phys ((LLP) + 1) : 0; } while (0)

#define DMA_SD_SETUP_RD(LLP)   do { writel (DMA_SD_INT, DMA_INT_TC_CLR); writel (DMA_SD_ERR | DMA_SD_ABT, DMA_ERRABT_CLR); writel (SRC_HE | SRC_CH (DMA_SD) | 0x6, DMA_SD_CFG); writel ((LLP)->srcaddr, DMA_SD_SrcAddr); writel ((LLP)->dstaddr, DMA_SD_DstAddr); writel ((LLP)->totsize, DMA_SD_SIZE); writel ((LLP)->llp, DMA_SD_LLP); writel (CSR_CFG | LLP2CSR (LLP_CTL_RD), DMA_SD_CSR); } while (0)

/* DMA NOR Setting */
#define DMA_NOR_CFG         DMA_CH_CFG(DMA_NOR)
#define DMA_NOR_SrcAddr     DMA_CH_SrcAddr(DMA_NOR)
#define DMA_NOR_DstAddr     DMA_CH_DstAddr(DMA_NOR)
#define DMA_NOR_SIZE        DMA_CH_SIZE(DMA_NOR)
#define DMA_NOR_LLP         DMA_CH_LLP(DMA_NOR)
#define DMA_NOR_CSR         DMA_CH_CSR(DMA_NOR)

#define DMA_NOR_INT         (0x00000001 << DMA_NOR)
#define DMA_NOR_ERR         (0x00000001 << DMA_NOR)
#define DMA_NOR_ABT         (0x00010000 << DMA_NOR)



/* dma linked-list descriptor */
struct lld {
    unsigned int    srcaddr;
    unsigned int    dstaddr;
    unsigned int    llp;
    unsigned int    control;
    unsigned int    totsize;
};

#endif /* __AHB_DMA_H */
