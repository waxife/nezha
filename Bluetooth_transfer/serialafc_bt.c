/**
 *  @file   serialafc.c
 *  @brief  serial driver to t300 SoC
 *  $Id: serialafc_bt.c,v 1.1 2016/07/22 10:44:45 joec Exp $
 *  $Author: joec $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/01/04  New file.
 *
 */

#include <stdio.h>
#include <sys.h>
#include <io.h>
#include <serial.h>
#include "irq.h"
#include "interrupt.h"

#define AFC_UART_BASE       0xb2400000
#define UART_BASE	        0xb2000000
#define UART_RBR(x)         ((x) + 0x00)
#define UART_THR(x)         ((x) + 0x00)
#define UART_DLL(x)	        ((x) + 0x00)
#define UART_DLH(x)         ((x) + 0x04)
#define UART_IER(x)		    ((x) + 0x04)
#define UART_IIR(x)         ((x) + 0x08)
#define UART_FCR(x)         ((x) + 0x08)
#define UART_LCR(x)			((x) + 0x0C)
#define UART_MCR(x)			((x) + 0x10)
#define UART_LSR(x)         ((x) + 0x14)
#define UART_MSR(x)         ((x) + 0x18)
#define UART_USR(x)			((x) + 0x7C)

/******************************************************************************
*
* serial_init - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/
static unsigned char fifo_w = 0, fifo_r = 0;
#define MAX_FIFO 32
static unsigned char afc_fifo[MAX_FIFO];
static unsigned char dbuf=0 ;
static int stop=0;
static unsigned char old=0 ;
static int offset=0;
char *ubuf=0;
int date_buf=0;


int afc_serial_init_bt(unsigned long uart_baud)
{
	unsigned clkdiv;
	
    writeb(0x83, UART_LCR(AFC_UART_BASE));  /* Divisor Latch */

	clkdiv = (sys_apb_clk+(8*uart_baud))/(16*uart_baud);

    writeb(clkdiv >> 8, UART_DLH(AFC_UART_BASE));
    writeb(clkdiv & 0xff, UART_DLL(AFC_UART_BASE));

	writeb(0x03, UART_LCR(AFC_UART_BASE));

    // FIFO mode enable
    writeb(0x01, UART_FCR(AFC_UART_BASE));   
   	writeb(0x22, UART_MCR(AFC_UART_BASE));
   		
	return 0;
}
void data_buf(char *c)
{
		ubuf=c;
}

int buf_switch(void)
{
		return offset;	
}
		/* 中斷 處理fifo收資料 計數 length*/
void Bluetooth()		
{		
	    while (readb(UART_LSR(AFC_UART_BASE)) & 0x01) {		    	
	   		 		dbuf = readb(UART_RBR(AFC_UART_BASE));     	
			    	ubuf[date_buf+offset]=dbuf;        	
			  	  date_buf++;
		  	
						if((dbuf== 0xD9)&&(old==0xFF)){		               	 	       
//	        	printf("Bluetooth->%d, %d, %x, \n", date_buf,offset,checksum);	
	        		writeb(0x01,0xb0403000);		           	        	    	
		       		date_buf=0;          		         	            
		 					offset+=32*1024;		 					
		 					offset%=64*1024;
		        }	        	
		        old=dbuf;  					           
 				} 					           				
}

static void afc_uart_irq_handler_bt(int irq, void *dev_id, struct pt_regs *regs)
{
		Bluetooth();		
}
int afc_uart_irq_open_bt(enum RCV_THRD thrd)
{
    int rc = 0;
    static struct irqaction uart_irq;

    uart_irq.handler = afc_uart_irq_handler_bt;
    uart_irq.flags = SA_INTERRUPT;
    uart_irq.mask = 0;
    uart_irq.name = "AFC_UART_IRQ";
    uart_irq.next = NULL;
    uart_irq.dev_id = NULL;

    writeb(0x03 | ((thrd << 6) & 0xC0), UART_FCR(AFC_UART_BASE));
    writeb(0x01, UART_IER(AFC_UART_BASE));
    rc = setup_irq(IRQ_UART_AFC, &uart_irq);
    if (rc < 0) {
        printf("Failed to setup AFC_IRQ_UART irq\n");	
        goto EXIT;
    }
    return 0;

EXIT:
    disable_irq(IRQ_UART_AFC);
    free_irq(IRQ_UART_AFC,NULL);
	return -1;  
}
int afc_uart_irq_close_bt(void)
{
    int rc = 0;

    writeb(0, UART_IER(AFC_UART_BASE));
    disable_irq(IRQ_UART_AFC);
    free_irq(IRQ_UART_AFC, NULL);

    return rc;
}

