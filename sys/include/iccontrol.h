/**
 *  @file   iccontrol.h
 *  @brief  head file for ic control function
 *  $Id: iccontrol.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc 	New file.
 *
 */

#ifndef	_ICCONTROL_H
#define _ICCONTROL_H

/*
 * for Terawins's i8051
 */
//extern unsigned char xdata CONFIG_REGS[];
//#define IC_WRITBYTE(PAGE, ADDR, VALUE)  CONFIG_REGS[PAGE + ADDR] = VALUE
//#define IC_READBYTE(PAGE, ADDR)         CONFIG_REGS[PAGE + ADDR]

/*
 * for Terawins's MIPS
 */
extern void IC_WriteByte(unsigned short bPage, unsigned char bAdd, unsigned char bData);
extern unsigned char IC_ReadByte(unsigned short bPage, unsigned char bAdd);
extern void twdDelay(unsigned short wLoops);

//extern void WriteRegsTable(unsigned char code* RegsTable,unsigned char trig);
//extern void ClosePanel(unsigned char uR, unsigned char uG, unsigned char uB);
//extern void OpenPanel(void);

#endif	/* _ICCONTROL_H */
