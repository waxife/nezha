/**
 *  @file   serialafc.c
 *  @brief  serial driver to t300 SoC
 *  $Id: serialafc.c,v 1.6 2016/05/25 08:27:56 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.6 $
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
int afc_serial_init(unsigned long uart_baud)
{
	unsigned clkdiv;
	
    writeb(0x83, UART_LCR(AFC_UART_BASE));  /* Divisor Latch */

	clkdiv = (sys_apb_clk+(8*uart_baud))/(16*uart_baud);

    writeb(clkdiv >> 8, UART_DLH(AFC_UART_BASE));
    writeb(clkdiv & 0xff, UART_DLL(AFC_UART_BASE));

	writeb(0x03, UART_LCR(AFC_UART_BASE));

    // FIFO mode enable
    writeb(0x81, UART_FCR(AFC_UART_BASE));
    
    //Enable CTS RTS Function
    //afc_enable_CTSRTS(1);

	return 0;
}

static unsigned char fifo_w = 0, fifo_r = 0;
#define MAX_FIFO    32
static unsigned char afc_fifo[MAX_FIFO];

void afc_fill_fifo()
{
    while (readb(UART_LSR(AFC_UART_BASE)) & 0x01) {        
        afc_fifo[fifo_w++] = readb(UART_RBR(AFC_UART_BASE));
        if (fifo_w >= MAX_FIFO) {
            fifo_w = 0;
        }
    }
}

static void afc_uart_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
    afc_fill_fifo();
}

int afc_uart_irq_open(enum RCV_THRD thrd)
{
    int rc = 0;
    static struct irqaction uart_irq;

    uart_irq.handler = afc_uart_irq_handler;
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

int afc_uart_irq_close(void)
{
    int rc = 0;

    writeb(0, UART_IER(AFC_UART_BASE));
    disable_irq(IRQ_UART_AFC);
    free_irq(IRQ_UART_AFC, NULL);

    return rc;
}

/* non-block getb, if no character ready then return -1 */
int afc_getb2(void)
{
    int data;
    //afc_fill_fifo();
    if (fifo_r == fifo_w) {
        return -1;
    }else{
        data = (int)afc_fifo[fifo_r++];
        if (fifo_r >= MAX_FIFO) {
            fifo_r = 0;
        }
        return data;
    }
//  if (readb(UART_LSR(AFC_UART_BASE)) & 0x01)
//      return readb(UART_RBR(AFC_UART_BASE));
//
//  return -1;
}

/* block getb, if no character ready then return -1 */
int afc_getb(void)
{
    int c;
    while(1) {
        if ((c = afc_getb2()) >= 0)
            break;
    }
	return c;
}


void afc_putb2(const char c)
{
    int i;

    for (i = 0;  i < 100000; i++) {
        if (readb(UART_LSR(AFC_UART_BASE)) & 0x40) /* until Trasnsmitter empty */
            break;
    }

    writeb(c, UART_THR(AFC_UART_BASE));
}

void afc_putb(const char c)
{
	if (c == '\n')
		afc_putb2('\r');
    
    afc_putb2(c);
}

void afc_enable_CTSRTS(int enable)
{
	if (enable)
		writeb(0x22, UART_MCR(AFC_UART_BASE));
    else
        writeb(0x00, UART_MCR(AFC_UART_BASE));
}


//void afc_uart_writeb(unsigned char ch)
//{
//    afc_putb2(ch);
//}

//int afc_uart_readb(unsigned char *ch)
//{
//    int rc = 0;
//
//    if (fifo_cnt > 0) {
//        fifo_r = (fifo_r + 1) & (MAX_FIFO - 1);
//        ch[0] = fifo[fifo_r];
//        fifo_cnt--;
//        rc = 1;
//    }
//
//    return rc;
//}
