/**
 *  @file   iichost.h
 *  @brief  T582 iic host driver
 *  $Id: iichost.h,v 1.2 2014/07/14 07:25:31 nick1516 Exp $
 *  $Author: nick1516 $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved. 
 *
 *  @date   2014/05/08  onejoe     New file.  
 *
 */


#ifndef	_IICHOST_H
#define _IICHOST_H

#include <io.h>

#define _iICReg 0xb0400f10L

#define _oSCL	0x80 //1000 0000
#define _iSCL	0x40 //0100 0000
#define _oSDA	0x20 //0010 0000
#define _iSDA	0x10 //0001 0000

#define _ACK	0x00
#define _nACK	0x01

//#define _iICrd		iIC_ReadByte(TWIC_P0,_iICReg)    
//#define	_iICwr(val)	iIC_WritByte(TWIC_P0,_iICReg,(val))

//#define _iICrd		CONFIG_REGS[_iICReg]
//#define	_iICwr(val)	(CONFIG_REGS[_iICReg]=val)     

#define _iICrd		readl(_iICReg)
#define	_iICwr(val)	writel(val,_iICReg)

#define	_rdSDA		((_iICrd&_iSDA)>>4)
#define _rdSCL		((_iICrd&_iSCL)>>6)

#define _wrSDA_H	_iICwr(_oSDA|_iICrd)
					
#define _wrSDA_L	_iICwr(~_oSDA&_iICrd)

#define _wrSCL_H	_iICwr(_oSCL|_iICrd)
#define _wrSCL_L	_iICwr(~_oSCL&_iICrd)

#define _SDA_H		_rdSDA!=0
#define _SDA_L		_rdSDA==0
#define _SCL_H		_rdSCL!=0
#define _SCL_L		_rdSCL==0

#define	_iIC_SPEED	0xf0		//value:1 (Fastest), 2 (slower), ... N (much slower) 0xf0  

#define	_MAX_WAIT	0x10	

#ifdef _iIC_SPEED
	#define _Delay(c) for(c = 0; c < _iIC_SPEED; c++){}
#else
	#define _Delay(c)
#endif	



void io_iic_start();
void io_iic_stop();

unsigned char io_iic_rdbyte(unsigned char ack);
unsigned char io_iic_wrbyte(unsigned char wrData);

unsigned char io_iic_wrqueue(unsigned char cDevAddr, unsigned char *arrData, unsigned char cNum);
unsigned char io_iic_rdqueue(unsigned char cDevAddr, unsigned char *arrData, unsigned char cNum);

unsigned char io_iic_wrreg(unsigned char cDevAddr, unsigned char cReg, 
						   unsigned char *arrData, unsigned char cNum);

unsigned char io_iic_rdreg(unsigned char cDevAddr, unsigned char cReg, 
						   unsigned char *arrData, unsigned char cNum);


#endif // _I51_I2C_GPIO_H

