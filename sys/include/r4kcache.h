/**
 *  @file   r4kcache.h
 *  @brief  cache ops and basic cache function for c-r4k.c
 *  $Id $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/10  New file.
 *
 */

/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Inline assembly cache operations.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997 - 2002 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 2004 Ralf Baechle (ralf@linux-mips.org)
 */
#ifndef __ASM_R4KCACHE_H
#define __ASM_R4KCACHE_H
#include "mipsregs.h"

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
#define Hit_Fill_I              0x14

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



static inline void flush_icache_line_indexed(unsigned long addr)
{
	cache_op(Index_Invalidate_I, addr);
}

static inline void flush_dcache_line_indexed(unsigned long addr)
{
	cache_op(Index_Writeback_Inv_D, addr);
}

static inline void flush_icache_line(unsigned long addr)
{
	cache_op(Hit_Invalidate_I, addr);
}

static inline void flush_dcache_line(unsigned long addr)
{
	cache_op(Hit_Writeback_Inv_D, addr);
}

static inline void wback_dcache_line(unsigned long addr)
{
    cache_op(Hit_Writeback_D, addr);
}

static inline void invalidate_dcache_line(unsigned long addr)
{
	cache_op(Hit_Invalidate_D, addr);
}

/*
 * The next two are for badland addresses like signal trampolines.
 */
static inline void protected_flush_icache_line(unsigned long addr)
{
	__asm__ __volatile__(
		".set noreorder\n\t"
		".set mips3\n"
		"1:\tcache %0,(%1)\n"
		"2:\t.set mips0\n\t"
		".set reorder\n\t"
		".section\t__ex_table,\"a\"\n\t"
		".word\t1b,2b\n\t"
		".previous"
		:
		: "i" (Hit_Invalidate_I), "r" (addr));
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

static inline void blast_dcache16(void)
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

static inline void blast_dcache16_page(unsigned long page)
{
	unsigned long start = page;
	unsigned long end = start + PAGE_SIZE;

	do {
		cache16_unroll32(start,Hit_Writeback_Inv_D);
		start += 0x200;
	} while (start < end);
}

static inline void blast_dcache16_page_indexed(unsigned long page)
{
	unsigned long start = page;
	unsigned long end = start + PAGE_SIZE;
	unsigned long ws_inc = 1UL << dcache.waybit;
	unsigned long ws_end = dcache.ways <<
	                       dcache.waybit;
	unsigned long ws, addr;

	for (ws = 0; ws < ws_end; ws += ws_inc) 
		for (addr = start; addr < end; addr += 0x200) 
			cache16_unroll32(addr|ws,Index_Writeback_Inv_D);
}

static inline void blast_icache16(void)
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

static inline void blast_icache16_page(unsigned long page)
{
	unsigned long start = page;
	unsigned long end = start + PAGE_SIZE;

	do {
		cache16_unroll32(start,Hit_Invalidate_I);
		start += 0x200;
	} while (start < end);
}

static inline void blast_icache16_page_indexed(unsigned long page)
{
	unsigned long start = page;
	unsigned long end = start + PAGE_SIZE;
	unsigned long ws_inc = 1UL << icache.waybit;
	unsigned long ws_end = icache.ways <<
	                       icache.waybit;
	unsigned long ws, addr;

	for (ws = 0; ws < ws_end; ws += ws_inc) 
		for (addr = start; addr < end; addr += 0x200) 
			cache16_unroll32(addr|ws,Index_Invalidate_I);
}

#endif /* __ASM_R4KCACHE_H */
