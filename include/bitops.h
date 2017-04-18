/**
 *  @file   bitops.h
 *  @brief  bit operations
 *  $Id: bitops.h,v 1.1.1.1 2013/12/18 03:44:03 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/04  hugo    new file
 *
 */
#ifndef __BITOPS_H
#define __BITOPS_H

#include <io.h>

#define BITMAP0     0x00000000
#define BITMAP1     0x00000001
#define BITMAP2     0x00000003
#define BITMAP3     0x00000007
#define BITMAP4     0x0000000f
#define BITMAP5     0x0000001f
#define BITMAP6     0x0000003f
#define BITMAP7     0x0000007f
#define BITMAP8     0x000000ff
#define BITMAP9     0x000001ff
#define BITMAP10    0x000003ff
#define BITMAP11    0x000007ff
#define BITMAP12    0x00000fff
#define BITMAP13    0x00001fff
#define BITMAP14    0x00003fff
#define BITMAP15    0x00007fff
#define BITMAP16    0x0000ffff
#define BITMAP17    0x0001ffff
#define BITMAP18    0x0003ffff
#define BITMAP19    0x0007ffff
#define BITMAP20    0x000fffff
#define BITMAP21    0x001fffff
#define BITMAP22    0x003fffff
#define BITMAP23    0x007fffff
#define BITMAP24    0x00ffffff
#define BITMAP25    0x01ffffff
#define BITMAP26    0x03ffffff
#define BITMAP27    0x07ffffff
#define BITMAP28    0x0fffffff
#define BITMAP29    0x1fffffff
#define BITMAP30    0x3fffffff
#define BITMAP31    0x7fffffff
#define BITMAP32    0xffffffff
#define MASK(bits)  (BITMAP##bits)

#define REG_GET_BITS(reg, offset, bits)         ((readl(reg) >> offset) & MASK(bits))
#define REG_SET_BITS(reg, offset, bits, value)  writel(((readl(reg) & ~(MASK(bits) << offset)) | ((value & MASK(bits)) << offset)), reg)

#define REG_GET_BIT(reg, offset)                REG_GET_BITS(reg, offset, 1)
#define REG_SET_BIT(reg, offset, value)         REG_SET_BITS(reg, offset, 1, value)

#endif /* __BITOPS_H */
