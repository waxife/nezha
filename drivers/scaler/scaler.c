/**
 *  @file   scaler.c
 *  @brief  ir sarkey interrupt 
 *  $Id: scaler.c,v 1.4 2014/04/23 02:46:01 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.4 $
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

#include "irq.h"
#include "interrupt.h"

#include <drivers/scaler/scaler.h>
#include <drivers/touch/touch.h>
#include <drivers/ir/ir.h>
#include <drivers/sarkey/sarkey.h>
#include <drivers/scaler/scaler_irq.h>
#include <drivers/cq/cq.h>

static void scaler_int(int irq,void *dev_id,struct pt_regs *regs)
{
	unsigned char irq_nr=0;
	unsigned short irq_flag;
	/* Need check this function if use command queue */
	if (cq_busy ())
		return;

	/* Disable External Interrupt */
	//EX0 = DISABLE;

	irq_flag = scaler_irq_dispatch();
	

	for(irq_nr=0; irq_nr<IRQ_TOTAL_NUM; irq_nr++)
	{
		if(!(irq_flag&(1<<irq_nr)))
			continue;		
		switch(irq_nr) {
			case IRQ_VSYNC_MISS:
				break;
			
			case IRQ_HSYNC_MISS:
				break;
			
			case IRQ_VSYNC_CHG:
				break;
			
			case IRQ_HSYNC_CHG:
				break;
			
			case IRQ_TIME_OUT:
				break;
			
			case IRQ_NO6_INT:
				break;
			
			case IRQ_IR_PACKET:
				ir_isr();
				break;
			
			case IRQ_TP_TOGGLE:	
				touch_isr();
				break;
			
			case IRQ_SARN_TOGGLE:
				sarkey_isr();
				break;
			
			case IRQ_SPI_DMA:
				break;
			
			case IRQ_NO11_INT:
				break;
			
			case IRQ_NO12_INT:
				break;
			
			case IRQ_NO13_INT:
				break;
			
			case IRQ_NO14_INT:
				break;
			
			case IRQ_CQ_INT:
				cq_isr ();
				break;
		}
	}
	
	/* Enable External Interrupt */
	//EX0 = ENABLE;
}

static int scaler_opend = 0;
int scaler_open (void)
{
	int rc=0;
	static struct irqaction scaler;

	if(scaler_opend == 0) {
		scaler.handler = scaler_int;
		scaler.flags = SA_INTERRUPT;
		scaler.mask = 0;
		scaler.name = "scalerint";
		scaler.next = NULL;
		scaler.dev_id = NULL;
		rc = setup_irq(IRQ_SCALER,&scaler);
		if(rc<0)
		{
			printf("scaler_open setup_irq IRQ_SCALER  ERROR\r\n");
			goto EXIT;
		}
		touch_init();
		ir_init();
		sarkey_init();
	}
	scaler_opend = 1;
	return 0;
EXIT:
	disable_irq(IRQ_SCALER);
    free_irq(IRQ_SCALER,NULL);
	return -1;
}

void scaler_close(void)
{
	touch_close();
	sarkey_close();
	ir_close();
	disable_irq(IRQ_SCALER);
    free_irq(IRQ_SCALER,NULL);
}
