/**
 *  @file   upgrade.c
 *  @brief  firmware upgrade process
 *  $Id: upgrade_cache.c,v 1.1.1.1 2013/12/18 03:43:43 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/04  hugo    New file.
 *
 */

#include "upgrade.h"

/* Cache Operations available on all MIPS processors with R4000-style caches */
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

#define cache16_unroll32(base,op)                       \
    __asm__ __volatile__(                               \
    "   .set noreorder                              \n" \
    "   .set mips3                                  \n" \
    "   cache %1, 0x000(%0); cache %1, 0x010(%0)    \n" \
    "   cache %1, 0x020(%0); cache %1, 0x030(%0)    \n" \
    "   cache %1, 0x040(%0); cache %1, 0x050(%0)    \n" \
    "   cache %1, 0x060(%0); cache %1, 0x070(%0)    \n" \
    "   cache %1, 0x080(%0); cache %1, 0x090(%0)    \n" \
    "   cache %1, 0x0a0(%0); cache %1, 0x0b0(%0)    \n" \
    "   cache %1, 0x0c0(%0); cache %1, 0x0d0(%0)    \n" \
    "   cache %1, 0x0e0(%0); cache %1, 0x0f0(%0)    \n" \
    "   cache %1, 0x100(%0); cache %1, 0x110(%0)    \n" \
    "   cache %1, 0x120(%0); cache %1, 0x130(%0)    \n" \
    "   cache %1, 0x140(%0); cache %1, 0x150(%0)    \n" \
    "   cache %1, 0x160(%0); cache %1, 0x170(%0)    \n" \
    "   cache %1, 0x180(%0); cache %1, 0x190(%0)    \n" \
    "   cache %1, 0x1a0(%0); cache %1, 0x1b0(%0)    \n" \
    "   cache %1, 0x1c0(%0); cache %1, 0x1d0(%0)    \n" \
    "   cache %1, 0x1e0(%0); cache %1, 0x1f0(%0)    \n" \
    "   .set mips0                                  \n" \
    "   .set reorder                                \n" \
        :                                               \
        : "r" (base),                                   \
          "i" (op));

__mips32__ __text__
void _blast_dcache16 (void)
{
    unsigned long start = KSEG0;
    unsigned long end = start + _ctx.dcache.waysize;
    unsigned long ws_inc = 1UL << _ctx.dcache.waybit;
    unsigned long ws_end = _ctx.dcache.ways << _ctx.dcache.waybit;
    unsigned long ws, addr;

    for (ws = 0; ws < ws_end; ws += ws_inc)
        for (addr = start; addr < end; addr += 0x200)
            cache16_unroll32 (addr|ws, Index_Writeback_Inv_D);
}

__text__
void _flush_dcache_all (void)
{
    _blast_dcache16 ();
}
