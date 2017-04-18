/**
 *  @file   c-r4k.c
 *  @brief  r4k cache
 *  $Id: c-r4k.c,v 1.3 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/05  New file.
 *
 */

/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 */

#include <config.h>
#include <stdio.h>
#include <io.h>
#include <mipsregs.h>

struct cache_desc dcache, icache;
static unsigned long dcache_size, icache_size;

#define PAGE_SIZE   4096

struct cache_desc {
	unsigned short linesz;	/* Size of line in bytes */
	unsigned short ways;	/* Number of ways */
	unsigned short sets;	/* Number of lines per set */
	unsigned int waysize;	/* Bytes per way */
	unsigned int waybit;	/* Bits to select in a cache set */
	unsigned int flags;	/* Flags describing cache properties */
};

extern struct cache_desc dcache, icache;


/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define Index_Invalidate_I      0x00
#define Index_Writeback_Inv_D   0x01
#define Index_Load_Tag_I	    0x04
#define Index_Load_Tag_D	    0x05
#define Index_Store_Tag_I       0x08
#define Index_Store_Tag_D       0x09
#define Hit_Invalidate_I        0x10
#define Hit_Invalidate_D        0x11
#define Hit_Writeback_Inv_D     0x15
#define Hit_Writeback_I         0x18
#define Hit_Writeback_D         0x19

/*
 * R4000-specific cacheops
 */
#define Create_Dirty_Excl_D     0x0d
#define Fill                    0x14

/*
 * R4000SC and R4400SC-specific cacheops
 */
#define Index_Invalidate_SI     0x02
#define Index_Writeback_Inv_SD  0x03
#define Index_Load_Tag_SI       0x06
#define Index_Load_Tag_SD       0x07
#define Index_Store_Tag_SI      0x0A
#define Index_Store_Tag_SD      0x0B
#define Create_Dirty_Excl_SD    0x0f
#define Hit_Invalidate_SI       0x12
#define Hit_Invalidate_SD       0x13
#define Hit_Writeback_Inv_SD    0x17
#define Hit_Writeback_SD        0x1b
#define Hit_Set_Virtual_SI      0x1e
#define Hit_Set_Virtual_SD      0x1f



#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	mips0					\n"	\
	"	.set	reorder"					\
	:								\
	: "i" (op), "m" (*(unsigned char *)(addr)))


__mips32__
static  void flush_icache_line(unsigned long addr)
{
	cache_op(Hit_Invalidate_I, addr);
}

__mips32__
static  void flush_dcache_line(unsigned long addr)
{
	cache_op(Hit_Writeback_Inv_D, addr);
}

__mips32__
static  void wback_dcache_line(unsigned long addr)
{
    cache_op(Hit_Writeback_D, addr);
}

__mips32__
static  void invalidate_dcache_line(unsigned long addr)
{
	cache_op(Hit_Invalidate_D, addr);
}

#define cache16_unroll32(base,op)					\
	__asm__ __volatile__(						\
	"	.set noreorder					\n"	\
	"	.set mips3					\n"	\
	"	cache %1, 0x000(%0); cache %1, 0x010(%0)	\n"	\
	"	cache %1, 0x020(%0); cache %1, 0x030(%0)	\n"	\
	"	cache %1, 0x040(%0); cache %1, 0x050(%0)	\n"	\
	"	cache %1, 0x060(%0); cache %1, 0x070(%0)	\n"	\
	"	cache %1, 0x080(%0); cache %1, 0x090(%0)	\n"	\
	"	cache %1, 0x0a0(%0); cache %1, 0x0b0(%0)	\n"	\
	"	cache %1, 0x0c0(%0); cache %1, 0x0d0(%0)	\n"	\
	"	cache %1, 0x0e0(%0); cache %1, 0x0f0(%0)	\n"	\
	"	cache %1, 0x100(%0); cache %1, 0x110(%0)	\n"	\
	"	cache %1, 0x120(%0); cache %1, 0x130(%0)	\n"	\
	"	cache %1, 0x140(%0); cache %1, 0x150(%0)	\n"	\
	"	cache %1, 0x160(%0); cache %1, 0x170(%0)	\n"	\
	"	cache %1, 0x180(%0); cache %1, 0x190(%0)	\n"	\
	"	cache %1, 0x1a0(%0); cache %1, 0x1b0(%0)	\n"	\
	"	cache %1, 0x1c0(%0); cache %1, 0x1d0(%0)	\n"	\
	"	cache %1, 0x1e0(%0); cache %1, 0x1f0(%0)	\n"	\
	"	.set mips0					\n"	\
	"	.set reorder					\n"	\
		:							\
		: "r" (base),						\
		  "i" (op));

__mips32__
static  void blast_dcache16(void)
{
	unsigned long start = KSEG0;
	unsigned long end = start + dcache.waysize;
	unsigned long ws_inc = 1UL << dcache.waybit;
	unsigned long ws_end = dcache.ways << 
	                       dcache.waybit;
	unsigned long ws, addr;

	for (ws = 0; ws < ws_end; ws += ws_inc) 
		for (addr = start; addr < end; addr += 0x200)
			cache16_unroll32(addr|ws,Index_Writeback_Inv_D);
}

__mips32__
static  void blast_icache16(void)
{
	unsigned long start = KSEG0;
	unsigned long end = start + icache.waysize;
	unsigned long ws_inc = 1UL << icache.waybit;
	unsigned long ws_end = icache.ways <<
	                       icache.waybit;
	unsigned long ws, addr;

	for (ws = 0; ws < ws_end; ws += ws_inc) 
		for (addr = start; addr < end; addr += 0x200) 
			cache16_unroll32(addr|ws,Index_Invalidate_I);
}

void flush_cache_all(void)
{
	blast_dcache16();
	blast_icache16();
}

void flush_icache_all(void)
{
    blast_icache16();
}

void flush_dcache_all(void)
{
    blast_dcache16();
}

void flush_icache_range(unsigned long start, unsigned long end)
{
	unsigned long ic_lsize = icache.linesz;
	unsigned long addr, aend;

	if (end - start > icache_size)
		blast_icache16();
	else {
		addr = start & ~(ic_lsize - 1);
		aend = (end - 1) & ~(ic_lsize - 1);
		while (1) {
			/* Hit_Invalidate_I */
			flush_icache_line(addr);
			if (addr == aend)
				break;
			addr += ic_lsize;
		}
	}
}

void dma_cache_wback_inv(unsigned long addr, int size)
{
    unsigned long end, a;
    
    if (size >= dcache_size) {
        blast_dcache16();
    } else {
        unsigned long dc_lsize = dcache.linesz;

		a = addr & ~(dc_lsize - 1);
		end = (addr + size - 1) & ~(dc_lsize - 1);
		while (1) {
			flush_dcache_line(a);	/* Hit_Writeback_Inv_D */
			if (a == end)
				break;
			a += dc_lsize;
		}
    }
    

}

void dma_cache_wback(unsigned long addr, int size)
{
	unsigned long end, a;

	if (size >= dcache_size) {
		blast_dcache16();
	} else {
		unsigned long dc_lsize = dcache.linesz;

		a = addr & ~(dc_lsize - 1);
		end = (addr + size - 1) & ~(dc_lsize - 1);
		while (1) {
			wback_dcache_line(a);	/* Hit_Writeback_D */
			if (a == end)
				break;
			a += dc_lsize;
		}
	}
}

void dma_cache_inv(unsigned long addr, int size)
{
	unsigned long end, a;

	if (size >= dcache_size) {
		blast_dcache16();
	} else {
		unsigned long dc_lsize = dcache.linesz;

		a = addr & ~(dc_lsize - 1);
		end = (addr + size - 1) & ~(dc_lsize - 1);
		while (1) {
			invalidate_dcache_line(a);	/* Hit_Invalidate_D */
			if (a == end)
				break;
			a += dc_lsize;
		}
	}
}

static int ffs(int x)
{
    int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}

__mips32__
void cache_init(void)
{
    unsigned long config1;
    int lsize;

    config1 = read_c0_config1();

	icache.flags = 0;
	lsize = ((config1 >> 19) & 7);
    icache.linesz = (lsize) ? 2 << lsize : 0;
	icache.sets = 64 << ((config1 >> 22) & 7);
	icache.ways = 1 + ((config1 >> 16) & 7);
	icache_size = icache.sets *
	              icache.ways *
	              icache.linesz;
	icache.waybit = ffs(icache_size/icache.ways) - 1;
    icache.waysize = icache_size / icache.ways;

	/*
	 * Now probe the MIPS32 / MIPS64 data cache.
	 */
	dcache.flags = 0;
	lsize = ((config1 >> 10) & 7);
    dcache.linesz = (lsize) ? 2 << lsize : 0;
	dcache.sets = 64 << ((config1 >> 13) & 7);
	dcache.ways = 1 + ((config1 >> 7) & 7);
	dcache_size = dcache.sets *
	              dcache.ways *
	              dcache.linesz;
	dcache.waybit = ffs(dcache_size/dcache.ways) - 1;
    dcache.waysize = dcache_size / dcache.ways;

    /* enable kseg0 as cacheable, write-through, write allocate */
    change_c0_config(0x7, 0);
}


