/**
 *  @file   scaler.h
 *  @brief  ir sarkey header 
 *  $Id: scaler.h,v 1.2 2013/12/27 07:34:59 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _SCALER_H_
#define _SCALER_H_

#define REG_SCALER_BASE         0xb0400000
#define PAGE_0                  0
#define PAGE_1                  0x1000
#define PAGE_2                  0x2000
#define PAGE_3                  0x3000

void scaler_close (void);
int scaler_open (void);

#endif
