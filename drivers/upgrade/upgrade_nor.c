/**
 *  @file   upgrade_nor.c
 *  @brief  nor flash driver for upgrade
 *  $Id: upgrade_nor.c,v 1.2 2014/08/04 10:07:05 nick1516 Exp $
 *  $Author: nick1516 $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2012/09/08  hugo    New file.
 *
 */

#include "upgrade.h"

#define NOR_CTRL_BASE               0xBD800000
#define NOR_RD_PORT                 (NOR_CTRL_BASE + 0x00) 
#define NOR_WR_PORT                 (NOR_CTRL_BASE + 0x04)
#define AUTO_RDSR_CFG               (NOR_CTRL_BASE + 0x08)
#define IP_STATUS                   (NOR_CTRL_BASE + 0x10)
#define NOR_GENERAL_CTRL            (NOR_CTRL_BASE + 0x14)
#define NOR_TIMING_PARA             (NOR_CTRL_BASE + 0x18)
#define SPI_CMD                     (NOR_CTRL_BASE + 0x20)
#define SPI_ADDR                    (NOR_CTRL_BASE + 0x24)
#define NOR_PPI_CMD                 (NOR_CTRL_BASE + 0x30)
#define PPI_ERASE_RDSR_ADDR         (NOR_CTRL_BASE + 0x38)
#define PPI_Pre_Idle_CMD            (NOR_CTRL_BASE + 0x3c)
#define PPI_Suspend_CMD             (NOR_CTRL_BASE + 0x40)
#define PPI_Resume_CMD              (NOR_CTRL_BASE + 0x44)
#define PPI_Suspend_Latency_CMD     (NOR_CTRL_BASE + 0x48)
#define NOR_STATUS                  (NOR_CTRL_BASE + 0x4c)
#define NOR_PPI_CYCLE_1             (NOR_CTRL_BASE + 0x50)
#define NOR_PPI_CYCLE_2             (NOR_CTRL_BASE + 0x54)
#define NOR_PPI_CYCLE_3             (NOR_CTRL_BASE + 0x58)
#define NOR_PPI_CYCLE_4             (NOR_CTRL_BASE + 0x5c)
#define NOR_PPI_CYCLE_5             (NOR_CTRL_BASE + 0x60)
#define NOR_PPI_CYCLE_6             (NOR_CTRL_BASE + 0x64)
#define NOR_PPI_CYCLE_7             (NOR_CTRL_BASE + 0x68)
#define PPI_PROGRAM_DATA_CYCLE      (NOR_CTRL_BASE + 0x6c)

/* IP_STATUS */
#define RD_Port_Valid               (1 << 0)
#define CMD_Done                    (1 << 1)
#define SPI_PPI_Sel                 (1 << 3)

/* NOR_GENERAL_CTRL */
#define RESET_WRITE_POINT           (1 << 8)

/* SPI_CMD */
#define OPC_PIORD                   (0x03 << 24)
#define OPC_RDID                    (0x9f << 24)    //CFI Data
#define OPC_WREN                    (0x06 << 24)
#define OPC_SECERASE                (0xd8 << 24)
#define OPC_CHIPERASE               (0xc7 << 24)
#define OPC_RDSR                    (0x05 << 24)
#define OPC_WRITE                   (0x02 << 24)

#define SPI_CMD_TYPE_RD             (0 << 22)
#define SPI_CMD_TYPE_BYTEWR         (1 << 22)
#define SPI_CMD_TYPE_PAGEWR         (2 << 22)
#define SPI_CMD_TYPE_ERASE          (3 << 22)

#define AUTO_RDSR_EN                (1 << 21)
#define AUTO_RDSR_DIS               (0 << 21)

#define ADDR_CYCLE_0                (0 << 18)
#define ADDR_CYCLE_1                (1 << 18)
#define ADDR_CYCLE_2                (2 << 18)
#define ADDR_CYCLE_3                (3 << 18)
#define ADDR_CYCLE_4                (4 << 18)
#define ADDR_CYCLE_5                (5 << 18)
#define ADDR_CYCLE_6                (6 << 18)
#define ADDR_CYCLE_7                (7 << 18)

#define CMD_SPI_PIORD       (OPC_PIORD | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_3)
#define CMD_SPI_BYTEWR      (OPC_WRITE | SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_PAGEWR      (OPC_WRITE | SPI_CMD_TYPE_PAGEWR | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_WREN        (OPC_WREN | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0)
#define CMD_SPI_SECERASE    (OPC_SECERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_CHIPERASE   (OPC_CHIPERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_0)
#define CMD_SPI_RDID        (SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)

/* NOR_STATUS */
#define ERASE_WR_OK_INT     (1 << 0)
#define ERASE_WR_ERR_INT    (1 << 1)

#ifndef UPGRADE_DEBUG

__text__
int _nor_erase (int addr)
{
    /* wait until cmd idle */
    while ((readl (IP_STATUS) & CMD_Done) == 0);

    /* write enable */
    writel (CMD_SPI_WREN, SPI_CMD);

    /* wait until cmd done */
    while ((readl (IP_STATUS) & CMD_Done) == 0);

    /* clear interrupt status */
    writel ((ERASE_WR_OK_INT | ERASE_WR_ERR_INT), NOR_STATUS);

    /* address to erase */
    writel (addr, SPI_ADDR);

    /* issue erase cmd */
    writel (CMD_SPI_SECERASE, SPI_CMD);

    /* wait interrupt done */
    while ((readl (NOR_STATUS) & ERASE_WR_OK_INT) == 0);

    return 0;
}

__text__
int _chip_erase (void)
{
	
    /* wait until cmd idle */
    while ((readl (IP_STATUS) & CMD_Done) == 0);

    /* write enable */
    writel (CMD_SPI_WREN, SPI_CMD);

    /* wait until cmd done */
    while ((readl (IP_STATUS) & CMD_Done) == 0);

    /* clear interrupt status */
    writel ((ERASE_WR_OK_INT | ERASE_WR_ERR_INT), NOR_STATUS);

    /* issue erase cmd */
    writel (CMD_SPI_CHIPERASE, SPI_CMD);
	 

    /* wait interrupt done */
    while ((readl (NOR_STATUS) & ERASE_WR_OK_INT) == 0);

    return 0;
}








#define MIN(A,B)    ((A) < (B) ? (A) : (B))

__text__
int _nor_write_sector (int addr, char *buf)
{
    int size = SECT_SIZE;
    int i, cnt;

    while (size) {
        /* wait until cmd idle */
        while ((readl (IP_STATUS) & CMD_Done) == 0);

        /* reset buffer point */
        writel (RESET_WRITE_POINT, NOR_GENERAL_CTRL);

        /* write 32-byte data to spi port each time */
        cnt = MIN (32, size);
        for (i = 0; i < cnt; i += 4) {
            writel (*(unsigned int *)(buf + i), NOR_WR_PORT);
        }

        /* write enable */
        writel (CMD_SPI_WREN, SPI_CMD);

        /* wait until cmd done */
        while ((readl (IP_STATUS) & CMD_Done) == 0);

        /* clear interrupt status */
        writel ((ERASE_WR_OK_INT | ERASE_WR_ERR_INT), NOR_STATUS);

        /* address to write */
        writel (addr, SPI_ADDR);

        /* issue write cmd */
        writel ((CMD_SPI_PAGEWR | cnt), SPI_CMD);

        /* wait interrupt done */
        while ((readl (NOR_STATUS) & ERASE_WR_OK_INT) == 0);

        addr += cnt;
        size -= cnt;
        buf += cnt;
    }

    return 0;
}

__text__
int _nor_read_sector (int addr, char *buf)
{
    /* 4-byte alignment is required */
    int *src = (int *) KSEG0ADDR (addr);
    int *dst = (int *) buf;
    int i;

    for (i = 0; i < (SECT_SIZE / 4); i++) {
        dst[i] = src[i];
    }
    return SECT_SIZE;
}

#endif
