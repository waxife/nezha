/**
 *  @file   iml.h
 *  @brief  scaler register setting for cmos/cvbs input
 *  $Id: iml.h,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2009 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/12/10  Hugo  New file.
 *
 */
#ifndef __IML_H
#define __IML_H

#define PAGE0(reg)	(0xb0400000 | (reg.addr << 4))
#define PAGE1(reg)	(0xb0401000 | (reg.addr << 4))
#define PAGE2(reg)	(0xb0402000 | (reg.addr << 4))
#define VALUE(reg)	(reg.value)

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */
struct reg {
    unsigned char addr;
    unsigned char value;
} __attribute__((packed));

#pragma pack(pop)

#endif /* __IML_H */
