/**
 *  @file   scaler_irq.c
 *  @brief  scaler irq entry header
 *  $Id: scaler_irq.h,v 1.1.1.1 2013/12/18 03:43:54 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/08/12  dos 	New file.
 *
 */

#ifndef _SCALER_IRQ_H
#define _SCALER_IRQ_H

#define IRQ_VSYNC_MISS		0
#define IRQ_HSYNC_MISS		1
#define IRQ_VSYNC_CHG		2
#define IRQ_HSYNC_CHG		3
#define IRQ_TIME_OUT		4
#define IRQ_VSYNC_LEAD		5
#define IRQ_NO6_INT			6
#define IRQ_IR_PACKET   	7
#define IRQ_TP_TOGGLE		8
#define IRQ_SARN_TOGGLE		9
#define IRQ_SPI_DMA		    10
#define IRQ_NO11_INT		11
#define IRQ_NO12_INT		12
#define IRQ_NO13_INT		13
#define IRQ_NO14_INT		14
#define IRQ_CQ_INT		    15
#define IRQ_TOTAL_NUM		16

#define REG_SCALER_BASE         0xb0400000
#define PAGE_0                  0
#define PAGE_1                  0x1000
#define PAGE_2                  0x2000
#define PAGE_3                  0x3000

extern void backup_scaler_irq (void);
extern void mute_scaler_irq (void);
extern void restore_scaler_irq (void);
extern void clear_scaler_irq(unsigned char irq_nr);
extern void disable_scaler_irq(unsigned char irq_nr);
extern void enable_scaler_irq(unsigned char irq_nr);
extern unsigned short scaler_irq_dispatch(void);
extern void scaler_init_IRQ(void);

#endif	/* _TW_IRQ_H */
