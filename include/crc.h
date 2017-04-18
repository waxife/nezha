/**
 *  @file   crc.h
 *  @brief  crc function prototyping
 *  $Id: crc.h,v 1.1.1.1 2013/12/18 03:44:03 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/05/10  jedy    New file.
 *
 */

#ifndef __CRC_H
#define __CRC_H

#include <stdint.h>

uint16_t crc16(uint16_t crc, uint8_t const *buffer, int len);
uint32_t crc32(uint32_t crc32, const unsigned char *s, unsigned int len);


#endif /* __CRC_H */

