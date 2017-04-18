/**
 *  @file   io.h
 *  @brief  mips registers input/output functions
 *  $Id: io.h,v 1.1.1.1 2013/12/18 03:44:03 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#ifndef __IO_H__
#define __IO_H__

/*
 * Memory segments (32bit kernel mode addresses)
 */

#define KUSEG                   0x00000000
#define KSEG0                   0x80000000	// Cacheable
#define KSEG1                   0xa0000000	// Uncacheable
#define KSEG2                   0xc0000000

/*
 * Physical address mask
 */
#define PHYSMASK		0x1FFFFFFF

/*
 * Returns the physical address of a KSEG0/KSEG1 address
 */

#define PHYSADDR(address)	((unsigned int)(address) & 0x1fffffff)

#define virt_to_phys(address)	PHYSADDR(address)
#define virt_to_bus(address)	PHYSADDR(address)

/*
 * Map an address to a certain kernel segment
 */

#define KSEG0ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG0)
#define KSEG1ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG1)
#define KSEG2ADDR(address)	((unsigned int)PHYSADDR(address) | KSEG2)

#define phys_to_virt(address)	(void *)KSEG0ADDR(address)

/* 
 * Access 32/16/8 bit Non-Cache-able (KSEG1)
 */

#define readb(addr)		(*(volatile unsigned char *)(addr))
#define readw(addr)		(*(volatile unsigned short *)(addr))
#define readl(addr)		(*(volatile unsigned int *)(addr))

#define writeb(b,addr)		((*(volatile unsigned char *)(addr)) = (b))
#define writew(b,addr)		((*(volatile unsigned short *)(addr)) = (b))
#define writel(b,addr)		((*(volatile unsigned int *)(addr)) = (b))

#define lcd_writeX(b,addr)  do {\
                                (*(volatile unsigned int *)(addr)) = (b);\
                               (*(volatile unsigned int *)(addr));\
                            } while (0)

/*
 * function prototyping for cache coherent
 */
#ifndef __ASSEMBLY__
extern void dma_cache_wback_inv(unsigned long start, int size);
extern void dma_cache_wback(unsigned long start, int size);
extern void dma_cache_inv(unsigned long start, int size);
#endif /* __ASSEMBLY__ */

#endif /* __IO_H__ */
