/**
 *  @file   iccontrol.c
 *  @brief  ic control function
 *  $Id: iccontrol.c,v 1.2 2014/08/07 03:39:09 onejoe Exp $
 *  $Author: onejoe $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc 	New file.
 *
 */

#include "reg_tw.h"
#include "iccontrol.h"
#include <io.h>
#include "unistd.h"

#define REG_SCALAR_BASE 0xb0400000

void IC_WriteByte(unsigned short bPage, unsigned char bAdd, unsigned char bData)
{
    unsigned long addr = (unsigned long)bAdd;
    unsigned long page = (unsigned long)bPage;    
    unsigned long data = (unsigned long)bData;
	unsigned long Address = REG_SCALAR_BASE + page + (addr << 4);

#if 0
	writel(data, Address);
#else
	(*(volatile unsigned int *)(Address)) = data;
    IC_ReadByte(bPage, bAdd);
    usleep(10);
#endif
}

unsigned char IC_ReadByte(unsigned short bPage, unsigned char bAdd)
{
    unsigned char ret  = 0;
    unsigned long addr = (unsigned long)bAdd;
    unsigned long page = (unsigned long)bPage;    
	unsigned long Address = REG_SCALAR_BASE + page + (addr << 4);

#if 0
	ret = readb(Address);
#else
	ret = (unsigned char)(*(volatile unsigned int *)(Address));
#endif
    return ret;
}
 
void twdDelay(unsigned short wLoops)  //one loop 0.1ms
{
	if(wLoops >= 1000) {
		wLoops /= 10;
		msleep(wLoops);
	} else {
		usleep(wLoops*100);
	}
}
