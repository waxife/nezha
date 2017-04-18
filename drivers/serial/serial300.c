/**
 *  @file   serial300.c
 *  @brief  serial driver to t300 SoC
 *  $Id: serial300.c,v 1.4 2014/02/27 07:55:07 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.4 $
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


#define FIFO

#ifdef FIFO
#define MAX_FIFO    64
static unsigned char fifo[MAX_FIFO];
static unsigned char qf = 0, qt = 0;
#endif

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
int serial_init (unsigned long uart_baud)
{
	unsigned clkdiv;
	
    writeb(0x83, UART_LCR(UART_BASE));  /* Divisor Latch */

	clkdiv = (sys_apb_clk+(8*uart_baud))/(16*uart_baud);

    writeb(clkdiv >> 8, UART_DLH(UART_BASE));
    writeb(clkdiv & 0xff, UART_DLL(UART_BASE));

	writeb(0x03, UART_LCR(UART_BASE));

    // FIFO mode enable
    writeb(0x81, UART_FCR(UART_BASE));

#if 0
#ifdef FIFO
    qf = qt = 0;
#endif
#endif

	return 0;
}

#ifdef FIFO
static void fill_fifo(void)
{
    int q;
    q = qf+1;

    if (q >= MAX_FIFO)
        q = 0;

    while(q != qt &&  
           (readb(UART_LSR(UART_BASE)) & 0x01)) {
        
        fifo[qf] = readb(UART_RBR(UART_BASE));
        qf = q;

        q++;
        if(q >= MAX_FIFO)
            q = 0;
    }
}
#endif

void putb2(const char c)
{
    int i;

    for (i = 0;  i < 100000; i++) {
        if (readb(UART_LSR(UART_BASE)) & 0x40) /* until Trasnsmitter empty */
            break;

#ifdef FIFO
        fill_fifo();
#endif
    }

    writeb(c, UART_THR(UART_BASE));
}

void putb(const char c)
{
    if (!config_console_enable)
        return;

	if (c == '\n')
		putb2('\r');
    
    putb2(c);
}

/* non-block getb, if no character ready then return -1 */
int getb2(void)
{
#ifdef FIFO
    int c;

    if (!config_console_enable)
        return -1;

    fill_fifo();
    if (qt == qf)
        return -1;

    c = fifo[qt];
    qt++;
    if (qt >= MAX_FIFO)
        qt = 0; 

    return c;
#else
    if (!config_console_enable)
        return;

    if (readb(UART_LSR(UART_BASE)) & 0x01)
        return readb(UART_RBR(UART_BASE));
    
    return -1;
#endif
   
}

/* block getb, if no character ready then return -1 */
int getb(void)
{
    int c;
    while(1) {
        if ((c = getb2()) >= 0)
            break;
    }
	return c;
}


