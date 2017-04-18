/*
 *  @file	t300_ostimer.c
 *  @brief	Use OSTIMER to implement periodic timer callback functionality
 *
 *	$Id: t300_ostimer.c,v 1.2 2014/07/11 11:00:10 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *	Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *	@date	2010/07/05	gary	new file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include <gpio.h>
#include <timer.h>
#include <mipsregs.h>
#include <interrupt.h>
#include <errno.h>

#define CB_TIMER_FREQ	6

#define OSCCD (999)
static unsigned long ostimer_offset;
static unsigned long ostimer_cur;

volatile unsigned int o_jiffies = 0;

static inline void ack_ostimer(void)
{
    writel(0x01, T30X_OSTIMER_OSSR);
}

static void t300_ostimer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned int oscr_now;

	oscr_now = readl(T30X_OSTIMER_OSCR);
	do {
    	ostimer_cur += ostimer_offset;
#if 1
    	if ( oscr_now > ostimer_cur ) { /* shouldn't be happening */
	        static int overflow = 0;
	        ostimer_cur = (oscr_now + ostimer_offset);
    	    overflow ++;
	        if ( overflow>10 ) {
	            printf("ostimer drifts\n");
	            overflow=0;
    	    }
        }
#endif
    o_jiffies ++;
    } while ( ostimer_cur <= oscr_now );
	writel(ostimer_cur, T30X_OSTIMER_OSMR0);
	ack_ostimer();

	return;
}

static unsigned long cal_ostimer_offset(void)
{
    unsigned long ostimer_osccd = readl(T30X_OSTIMER_OSCCD);

    printf("t300 ostimer: check APB %lu HZ %d OSCCD %lu\n", 
           sys_apb_clk, CB_TIMER_FREQ, ostimer_osccd);
    
    /* 
     * To keep implementation easy, only 
     * APB=30Mhz, OSCCD=999 are considered for now.
     */
    if ( sys_apb_clk != 30000000 && ostimer_osccd != 999 ) {
        printf("t300 ostimer: check APB %lu HZ %d OSCCD %lu\n",
               sys_apb_clk, CB_TIMER_FREQ, ostimer_osccd);
    }

    return (sys_apb_clk/(ostimer_osccd+1))/CB_TIMER_FREQ;
}

int t300_ostimer_init(void)
{
    int rc;

    printf("request IRQ %d %p\n", IRQ_TIMER, t300_ostimer_interrupt);
    rc = request_irq(IRQ_TIMER, t300_ostimer_interrupt, 0, "ostimer", 0);
    if ( rc < 0 ) {
        printf("Cannot request ostimer %d, rc = %d\n", IRQ_TIMER, rc);
        return rc;
    }

	writel(OSCCD, T30X_OSTIMER_OSCCD);
	ostimer_offset = cal_ostimer_offset();
	ostimer_cur = readl(T30X_OSTIMER_OSCR) + ostimer_offset;
	writel(0x0e, T30X_OSTIMER_OSIMR);
	ack_ostimer();
	writel(ostimer_cur, T30X_OSTIMER_OSMR0);

	return 0;
}

