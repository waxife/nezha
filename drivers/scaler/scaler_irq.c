/**
 *  @file   scaler_irq.c
 *  @brief  scaler irq entry
 *  $Id: scaler_irq.c,v 1.1.1.1 2013/12/18 03:43:54 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/08/12  dos 	New file.
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
#include "iccontrol.h" 
#include <drivers/scaler/scaler_irq.h>
#include <drivers/ir/ir.h>
#include <drivers/sarkey/sarkey.h>
#include <drivers/touch/touch.h>

/*
 * P2_E0			Scaler Interrupt Status 1
 *	[7]		R/W		IR_Packet (Write 1 Clear)
 *  [6]     R/W     NO
 *	[5]		R/W		VSYNC Lead.edge (Write 1 Clear)
 *	[4]		R/W		Time Out (Write 1 Clear)
 *	[3]		R/W		HSYNC timing Chg (Write 1 Clear)
 *	[2]		R/W		VSYNC timing Chg (Write 1 Clear)
 *	[1]		R/W		HSYNC missing (Write 1 Clear)
 *	[0]		R/W		VSYNC missing (Write 1 Clear)		
 *
 * P2_E1			Scaler Interrupt Status 2
 *	[7]		R/W		CQ_int (Write 1 Clear)
 *	[6]		R/W		NO (Write 1 Clear)
 *	[5]		R/W		NO (Write 1 Clear)
 *	[4]		R/W		NO (Write 1 Clear)
 *	[3]		R/W		NO (Write 1 Clear)
 *	[2]		R/W		SPI_DMA_DONE (Write 1 Clear)
 *	[1]		R/W		SARN_Toggle (Write 1 Clear)
 *	[0]		R/W		TP_Int (Write 1 Clear)
 *
 * P2_E2			Scaler Interrupt Mask 1
 *	[7]		R/W		Mask IR_Packet
 *	[6]		R/W		NO
 *	[5]		R/W		Mask VSYNCedg.int
 *	[4]		R/W		Mask Time Out
 *	[3]		R/W		Mask HSYNC Chg
 *	[2]		R/W		Mask VSYNC Chg
 *	[1]		R/W		Mask HSYNC miss
 *	[0]		R/W		Mask VSYNC miss
 *
 * P2_E3			Scaler Interrupt Mask 2
 *	[7]		R/W		Mask CQ_int
 *	[6]		R/W		NO
 *	[5]		R/W		NO
 *	[4]		R/W		NO
 *	[3]		R/W		NO
 *	[2]		R/W		Mask SPI_DMA_Done_Int
 *	[1]		R/W		Mask_SARN_Toggle
 *	[0]		R/W		Mask_TP_Int
 */
#define INT_STATUS_REG1				IC_ReadByte(TWIC_P2, 0xe0)
#define INT_STATUS_REG2				IC_ReadByte(TWIC_P2, 0xe1)
#define INT_MASK_REG1				IC_ReadByte(TWIC_P2, 0xe2)
#define INT_MASK_REG2				IC_ReadByte(TWIC_P2, 0xe3)

#define INT_STATUS_REG1_INT			IC_ReadByte(TWIC_P2, 0xe0)
#define INT_STATUS_REG2_INT			IC_ReadByte(TWIC_P2, 0xe1)
#define INT_MASK_REG1_INT			IC_ReadByte(TWIC_P2, 0xe2)
#define INT_MASK_REG2_INT			IC_ReadByte(TWIC_P2, 0xe3)

#define INT_STATUS_CLEAR1_INT(VAL)	do { IC_WriteByte(TWIC_P2, 0xe0, VAL); } while (0)
#define INT_STATUS_CLEAR2_INT(VAL)	do { IC_WriteByte(TWIC_P2, 0xe1, VAL); } while (0)
#define INT_MASK_SET1_INT(VAL)		do { IC_WriteByte(TWIC_P2, 0xe2, VAL); } while (0)
#define INT_MASK_SET2_INT(VAL)		do { IC_WriteByte(TWIC_P2, 0xe3, VAL); } while (0)

/* STATUS POLARITY */
#define POL1_VSYNC_MISS		(1 << 0)
#define POL1_HSYNC_MISS		(1 << 1)
#define POL1_VSYNC_CHG		(1 << 2)
#define POL1_HSYNC_CHG		(1 << 3)
#define POL1_TIME_OUT		(1 << 4)
#define POL1_VSYNC_LEAD		(1 << 5)
#define POL1_NO_ASSIGN      (1 << 6)
#define POL1_IR_PACKET		(1 << 7)

#define POL2_SAR0_TOGGLE	(1 << 0)
#define POL2_SAR1_TOGGLE	(1 << 1)
#define POL2_SPI_DMA_INT	(1 << 2)
#define POL2_NO11_ASSIGN	(1 << 3)
#define POL2_NO12_ASSIGN	(1 << 4)
#define POL2_NO13_ASSIGN	(1 << 5)
#define POL2_NO14_ASSIGN	(1 << 6)
#define POL2_CQ_INT			(1 << 7)

/* MASK POLARITY */
#define MASK1_VSYNC_MISS	(1 << 0)
#define MASK1_HSYNC_MISS	(1 << 1)
#define MASK1_VSYNC_CHG		(1 << 2)
#define MASK1_HSYNC_CHG		(1 << 3)
#define MASK1_10MS_TIME		(1 << 4)
#define MASK1_VSYNC_LEAD	(1 << 5)
#define MASK1_SPI_DMA		(1 << 6)
#define MASK1_IR_PACKET		(1 << 7)

#define MASK2_SAR0_TOGGLE	(1 << 0)
#define MASK2_SAR1_TOGGLE	(1 << 1)
#define MASK2_SHORT_VS		(1 << 2)
#define MASK2_HSYNC_CHG		(1 << 3)
#define MASK2_UART0_INT		(1 << 4)
#define MASK2_UART1_INT		(1 << 5)
#define MASK2_GPIO_INT		(1 << 6)
#define MASK2_CQ_INT		(1 << 7)

static unsigned char INT_MASK_REG1_BK = 0, INT_MASK_REG2_BK = 0;

void backup_scaler_irq (void)
{
	INT_MASK_REG1_BK = INT_MASK_REG1;
	INT_MASK_REG2_BK = INT_MASK_REG2;
}

void mute_scaler_irq (void)
{
	INT_MASK_REG1_BK = INT_MASK_REG1;
	INT_MASK_REG2_BK = INT_MASK_REG2;

	//DBG_PRINT("MUTE MASK1= 0x%bX!\r\n", INT_MASK_REG1_BK);
	//DBG_PRINT("MUTE MASK2= 0x%bX!\r\n", INT_MASK_REG2_BK);

	INT_MASK_SET1_INT(0xFF);
	INT_MASK_SET2_INT(0xFF);
}

void restore_scaler_irq (void)
{
	INT_MASK_SET1_INT(INT_MASK_REG1_BK);
	INT_MASK_SET2_INT(INT_MASK_REG2_BK);
}

void clear_scaler_irq(unsigned char irq_nr)
{
	unsigned char clear;
	if(irq_nr<8) {
		clear = (1 << irq_nr);
		INT_STATUS_CLEAR1_INT(clear);
	} else {
		clear = (1 << (irq_nr-8));
		INT_STATUS_CLEAR2_INT(clear);
	}
}

void disable_scaler_irq(unsigned char irq_nr)
{
	unsigned char mask;
	if(irq_nr<8) {
		mask = INT_MASK_REG1;
		mask |= (1 << irq_nr);
		INT_MASK_SET1_INT(mask);
		//DBG_PRINT("DISABLE MASK1= 0x%bX!\r\n", mask);
	} else {
		mask = INT_MASK_REG2;
		mask |= (1 << (irq_nr-8));
		INT_MASK_SET2_INT(mask);
		//DBG_PRINT("DISABLE MASK2= 0x%bX!\r\n", mask);
	}
}

void enable_scaler_irq(unsigned char irq_nr)
{
	unsigned char mask;
	if(irq_nr<8) {
		mask = INT_MASK_REG1;
		mask &= ~(1 << irq_nr);
		INT_MASK_SET1_INT(mask);
	} else {
		mask = INT_MASK_REG2;
		mask &= ~(1 << (irq_nr-8));
		INT_MASK_SET2_INT(mask);
	}
}

unsigned short scaler_irq_dispatch(void)
{
	unsigned char status1=0;
	unsigned char status2=0;
	unsigned char mask1=0xFF;
	unsigned char mask2=0xFF;
	
	status1 = INT_STATUS_REG1_INT;
	status2 = INT_STATUS_REG2_INT;
	mask1 = INT_MASK_REG1_INT;
	mask2 = INT_MASK_REG2_INT;

	status1 &=(~mask1);
	status2 &=(~mask2);
	return ((status2<<8)|status1);
}

void scaler_init_IRQ(void)
{
	INT_MASK_SET1_INT(0xFF);		/* all interrupt mask on */
	INT_MASK_SET2_INT(0xFF);
	INT_STATUS_CLEAR1_INT(0xFF);	/* all status clear */
	INT_STATUS_CLEAR2_INT(0xFF);
	backup_scaler_irq();
}
