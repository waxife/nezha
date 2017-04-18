/**
 *  @file   ir.c
 *  @brief  ir interrupt handler 
 *  $Id: ir.c,v 1.1.1.1 2013/12/18 03:43:42 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <sys.h>
#include <mipsregs.h>
#include <math.h>
#include <cache.h>
#include <ctype.h>
#include <unistd.h>
#include <debug.h>
#include <gpio.h>
#include <serial.h>
#include <nor.h>
#include "reg_tw.h"

#include <drivers/scaler/scaler_irq.h>
#include <drivers/ir/ir.h>

//IR
#define IR_SETUP                (REG_SCALER_BASE | PAGE_1 | (0x47 << 4))    
#define IR_MASK                 (REG_SCALER_BASE | PAGE_2 | (0xE2 << 4))
#define IR_STATUS               (REG_SCALER_BASE | PAGE_2 | (0xE0 << 4))
#define IR_SYSCODE_HI           (REG_SCALER_BASE | PAGE_1 | (0x43 << 4))
#define IR_SYSCODE_LO           (REG_SCALER_BASE | PAGE_1 | (0x42 << 4))
#define IR_CODE_HI              (REG_SCALER_BASE | PAGE_1 | (0x44 << 4))
#define IR_CODE_LO              (REG_SCALER_BASE | PAGE_1 | (0x45 << 4))

#define IR_TICK_LO              (REG_SCALER_BASE | PAGE_1 | (0x40 << 4))
#define IR_TICK_HI              (REG_SCALER_BASE | PAGE_1 | (0x41 << 4))
#define IR_LAG_REPEAT           (REG_SCALER_BASE | PAGE_1 | (0x46 << 4))

#define IR_SETUP_VALUE          0x84 
#define IR_MASK_VALUE           0x7f
#define IR_SYSCODE_VALUE        0xff00
#define IR_INTCLEAR_VALUE       0x80

static unsigned char irRepeat=0;
static unsigned char irqueue[10]={0};
static unsigned char ifront=9;
static unsigned char irear=9;


void ir_event(void)
{
	unsigned short tempIRSYS;
	unsigned char tempIRCode;
	unsigned char tempIRStd;

	tempIRSYS = readb(IR_SYSCODE_HI);
	tempIRSYS <<= 8;
	tempIRSYS |= readb(IR_SYSCODE_LO );
	if(tempIRSYS==IR_SYSCODE_VALUE){
		tempIRCode = readb(IR_CODE_HI);
		tempIRStd  = readb(IR_CODE_LO);
		tempIRStd  = 0xff-tempIRStd;
		if(tempIRCode==tempIRStd){
			if(readb(IR_SETUP)&0x01){
				irRepeat++;
				if(irRepeat<4)
					return;
			}
			irear=(irear+1)%10;
			if(irear==ifront){
				if(irear==0){
					irear=10-1;//max value
				}else{
					irear=irear-1;
				}
			}else{
				irqueue[irear]=tempIRCode;
			}
			irRepeat=0;
		}
	}
}

void ir_isr(void)
{
	ir_event();
	clear_scaler_irq(IRQ_IR_PACKET);
}

int IRread(void)
{
	if(ifront==irear){
		return 0;
	} else {
		ifront=(ifront+1)%10;
		return irqueue[ifront];
	}
}

void ir_init(void)
{
	//IR
	writeb(IR_SETUP_VALUE,IR_SETUP);
	writeb((readb(IR_MASK) & IR_MASK_VALUE),IR_MASK);
	writeb(0x10, IR_TICK_LO);
	writeb(0x3b, IR_TICK_HI);
	writeb(0x9a, IR_LAG_REPEAT);
}

void ir_close(void)
{
	//writeb((readb(IR_MASK) | 0x80),IR_MASK);
	disable_scaler_irq(IRQ_IR_PACKET);
}
