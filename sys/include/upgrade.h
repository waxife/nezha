/**
 *  @file   upgrade.h
 *  @brief  header of upgrade.c
 *  $Id: upgrade.h,v 1.11 2016/03/17 02:43:51 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.11 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/04  hugo    New file.
 *
 */

#ifndef __UPGRADE_H
#define __UPGRADE_H

//#define UPGRADE_DEBUG
#ifdef UPGRADE_DEBUG
#include <stdio.h>
#include <string.h>
#include <debug.h>
#define _printf(fmt, arg...)    printf(fmt, ##arg)
#define _INFO(fmt, arg...)      INFO(fmt, ##arg)
#define _WARN(fmt, arg...)      WARN(fmt, ##arg)
#define _ERROR(fmt, arg...)     ERROR(fmt, ##arg)
#define _hexdump(data, len)     hexdump(data, len)
#else
#define _printf(fmt, arg...)
#define _INFO(fmt, arg...)
#define _WARN(fmt, arg...)
#define _ERROR(fmt, arg...)
#define _hexdump(data, len)
#endif

#define REGION_NUM          8   /* the max number of spare regions */

struct up_region {
    unsigned int    start;
    unsigned int    count;
};

struct up_fd32 {
    unsigned int    magic;
    unsigned short  mnt_cnt;        /* to check fat is valid or invalid */

    unsigned int    size;           /* file size */
    unsigned int    offset;         /* read/write file offset */
    unsigned char   flag;

    unsigned int    dir_clus;       /* cluster of parent directory */
    unsigned int    dire_index;
    unsigned int    start_clus;

    /* spare region */
    struct up_region   region[REGION_NUM];
    unsigned char   region_cnt;
};

struct upgrade_ctx {
    /* cache */
    struct _dcache {
        unsigned short ways;    /* Number of ways */
        unsigned short sets;    /* Number of lines per set */
        unsigned int waysize;   /* Bytes per way */
        unsigned int waybit;    /* Bits to select in a cache set */
    } dcache;

    /* sd */
    struct _sdc {
        unsigned int rca;

        struct _card {
            unsigned short type;
            int readtimeout;
        } card;
    } sdc;

    /* fd32 */
#define REGION_NUM  8
    struct _fd32 {
        struct _region {
            unsigned int start;     /* sector start */
            unsigned int end;       /* sector end */
        } region[REGION_NUM];
        unsigned char region_cnt;
        unsigned int  size;
    } fd;

    int   *upgrade_orders;
    struct up_fd32 *upgrade_fds;
    unsigned long time_stamp_mark;
};

extern struct upgrade_ctx _ctx;

/* for MIPS 16/32-bit instruction directives */
#define __mips16__  __attribute__((mips16))
#define __mips32__  __attribute__((nomips16, noinline))

/* section attributes */

#define __text__    __attribute__((section (".text.upgrade")))
#define __rodata__  __attribute__((section (".rodata.upgrade")))


/* Memory segments (32bit kernel mode addresses) */
#define KUSEG   0x00000000
#define KSEG0   0x80000000	// Cacheable
#define KSEG1   0xa0000000	// Uncacheable
#define KSEG2   0xc0000000

/* physical address of a KSEG0/KSEG1 address */
#define PHYSADDR(address)	    ((unsigned int)(address) & 0x1fffffff)

#define virt_to_phys(address)	PHYSADDR(address)
#define virt_to_bus(address)	PHYSADDR(address)

/* map address to kernel segment */
#define KSEG0ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG0)
#define KSEG1ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG1)
#define KSEG2ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG2)

#define phys_to_virt(address)	(void *)KSEG0ADDR(address)

/* access 32/16/8 bit Non-Cache-able (KSEG1) */
#define readb(addr)		    (*(volatile unsigned char *)(addr))
#define readw(addr)		    (*(volatile unsigned short *)(addr))
#define readl(addr)		    (*(volatile unsigned int *)(addr))

#define writeb(b,addr)		((*(volatile unsigned char *)(addr)) = (b))
#define writew(b,addr)		((*(volatile unsigned short *)(addr)) = (b))
#define writel(b,addr)		((*(volatile unsigned int *)(addr)) = (b))


/*
 * IO header
 */
/* Memory segments (32bit kernel mode addresses) */
#define KUSEG                   0x00000000
#define KSEG0                   0x80000000  // Cacheable
#define KSEG1                   0xa0000000  // Uncacheable
#define KSEG2                   0xc0000000

/* Returns the physical address of a KSEG0/KSEG1 address */
#define PHYSADDR(address)       ((unsigned int)(address) & 0x1fffffff)
#define virt_to_phys(address)   PHYSADDR(address)
#define virt_to_bus(address)    PHYSADDR(address)

/* Access 32/16/8 bit Non-Cache-able (KSEG1) */
#define readb(addr)     (*(volatile unsigned char *)(addr))
#define readw(addr)     (*(volatile unsigned short *)(addr))
#define readl(addr)     (*(volatile unsigned int *)(addr))

#define writeb(b,addr)  ((*(volatile unsigned char *)(addr)) = (b))
#define writew(b,addr)  ((*(volatile unsigned short *)(addr)) = (b))
#define writel(b,addr)  ((*(volatile unsigned int *)(addr)) = (b))

/*
 * GPIO driver
 */
#define GPIO_BASE               0xb9000000
#define GPIO_LEVEL              (GPIO_BASE + 0x00)
#define GPIO_OE                 (GPIO_BASE + 0x04)
#define GPIO_SET                (GPIO_BASE + 0x08)
#define GPIO_CLEAR              (GPIO_BASE + 0x0c)
#define GPIO_RED                (GPIO_BASE + 0x10)
#define GPIO_FED                (GPIO_BASE + 0x14)
#define GPIO_RED_STATUS         (GPIO_BASE + 0x18)
#define GPIO_FED_STATUS         (GPIO_BASE + 0x1c)
#define GPIO_INT_MASK           (GPIO_BASE + 0x24)
#define GPIO_ALTFUNC_SEL        (GPIO_BASE + 0x28)
#define GPIO_HW_TRAP_FF         (GPIO_BASE + 0x2c)
#define GPIO_SP_0               (GPIO_BASE + 0x30)
#define GPIO_SP_1               (GPIO_BASE + 0x34)
#define GPIO_SP_2               (GPIO_BASE + 0x38)
#define GPIO_CARD_SEL           (GPIO_BASE + 0x3c)

#define CARD_SEL_CF             3
#define CARD_SEL_SD             2
#define CARD_SEL_MS             1
#define CARD_SEL_XD             0

#define XD_CARDDECT             (1 << 31)
#define MS_CARDDECT             (1 << 30)
#define SD_CARDDECT             (1 << 12)
#define CF_CARDDECT             (1 << 28)

#define NOR_BLOCK_SIZE  (64 * 1024)
#define SECT_SIZE       512

/* cache */
extern void _flush_dcache_all (void);

/* uart */
void _putchar (const char c);
void _puts (const char *buf);

/* osd */
void _osd_disable (void);
void _osd_enable (void);
void _osd_draw_string (const char *str, int line);

/* sd */
int _sdc_read_sector (unsigned int start, char *buf);
/*nor*/
int _nor_erase (int addr);
int _chip_erase (void);

int _nor_write_sector (int addr, char *buf);
int _nor_read_sector (int addr, char *buf);

/* upgrade entry */
void upgrade_ring(int i);
void upgrade_sequence(void);
void upgrade_dfu (void);
void upgrade_spi(void);

void Uart_Upgrade(void);

#endif /* __UPGRADE_H */
