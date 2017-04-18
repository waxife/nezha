/*
 *  @file	watchdog.c
 *  @brief	Watchdog functions
 *
 *	$Id: watchdog.c,v 1.6 2014/02/13 11:50:55 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.6 $
 *
 *	Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *	@date	2010/05/16  C.N.Yu  new file.
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
#include <watchdog.h>

#define OS_TIMER_BASE		(0xb1c00000)
#define OS_TIMER_OSMR0  	(void *)(OS_TIMER_BASE + 0x00)
#define OS_TIMER_OSMR1  	(void *)(OS_TIMER_BASE + 0x04)
#define OS_TIMER_OSMR2  	(void *)(OS_TIMER_BASE + 0x08)
#define OS_TIMER_OSMR3  	(void *)(OS_TIMER_BASE + 0x0c)	// OS Timer Match REG3
#define OS_TIMER_OSCCD  	(void *)(OS_TIMER_BASE + 0x10)
#define OS_TIMER_OSCR   	(void *)(OS_TIMER_BASE + 0x14)	// OS Timer Counter REG
#define OS_TIMER_OSWER  	(void *)(OS_TIMER_BASE + 0x18)	// OS Timer Watchdog Enable REG
#define OS_TIMER_OSIMR  	(void *)(OS_TIMER_BASE + 0x1c)	// OS Timer Interrupt Mask
#define OS_TIMER_OSSR   	(void *)(OS_TIMER_BASE + 0x20)

#define WATCHDOG_EN_VAL		(1 << 0)
#define WATCHDOG_TIME_STA	(1 << 3)
#define WATCHDOG_INT_MASK	(1 << 3)

// FREQ = APB_CLK_FREQ / (OSCCD + 1 )
#define millisec2ostimer(val)    (val * (sys_clk_src / 1000000))


/**
 * @func    watchdog_enable
 * @brief   enable the watchdog
 * @param   none
 * @return  none
 */
void watchdog_enable (void)
{
    // pass interrupt of timer3
	writel(readl(OS_TIMER_OSIMR) & ~WATCHDOG_INT_MASK, OS_TIMER_OSIMR);
    // enable watchdog
	writel(WATCHDOG_EN_VAL, OS_TIMER_OSWER);
	// write 1 clear
	writel(WATCHDOG_TIME_STA, OS_TIMER_OSSR);
}

/**
 * @func    watchdog_disable
 * @brief   disable the watchdog
 * @param   none
 * @return  none
 */
void watchdog_disable (void)
{
	// mask interrupt of timer3
	writel(readl(OS_TIMER_OSIMR) | WATCHDOG_INT_MASK, OS_TIMER_OSIMR);
    // disable watchdog
	writel(0, OS_TIMER_OSWER);
}

/**
 * @func    watchdog_keepalive
 * @brief   keep alive and setting timeout
 * @param   millisec	millisecond
 * @return  none
 */
void watchdog_keepalive (unsigned long millisec)
{
	writel(readl(OS_TIMER_OSCR) + millisec2ostimer(millisec), OS_TIMER_OSMR3);
}

/**
 * @func    watchdog_init
 * @brief   watchdog initialization
 * @param   none
 * @return  none
 */
void watchdog_init (void)
{
	if(config_watchdog_timeout >= WATCHDOG_MIN_CNT) {
		printf("The WDT timeout period is %lums and WDT will start after %us.\n",
			config_watchdog_timeout, (WATCHDOG_INIT_TIME/1000));
		watchdog_keepalive(WATCHDOG_INIT_TIME);
		watchdog_enable();
	} else if (config_watchdog_timeout > 0) {
		printf("The WDT timeout period must more the %ums.\n", WATCHDOG_MIN_CNT);
	} else {
		printf("The default does not configure the WDT.\n");
	}
}

/**
 * @func    watchdog_touch
 * @brief   keep alive and getting the timeout from CONFIG_WATCHDOG_TIMEOUT
 * @param   none
 * @return  none
 */
void watchdog_touch (void)
{
	if(config_watchdog_timeout >= WATCHDOG_MIN_CNT)
		watchdog_keepalive(config_watchdog_timeout);
}
